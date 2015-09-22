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

#include <QAbstractButton>
#include <QBoxLayout>
#include <QtDebug>

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/cbtconfig.h"
#include "backend/cswordmodulesearch.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/centrydisplay.h"
#include "btglobal.h"

#include "btmini.h"
#include "btminimoduletextmodel.h"
#include "models/btminimodulenavigationmodel.h"
#include "view/btminilayoutdelegate.h"
#include "view/btminiview.h"
#include "ui/btminimenu.h"

struct List
{
    List()
    {
        _name = "";
        _module = 0;
        _maxEntries = 0;
        _firstEntry = 0;
		_hasScope = false;
    }

    List(QString &module)
    {
        _name = module;
        _module = CSwordBackend::instance()->findModuleByName(module);
        _firstEntry = 0;
		_hasScope = false;

        // \todo view now not supports row without items at all
        _maxEntries = 1;

        _displayOptions = CBTConfig::getDisplayOptionDefaults();
        _filterOptions = CBTConfig::getFilterOptionDefaults();

		if(module == "[Search]")
		{
			_displayOptions.verseNumbers = 2;
		}
		else if(_module)
            setModule(_module);
    }

    void setModule(CSwordModuleInfo *module)
    {
        if(module->type() == CSwordModuleInfo::Bible ||
			module->type() == CSwordModuleInfo::Commentary)
        {
            CSwordBibleModuleInfo *bm = qobject_cast<CSwordBibleModuleInfo*>(module);

            _firstEntry = bm->lowerBound().Index();
            _maxEntries = bm->upperBound().Index() + 1;

            _displayOptions.verseNumbers = true;
        }
    }

	/** */
	void setScope(sword::ListKey &list)
	{
		_hasScope = list.Count() > 0;

		if(_hasScope)
		{
			_scopeMap.clear();
			for(int i = 0; i < list.Count(); ++i)
				_scopeMap.append(list.GetElement(i)->Index());
		}
		else
			_scopeMap.clear();
	}
    
	QString             _name;

    CSwordModuleInfo   *_module;
    
    long                _maxEntries;
    long                _firstEntry;

    DisplayOptions      _displayOptions;
    FilterOptions       _filterOptions;

	bool                _hasScope;
	QVector<int>        _scopeMap;
};

class BtMiniModuleTextModelPrivate
{
public:
    BtMiniModuleTextModelPrivate()
    {
		_isSearch = false;
    }
    
    ~BtMiniModuleTextModelPrivate()
    {
        ;
    }

    CSwordVerseKey indexToVerseKey(const QModelIndex &index) const
    {
        Q_ASSERT(indexDepth(index) == 2);

        List *l = indexList(index);

        CSwordVerseKey key(l->_module);

		if(l->_hasScope)
		{
			key.Headings(1);
			key.Index(l->_scopeMap[index.row()]);
		}
		else
		{
			key.Headings(1);
			key.Index(index.row() + l->_firstEntry);
		}

        return key;
    }

    /** Number or parents. */
    inline int indexDepth(const QModelIndex &index) const
    {
        return index.isValid() ? (index.internalId() <= _lists.size() ? 1 : 2) : 0;
    }

    /** Supposed to call on ending indexies referenced to verses/entries. */
    List * indexList(const QModelIndex &index) const
    {
        Q_CHECK_PTR(reinterpret_cast<List*>(index.internalId()));

        return reinterpret_cast<List*>(index.internalId());
    }

    /** */
    int indexListId(const QModelIndex &index) const
    {
        if(!index.isValid())
            return -1;

        const List *l = indexList(index);
        for(int i = 0; i < _lists.size(); ++i)
            if(&_lists[i] == l)
                return i;
        
        Q_ASSERT(false);
        return -1;
    }

    /** */
    void insertModule(int i, QString module)
    {
        _lists.insert(i, List(module));

		QVector<BtMiniLayoutOption> os;

		for(int ii = 0; ii < _ld->levelOptionsCount(); ++ii)
			os.append(_ld->levelOption(ii));

        BtMiniLayoutOption o(_ld->levelOption(i));

		if(module == "[Search]")
		{
			o.allowScrollBar = true;
			o.limitItems     = true;
			o.perCycle       = 1;
			o.scrollPerItem  = true;

			_isSearch        = true;
		}
		else
		{
			o.allowScrollBar = false;
			o.limitItems     = true;
			o.perCycle       = 1;
		}

		os.insert(i, o);

		for(int ii = 0; ii < os.size(); ++ii)
			_ld->setLevelOption(ii, os[ii]);
    }

    /** */
    void eraseModule(int i)
    {
        _ld->eraseLevelOption(i);
        _lists.erase(_lists.begin() + i);
    }

