/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime Mini project. Visit 
* http://sourceforge.net/projects/bibletimemini for more information.
*
* This code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QThread>
#include <QtDebug>

#include <ftplibftpt.h>

#include "backend/config/btconfig.h"
#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/btinstallbackend.h"
#include "backend/btinstallmgr.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "util/geticon.h"

#include "btmini.h"
#include "ui/btminimenu.h"
#include "btminimodulesmodel.h"

struct Item
{
    Item() {;}
    Item(QString text, QString repository = QString(), QModelIndex index = QModelIndex(), QIcon icon = QIcon())
        : _text(text)
        , _repository(repository)
        , _index(index)
        , _icon(icon) {;}
    Item(const Item &copy)
        : _text(copy._text)
        , _index(copy._index)
        , _repository(copy._repository)
        , _icon(copy._icon)
		, _children(copy._children) {;}
    ~Item() {;}

    bool operator < (const Item &s2) const
    {
        if(_index.isValid())
            return _index.data().toString() < s2._index.data().toString();
        return _text < s2._text;
    }

    QString        _text;
    QModelIndex    _index;
    QString        _repository;
    QIcon          _icon;
    QVector<Item>  _children;
};

QDebug operator<<(QDebug dbg, const Item &item)
{
	dbg.nospace() << "Item(" << item._text << ", " << item._repository << ", " 
		<< (item._index.isValid() ? "has index, " : "no index, ") << item._icon << ", " 
		<< item._children.size() << " children)";
	return dbg.space();
}


class Thread : public QThread
{
public:
    Thread(QObject *parent, BtInstallMgr *im)
        : QThread(parent)
        , _installManager(im)
        , _download(true)
        ,_dataComplete(false) {;}
    ~Thread() {;}

    void run()
    {
        QStringList ss(BtInstallBackend::sourceNameList(_download));

        foreach(QString s, ss)
        {
            sword::InstallSource is = BtInstallBackend::source(s);

            if(_download) _installManager->refreshRemoteSource(&is);

            CSwordBackend *be = BtInstallBackend::backend(is);

            if(be->moduleList().size() == 0)
                continue;

            BtBookshelfTreeModel::Grouping g(BtBookshelfTreeModel::GROUP_LANGUAGE);
            g.push_back(BtBookshelfTreeModel::GROUP_CATEGORY);

            BtBookshelfTreeModel *m = new BtBookshelfTreeModel(g);
            m->setDisplayFormat(QList<QVariant>() << BtBookshelfModel::ModuleNameRole << "<font size='60%' color='#555555'><word-breaks/>"
                                << BtBookshelfModel::ModuleDescriptionHtmlRole << "</font>");
            m->setSourceModel(be->model());
            m->setObjectName(s);

            _data.append(m);
        }

        _dataComplete = true;
    }

    bool                          _download;
    QVector<QAbstractItemModel*>  _data;
    bool                          _dataComplete;
    BtInstallMgr                 *_installManager;
};


class BtMiniModulesModelPrivate
{
public:
    BtMiniModulesModelPrivate(BtMiniModulesModel *q)
        : q_ptr(q)
        , _installManager(new BtInstallMgr(q))
        , _indicator(0)
        , _thread(new Thread(q, _installManager))
    {
        QObject::connect(_thread, SIGNAL(finished ()), q, SLOT(downloadComplete()));
    }
    ~BtMiniModulesModelPrivate()
    {
        clear();
        if(_thread->isRunning()) _thread->terminate();
    }

	// fff      language
	//    ff    category
	//      fff module
	int indexDepth(int id) const
	{
		return id > 0xfffff ? 3 : id > 0xfff ? 2 : 1;
	}

	int index(int language, int category = -1, int module = -1) const
	{
		return (language + 1) +
			(category > -1 ? (category + 1) << (4*3) : 0) +
			(module > -1 ? (module + 1) << (4*5) : 0);
	}

	int indexLevel(int id, int level) const
	{
		return level == 3 ? ((id & 0xfff00000) >> (4*5)) - 1 : 
			level == 2 ? ((((id & 0xff000)) - 1) >> (4*3)) : (id & 0xfff) - 1;
	}

    void clear()
    {
        foreach (QAbstractItemModel *m, _models)
            delete m;
        _models.clear();
        _items.clear();
    }

