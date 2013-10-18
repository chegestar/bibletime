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

#include <QLabel>
#include <QtDebug>

#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "util/geticon.h"

#include "btmini.h"
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

class BtMiniModulesModelPrivate
{
public:
	BtMiniModulesModelPrivate() {;}
	~BtMiniModulesModelPrivate() {;}


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

    QLabel                                        *_indicator;
    QVector<Item>                                  _items;
    QVector<QPair<QAbstractItemModel*, QString> >  _models;
};


BtMiniModulesModel::BtMiniModulesModel(QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new BtMiniModulesModelPrivate())
{
    ;
}

BtMiniModulesModel::~BtMiniModulesModel()
{
    ;
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

	if(!index.isValid())
		return QVariant();
	if(d->indexDepth(index.internalId()) == 1)
	{
        const Item &l = d->_items[d->indexLevel(index.internalId(), 1)];
		if(role == Qt::DisplayRole)
            return l._text;
		if(role == Qt::DecorationRole)
            return util::getIcon("flag.svg");
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

void BtMiniModulesModel::addModel(QAbstractItemModel *model, QString repository)
{
	Q_D(BtMiniModulesModel);

    d->_models.append(QPair<QAbstractItemModel*, QString>(model, repository));

	for(int i = 0, s = model->rowCount(); i < s; ++i)
	{
		// language
		QModelIndex li = model->index(i, 0);
		QString language = model->data(li).toString();

		int lid = -1;
        for(int ii = 0; ii < d->_items.size(); ++ii)
		{
            if(d->_items[ii]._text == language)
			{
				lid = ii;
				break;
			}
		}

		// add new language
		if(lid == -1)
		{
            d->_items.append(Item(language));
            lid = d->_items.size() - 1;

			//qDebug() << "new lang" << d->_items.last();
		}

		// add categories
		for(int ii = 0, s = model->rowCount(li); ii < s; ++ii)
		{
			QModelIndex ci = model->index(ii, 0, li);
			QString category = model->data(ci).toString();

			int cid = -1;
            for(int iii = 0; iii < d->_items[lid]._children.size(); ++iii)
			{
                if(d->_items[lid]._children[iii]._text == category)
				{
					cid = iii;
					break;
				}
			}

			// add new category
			if(cid == -1)
			{
                d->_items[lid]._children.append(Item(category, QString(), QModelIndex(), ci.data(Qt::DecorationRole).value<QIcon>()));
                cid = d->_items[lid]._children.size() - 1;
			}

            d->_items[lid]._children[cid]._children.append(Item("<font size=\"66%\"><center><b>" + repository + "</b></center></font>"));

			// add modules
			for(int iii = 0, s = model->rowCount(ci); iii < s; ++iii)
			{
				QModelIndex mi = model->index(iii, 0, ci);
				d->_items[lid]._children[cid]._children.append(Item(QString(), repository, mi));
            }
		}
    }

    qSort(d->_items);
}

void BtMiniModulesModel::setIndicator(QWidget *w)
{
    d_ptr->_indicator = qobject_cast<QLabel*>(w);
}

void BtMiniModulesModel::updateIndicators(QModelIndex index)
{
    if(d_ptr->_indicator)
    {
        if(!index.isValid() || !index.parent().isValid())
            d_ptr->_indicator->setText(tr("Select language:"));
        else
            d_ptr->_indicator->setText(index.parent().data().toString().remove(QRegExp("<[^>]*>")));
    }
}