	/** */
	static void searchProgress(char percent, void *data)
	{
		typedef QPair<BtMiniMenu*, CSwordModuleInfo*> Pair;
		
		Pair *p = reinterpret_cast<Pair*>(data);

		if(!p)
			return;

		BtMiniMenu *dialog = p->first;
		CSwordModuleInfo *m = p->second;

		if(dialog && m)
		{
			if(dialog->wasCanceled())
				m->module()->terminateSearch = true;
			dialog->setValue(percent);
		}
	}

    QList<List>                 _lists;

    BtMiniLayoutDelegate       *_ld;

    QPointer<QWidget>           _leftIndicator;
    QPointer<QAbstractButton>   _moduleIndicator;
    QPointer<QAbstractButton>   _placeIndicator;
    QPointer<QWidget>           _rightIndicator;

	QString                     _searchText;
	bool                        _isSearch;
};

BtMiniModuleTextModel::BtMiniModuleTextModel(QStringList &modules, QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new BtMiniModuleTextModelPrivate)
{
    Q_D(BtMiniModuleTextModel);

    d->_ld = new BtMiniLayoutDelegate(this);
    d->_ld->setPlainMode(true);

    for(int i = 0; i < modules.size(); ++i)
        d->insertModule(i, modules[i]);
}

BtMiniModuleTextModel::~BtMiniModuleTextModel()
{
    Q_D(BtMiniModuleTextModel);
    
    delete d;
}

int BtMiniModuleTextModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int BtMiniModuleTextModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const BtMiniModuleTextModel);
    
    switch(d->indexDepth(parent))
    {
    case 0:
        return d->_lists.size();
    case 1:
		{
			if(d->_lists[parent.internalId()]._hasScope)
				return d->_lists[parent.internalId()]._scopeMap.size();

			return d->_lists[parent.internalId()]._maxEntries - 
				d->_lists[parent.internalId()]._firstEntry;
		}
    }

    return 0;
}

QModelIndex BtMiniModuleTextModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const BtMiniModuleTextModel);

    switch(d->indexDepth(parent))
    {
    case 0:
        return createIndex(row, column, row);
    case 1:
        return createIndex(row, column, (void*)&d->_lists[parent.internalId()]);
    }

    return QModelIndex();
}

QModelIndex BtMiniModuleTextModel::parent(const QModelIndex &index) const
{
    Q_D(const BtMiniModuleTextModel);

    switch(d->indexDepth(index))
    {
    case 2:
        {
            const int i = d->indexListId(index);
            return createIndex(i, 0, i);
        }
    default:
        return QModelIndex();
    }
}

QVariant BtMiniModuleTextModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BtMiniModuleTextModel);

    switch(role)
    {
    case Qt::DisplayRole:
        {
            QString r;

            switch(d->indexDepth(index))
            {
            case 1:
                return d->_lists[index.internalId()]._module->name();
            case 2:
                {
                    List *l = d->indexList(index);

                    if(l->_module)
					{
						//Rendering::CEntryDisplay display() = l->_module->getDisplay();
                        Rendering::CEntryDisplay *display = &Rendering::CEntryDisplay();

                        CSwordVerseKey key(d->indexToVerseKey(index));

                        //qDebug() << "MODEL: render" << key.Index() << key.getText() << index;

                        if(key.getBook() != 0 && key.getChapter() != 0)
                        {
                            if(key.getVerse() > 0 && key.getBook() > 0 && display)
                            {
                                if(key.getVerse() == 1)
                                    r += "<center><b><big>" + key.book() + " " +
                                        QString::number(key.getChapter()) + "</big></b></center>";

								r += display->Rendering::CEntryDisplay::text(QList<const CSwordModuleInfo*>() << l->_module, 
                                    QString::fromUtf8(key.getText()),
                                    l->_displayOptions, l->_filterOptions);

								if(d->_isSearch && !d->_searchText.isEmpty())
									r = CSwordModuleSearch::highlightSearchedText(r, d->_searchText);
                            }
                        }
                    }
                }
                break;
            }
            return r;
        }
        
    case BtMini::PlaceRole:
        if(d->indexDepth(index) == 2)
        {
            List *l = d->indexList(index);

            if(l->_module)
            {
                CSwordVerseKey key(d->indexToVerseKey(index));

                //return key.key();
                return QString::fromLocal8Bit(key.getShortText());
            }
        }
        break;
        
    case BtMini::ModuleRole:
        switch(d->indexDepth(index))
        {
        case 1:
            return d->_lists[index.internalId()]._module->name();
        case 2:
            if(d->indexList(index)->_module)
                return d->indexList(index)->_module->name();
            else
                return QString();
        }
        break;
    }
    
    return QVariant();
}

bool BtMiniModuleTextModel::hasChildren(const QModelIndex &parent) const
{
    return (rowCount(parent) > 0);
}