    void generateSuggestions()
    {
        // add current language, Greek and Hebrew
        Item currentItem;
        Item englishItem;
        Item hebrewItem;
        Item greekItem;


        QString languageEnglish(QLocale::system().languageToString(QLocale::system().language()));
        QString language(QObject::tr(languageEnglish.toLocal8Bit()));

        bool add = false;

        foreach(Item i, _items)
        {
            if(i._text == QObject::tr("English"))
                englishItem = i, add = true;
            if(i._text == language)
                currentItem = i, add = true;
            if(i._text == QObject::tr("Hebrew"))
                hebrewItem = i, add = true;
            if(i._text == QObject::tr("Greek, Ancient (to 1453)"))
                greekItem = i, add = true;
        }

        if(add)
        {
            _items.prepend(Item("<font size=\"66%\"><center><b>" + BtMiniModulesModel::tr("Available languages:") + "</b></center></font>"));

            if(!greekItem._text.isEmpty()) _items.prepend(greekItem);
            if(!hebrewItem._text.isEmpty()) _items.prepend(hebrewItem);
            if(!englishItem._text.isEmpty() && currentItem._text != englishItem._text) _items.prepend(englishItem);
            if(!currentItem._text.isEmpty()) _items.prepend(currentItem);

            _items.prepend(Item("<font size=\"66%\"><center><b>" + BtMiniModulesModel::tr("Suggestions:") + "</b></center></font>"));
        }
    }

    void addModel(QAbstractItemModel *model)
	{
        Q_Q(BtMiniModulesModel);

        _models.append(model);

		for(int i = 0, s = model->rowCount(); i < s; ++i)
		{
			// language
			QModelIndex li = model->index(i, 0);
			QString language = li.data().toString();

			int lid = -1;
			for(int ii = 0; ii < _items.size(); ++ii)
			{
				if(_items[ii]._text == language)
				{
					lid = ii;
					break;
				}
			}

			// add new language
			if(lid == -1)
			{
                _items.append(Item(language, QString(), QModelIndex(), util::getIcon("flag.svg")));
				lid = _items.size() - 1;
			}

			// add categories
			for(int ii = 0, s = model->rowCount(li); ii < s; ++ii)
			{
				QModelIndex ci = model->index(ii, 0, li);
				QString category = model->data(ci).toString();

				int cid = -1;
				for(int iii = 0; iii < _items[lid]._children.size(); ++iii)
				{
					if(_items[lid]._children[iii]._text == category)
					{
						cid = iii;
						break;
					}
				}

				// add new category
				if(cid == -1)
				{
					_items[lid]._children.append(Item(category, QString(), QModelIndex(), ci.data(Qt::DecorationRole).value<QIcon>()));
					cid = _items[lid]._children.size() - 1;
				}

                _items[lid]._children[cid]._children.append(Item("<font size=\"66%\"><center><b>" + model->objectName() + "</b></center></font>"));

				// add modules
				for(int iii = 0, s = model->rowCount(ci); iii < s; ++iii)
				{
					QModelIndex mi = model->index(iii, 0, ci);
                    _items[lid]._children[cid]._children.append(Item(QString(), model->objectName(), mi));
				}
			}
		}

		qSort(_items);
	}

    QLabel                                        *_indicator;
    QVector<Item>                                  _items;
    QVector<QAbstractItemModel*>                   _models;

    BtInstallMgr                                  *_installManager;
    Thread                                        *_thread;

	Q_DECLARE_PUBLIC(BtMiniModulesModel);
	BtMiniModulesModel * const                     q_ptr;
};


BtMiniModulesModel::BtMiniModulesModel(QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new BtMiniModulesModelPrivate(this))
{
    Q_D(BtMiniModulesModel);

    d->_thread->_download = false;
    d->_thread->start();
    updateIndicators(QModelIndex());
}

BtMiniModulesModel::~BtMiniModulesModel()
{
	Q_D(BtMiniModulesModel);
	delete d;
}

int BtMiniModulesModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int BtMiniModulesModel::rowCount(const QModelIndex &parent) const
{
	Q_D(const BtMiniModulesModel);

    if(!parent.isValid())
        return d->_items.size();
	else if(d->indexDepth(parent.internalId()) == 1)
	{
        return d->_items[d->indexLevel(parent.internalId(), 1)]._children.size();
	}
	else if(d->indexDepth(parent.internalId()) == 2)
	{
        return d->_items[d->indexLevel(parent.internalId(), 1)]._children[
			d->indexLevel(parent.internalId(), 2)]._children.size();
	}
	else
		return 0;
}

QModelIndex BtMiniModulesModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_D(const BtMiniModulesModel);

	if(!parent.isValid())
		return createIndex(row, column, d->index(row));
	else if(d->indexDepth(parent.internalId()) == 1)
	{
		return createIndex(row, column, d->index(d->indexLevel(parent.internalId(), 1), row));
	}
	else if(d->indexDepth(parent.internalId()) == 2)
	{
		return createIndex(row, column, d->index(d->indexLevel(parent.internalId(), 1), 
			d->indexLevel(parent.internalId(), 2), row));
    }

    Q_ASSERT(false);

	return QModelIndex();
}