Qt::ItemFlags BtMiniModuleTextModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QVariant BtMiniModuleTextModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
        return "Module";
    return QVariant();
}

QModelIndex BtMiniModuleTextModel::keyIndex(int i, QString key) const
{
    Q_D(const BtMiniModuleTextModel);

    if(key.isEmpty())
        return QModelIndex();

	Q_ASSERT(!d->_lists[i]._hasScope);

    CSwordVerseKey verse(d->_lists[i]._module);
    
    verse.setKey(key);

    const List *l = &d->_lists[i];
    const int r = verse.Index() - l->_firstEntry;

    if(r < 0 || r >= l->_maxEntries)
        return QModelIndex();

    return createIndex(r, 0, (void*)l);
}

void BtMiniModuleTextModel::setIndicators(QWidget *left, QWidget *module, QWidget *place, QWidget *right)
{
    Q_D(BtMiniModuleTextModel);

    d->_moduleIndicator = qobject_cast<QAbstractButton*>(module);
    Q_CHECK_PTR(d->_moduleIndicator);
    QObject::connect(module, SIGNAL(clicked()), this, SLOT(openModuleSelection()));

    d->_placeIndicator = qobject_cast<QAbstractButton*>(place);
    Q_CHECK_PTR(d->_placeIndicator);
    QObject::connect(place, SIGNAL(clicked()), this, SLOT(openPlaceSelection()));

    d->_leftIndicator  = left;
    d->_rightIndicator = right;
}

void BtMiniModuleTextModel::openContext(const QModelIndex &index)
{

}

void BtMiniModuleTextModel::openMenu(const QModelIndex &index)
{
    Q_D(BtMiniModuleTextModel);

    BtMiniView *v = qobject_cast<BtMiniView *>(sender());

    Q_CHECK_PTR(v);

    int level = v->currentLevel();

	QStringList actions;

	actions << tr("Add Left") << tr("Add Right");

	if(d->_lists.size() > 1)
		actions << tr("Clear");

    switch(BtMiniMenu::execMenu(actions))
    {
    case 0:
        {
            beginInsertRows(QModelIndex(), level, level);
            d->insertModule(level, "");
            endInsertRows();
        }
        break;
    case 1:
        {
            beginInsertRows(QModelIndex(), level + 1, level + 1);
            d->insertModule(level + 1, "");
            endInsertRows();
        }
        break;
    case 2:
        {
            beginRemoveRows(QModelIndex(), level, level);
            d->eraseModule(level);
            endRemoveRows();
        }
        break;
    }
}

void BtMiniModuleTextModel::openModuleSelection()
{
    BtMiniView *works = BtMini::findView(BtMini::worksWidget());
    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();

    BtMiniMenu menu;

    QFont f(menu.font());
    f.setPixelSize(f.pixelSize() * 0.8);
    menu.setFont(f);

    BtMiniView *view = new BtMiniView(&menu);

    view->setInteractive(true);
    view->setLevelOptions(0, 1, "<small><b><center>", "</center></b></small>");

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(view);
    menu.setLayout(l);

    BtBookshelfTreeModel * m = new BtBookshelfTreeModel("GUI/MainWindow/Docks/Bookshelf/grouping", view);
    m->setSourceModel(CSwordBackend::instance()->model());

    view->setModel(m);

	view->scrollTo(m->findModuleIndex(cm));

    menu.exec();

    QString nm = view->currentIndex().data().toString();

    if(!menu.wasCanceled() && cm != nm)
    {
        QString place = works->currentIndex().data(BtMini::PlaceRole).toString();

        if(!works->model()->setData(works->currentIndex(), nm, BtMini::ModuleRole))
            qDebug() << "BtMiniModuleTextModel::openModuleSelection: failed to change module";
        else
		{
			if(place.isEmpty())
				place = "Gen 1:1";

			QModelIndex index = keyIndex(works->currentLevel(), place);

			if(index.isValid())
				works->scrollTo(index);
		}
    }
}

void BtMiniModuleTextModel::openPlaceSelection()
{
    BtMiniView *works = BtMini::findView(BtMini::worksWidget());

    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();
    QString cp = works->currentIndex().data(BtMini::PlaceRole).toString();

    BtMiniMenu menu;

    QFont f(menu.font());
    f.setPixelSize(f.pixelSize() * 0.8);
    menu.setFont(f);

    BtMiniView *view = new BtMiniView(&menu);

    view->setInteractive(true);

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(view);
    menu.setLayout(l);

    BtMiniModuleNavigationModel * m = new BtMiniModuleNavigationModel(cm, view);

    view->setModel(m);

	// setup current place and scroll to proper place
	QModelIndex placeIndex = m->keyToIndex(cp);
    view->setCurrentIndex(placeIndex);
	while(placeIndex.parent() != QModelIndex())
		placeIndex = placeIndex.parent();
	view->scrollTo(placeIndex);

    menu.exec();

    QString np = view->currentIndex().data(BtMini::PlaceRole).toString();

    if(!menu.wasCanceled())
        works->scrollTo(keyIndex(works->currentLevel(), np));
}

void BtMiniModuleTextModel::updateIndicators(const QModelIndex &index)
{
    Q_D(BtMiniModuleTextModel);

    if(d->_placeIndicator)
	{
		QString place(index.data(BtMini::PlaceRole).toString());

		if(place.isEmpty() && d->_placeIndicator->isEnabled())
			d->_placeIndicator->setEnabled(false);
		else if(!place.isEmpty() && !d->_placeIndicator->isEnabled())
			d->_placeIndicator->setEnabled(true);

        d->_placeIndicator->setText(place);
	}

    if(d->_moduleIndicator)
    {
        QString module(index.data(BtMini::ModuleRole).toString());
        if(module.isEmpty())
            module = "No Module";
        d->_moduleIndicator->setText(module);
    }

    int i = 0;
    if(d->indexDepth(index) == 1)
        i = index.internalId();
    else
        i = d->indexListId(index);
    
    Q_ASSERT(i >= 0 && i < d->_lists.size());

    if(d->_leftIndicator)
        d->_leftIndicator->setEnabled(i > 0);
    if(d->_rightIndicator)
        d->_rightIndicator->setEnabled(i < d->_lists.size() - 1);
}

bool BtMiniModuleTextModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(BtMiniModuleTextModel);

    switch(role)
    {
    case BtMini::ModuleRole:
        switch(d->indexDepth(index))
        {
        case 2:
            {
                QModelIndex p(index.parent());
				d->eraseModule(p.internalId());
                d->insertModule(p.internalId(), value.toString());
                emit dataChanged(p, p);
            }
            return true;

        case 1:
            {
				d->eraseModule(index.internalId());
                d->insertModule(index.internalId(), value.toString());
                emit dataChanged(index, index);
            }
            return true;
        }
        break;
    }

    return false;
}

void BtMiniModuleTextModel::setSearchText(const QString &text)
{
	Q_D(BtMiniModuleTextModel);

	d->_searchText = text;
}

void BtMiniModuleTextModel::startSearch()
{
	Q_D(BtMiniModuleTextModel);

	BtMiniView *works = BtMini::findView(BtMini::worksWidget());

	QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();
	CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(cm);

	//qDebug() << "Start search" << cm << d->_searchText;

	sword::ListKey results;
	sword::ListKey scope;
	
	if(CSwordBibleModuleInfo *bm = qobject_cast<CSwordBibleModuleInfo*>(m))
	{
		sword::VerseKey key(bm->lowerBound());
		key.LowerBound(bm->lowerBound());
		key.UpperBound(bm->upperBound());
		scope = key;
	}

	if(d->_searchText.isEmpty())
		;
	else if(CBTConfig::get(CBTConfig::searchUsingSword))
	{
		QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress("Search ..."));
		dialog->show();

		m->module()->createSearchFramework();

		results = m->module()->search(d->_searchText.toUtf8(), 0, 0,
			&scope, 0, &BtMiniModuleTextModelPrivate::searchProgress, &QPair<BtMiniMenu*,
			CSwordModuleInfo*>(dialog.data(), m));

		if(dialog->wasCanceled() || m->module()->terminateSearch)
			results = sword::ListKey();
	}
	else
	{
		CSwordModuleSearch searcher;
		bool ok = true;

		// build index if abcent
		if(!m->hasIndex())
		{
			if(BtMiniMenu::execQuery("Build index for module?", QStringList() << "Yes" << "No") != 0)
				ok = false;
			else
			{
				QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress("Indexing..."));

				QObject::connect(dialog.data(), SIGNAL(canceled()), m, SLOT(cancelIndexing()));
				QObject::connect(m, SIGNAL(indexingProgress(int)), dialog.data(), SLOT(setValue(int)));

				dialog->show();

				if(!m->buildIndex() || dialog->wasCanceled())
				{
					if (m->hasIndex())
						m->deleteIndex();
					ok = false;
				}
			}
		}

		if(ok)
		{
			// set the search parameters
			searcher.setSearchedText(d->_searchText);
			searcher.setModules(QList<const CSwordModuleInfo*>() << m);
			searcher.setSearchScope(scope);

			searcher.startSearch();

			results = searcher.results()[m];
		}
	}

	Q_ASSERT(d->_lists.size() == 1 && d->_lists[0]._name == "[Search]");

	d->_lists[0]._module = results.Count() > 0 ? m : 0;
	
	d->_lists[0].setScope(results);

	QModelIndex i = index(0, 0);
	emit dataChanged(i, i);
}