QModelIndex BtMiniModulesModel::parent(const QModelIndex &index) const
{
	Q_D(const BtMiniModulesModel);

	if(d->indexDepth(index.internalId()) == 1)
	{
		return QModelIndex();
	}
	else if(d->indexDepth(index.internalId()) == 2)
	{
		return createIndex(d->index(d->indexLevel(index.internalId(), 1)), 0, d->index(d->indexLevel(index.internalId(), 1)));
	}
	else
	{
		return createIndex(d->index(d->indexLevel(index.internalId(), 2)), 0, d->index(d->indexLevel(index.internalId(), 1), 
			d->indexLevel(index.internalId(), 2)));
	}
}

QVariant BtMiniModulesModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BtMiniModulesModel);

    Q_ASSERT(index.model() == this);

	if(!index.isValid() || d->_items.size() == 0)
		return QVariant();
	if(d->indexDepth(index.internalId()) == 1)
	{
        const Item &l = d->_items[d->indexLevel(index.internalId(), 1)];
		if(role == Qt::DisplayRole)
            return l._text;
        if(role == Qt::DecorationRole)
            return l._icon;
	}
	if(d->indexDepth(index.internalId()) == 2)
	{
        const Item &c = d->_items[d->indexLevel(index.internalId(), 1)]._children[d->indexLevel(index.internalId(), 2)];
		if(role == Qt::DisplayRole)
            return c._text;
		if(role == Qt::DecorationRole)
            return c._icon;
	}
	if(d->indexDepth(index.internalId()) == 3)
	{
        const Item &m = d->_items[d->indexLevel(index.internalId(), 1)]._children[d->indexLevel(index.internalId(), 2)]._children[d->indexLevel(index.internalId(), 3)];
		if(m._index.isValid())
		{
            // protect from module installation during remote sources update
            if(d->_thread->isRunning() && role == BtBookshelfModel::ModulePointerRole)
                return QVariant();

			if(role == BtMini::RepositoryRole)
				return m._repository;
			return m._index.data(role);
		}
		else if(role == Qt::DisplayRole)
            return m._text;
    }

    return QVariant();
}

bool BtMiniModulesModel::hasChildren(const QModelIndex &parent) const
{
    return (rowCount(parent) > 0);
}

Qt::ItemFlags BtMiniModulesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QVariant BtMiniModulesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
        return "Module";
    return QVariant();
}

bool BtMiniModulesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

void BtMiniModulesModel::setIndicator(QWidget *w)
{
    d_ptr->_indicator = qobject_cast<QLabel*>(w);
}

void BtMiniModulesModel::updateIndicators(QModelIndex index)
{
    Q_D(BtMiniModulesModel);

    if(d->_thread->isRunning())
    {
        static QTimer timer;

        if(d_ptr->_indicator)
        {
            QString t(tr("Updating"));
            QString tt(d_ptr->_indicator->text());

            if(tt == t + "...") tt = t;
            else if(tt == t + "..") tt = t + "...";
            else if(tt == t + ".") tt = t + "..";
            else tt = t + ".";

            d_ptr->_indicator->setText(tt);
        }

        timer.disconnect();
        timer.setSingleShot(true);
        timer.setInterval(800);
        timer.start();
        connect(&timer, SIGNAL(timeout()), this, SLOT(updateIndicators()));
    }
    else if(d_ptr->_indicator)
    {
        if(d->_models.size() == 0)
            d_ptr->_indicator->setText(tr("Need module sources"));
        else if(!index.isValid() || !index.parent().isValid())
            d_ptr->_indicator->setText(tr("Select language:"));
        else
            d_ptr->_indicator->setText(index.parent().data().toString().remove(QRegExp("<[^>]*>")));
    }
}

void BtMiniModulesModel::backgroundDownload()
{
    Q_D(BtMiniModulesModel);

    if(d->_thread->isRunning())
        return;

    d->_thread->_download = true;
    d->_thread->start();
    updateIndicators(QModelIndex());
}

void BtMiniModulesModel::downloadComplete()
{
    Q_D(BtMiniModulesModel);

    if(d->_thread->_dataComplete)
    {
        d->_thread->_dataComplete = false;

        beginResetModel();

        d->clear();

        // add calculated sources, pass ownership to Private object
        foreach (QAbstractItemModel *m, d->_thread->_data)
            d->addModel(m);
        d->_thread->_data.clear();

        d->generateSuggestions();

        endResetModel();
    }
}
