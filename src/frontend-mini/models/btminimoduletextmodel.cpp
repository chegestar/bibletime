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
#include <QLabel>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QStringListModel>
#include <QtDebug>

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/cbtconfig.h"
#include "backend/cswordmodulesearch.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/cdisplayrendering.h"
#include "btglobal.h"
#include "frontend/cinfodisplay.h"

#include "btmini.h"
#include "btminimoduletextmodel.h"
#include "models/btminimodulenavigationmodel.h"
#include "view/btminilayoutdelegate.h"
#include "view/btminiview.h"
#include "ui/btminimenu.h"


QMutex BtMiniSwordMutex;

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

	struct List
	{
		List()
		{
			init();
		}

		List(QString &name)
		{
			init();

			_name = name;

			// TODO view now not supports row without items at all
			_maxEntries = 1;

			_displayOptions = CBTConfig::getDisplayOptionDefaults();
			_filterOptions = CBTConfig::getFilterOptionDefaults();

			setModule(_name);
		}

		void init()
		{
			_name = "";
			_module = 0;
			_maxEntries = 0;
			_firstEntry = 0;
			_hasScope = false;
			_hasContents = false;
		}

		/** */
		void setModule(QString module)
		{
			if(module == "[Commentary]")
				setModule(CBTConfig::get(CBTConfig::standardCommentary));
			else
				setModule(CSwordBackend::instance()->findModuleByName(module));
		}

		void setModule(CSwordModuleInfo *module)
		{
			_module = module;

			if(!_module)
				return;

			if(_module->type() == CSwordModuleInfo::Bible ||
				_module->type() == CSwordModuleInfo::Commentary)
			{
				CSwordBibleModuleInfo *bm = qobject_cast<CSwordBibleModuleInfo*>(_module);

				_firstEntry = bm->lowerBound().getIndex();
				_maxEntries = bm->upperBound().getIndex() - _firstEntry + 1;

				_displayOptions.verseNumbers = true;
			}
		}

		/** Set list contents to specified text. */
		void setContents(QString &contents)
		{
			Q_ASSERT(_name == "[Contents]");

			_hasContents = true;
			_contents = contents;

			_maxEntries = 1;
			_firstEntry = 0;
		}

		/** For sword module lists set scope of verses. */
		void setScope(sword::ListKey &list)
		{
			_hasScope = list.Count() > 0;
			_scopeMap.clear();

			if(_hasScope)
				for(int i = 0; i < list.Count(); ++i)
					_scopeMap.append(list.GetElement(i)->getIndex());
		}

		QString             _name;

		CSwordModuleInfo   *_module;

		long                _maxEntries;
		long                _firstEntry;

		DisplayOptions      _displayOptions;
		FilterOptions       _filterOptions;

		bool                _hasScope;
		QVector<int>        _scopeMap;

		bool                _hasContents;
		QVariant            _contents;
	};

    CSwordVerseKey indexToVerseKey(const QModelIndex &index) const
    {

        Q_ASSERT(indexDepth(index) == 2);

        const List *l = indexList(index);

		BtMiniSwordMutex.lock();

        CSwordVerseKey key(l->_module);

		if(l->_hasScope)
		{
			key.Headings(1);
			key.setIndex(l->_scopeMap[index.row()]);
		}
		else
		{
			key.Headings(1);
			key.setIndex(index.row() + l->_firstEntry);
		}

		BtMiniSwordMutex.unlock();

        return key;
    }

    /** Parents count. */
    inline int indexDepth(const QModelIndex &index) const
    {
        return index.isValid() ? (index.internalId() <= _lists.size() ? 1 : 2) : 0;
    }

    /** List for index. */
    const List * indexList(const QModelIndex &index) const
    {
        if(indexDepth(index) == 1)
        {
            Q_ASSERT(index.internalId() >= 0 && index.internalId() < _lists.size());
            return &_lists[index.internalId()];
        }
        else
        {
            Q_CHECK_PTR(reinterpret_cast<const List*>(index.internalId()));
            return reinterpret_cast<const List*>(index.internalId());
        }
    }

    /** Index of list data for model index. */
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

        _isSearch        = false;

		if(module == "[Search]")
		{
			o.scrollBarPolicy = Qt::ScrollBarAsNeeded;
			o.limitItems     = true;
			o.perCycle       = 1;
			o.scrollPerItem  = true;

			_isSearch        = true;
        }
        else if(module == "[Contents]")
        {
            o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
            o.limitItems      = false;
            o.perCycle        = 0;
        }
        else if(module == "[Commentary]")
        {
            o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
        }
		else
		{
			o.scrollBarPolicy = Qt::ScrollBarAlwaysOff;
			o.limitItems      = true;

			if(CBTConfig::get(CBTConfig::threadedTextRetrieving))
			{
				o.perCycle    = 3;
				o.useThread   = true;
			}
			else
			{
				o.perCycle    = 1;
			}
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

    /** Construct model from current user settings with given info text. Info is
        string with text and outer opening and closing tag (<tag attr="value">data</tag>). */
    BtMiniModuleTextModel * fromContentsInfo(const QString &info, QObject *parent) const
    {
        QString mc = info.left(info.indexOf('>') - 1).mid(info.indexOf(' ') + 1
            ).replace("\" ", "||").replace("=\"", "=");

        using namespace InfoDisplay;

        CInfoDisplay::ListInfoData list = CInfoDisplay::detectInfo(mc);
        mc = InfoDisplay::CInfoDisplay::formatInfo(list);

        if(mc.isEmpty())
            return 0;

        QStringList modules = CBTConfig::get(CBTConfig::openInfoModules);

        if(!(list.size() == 1 && list[0].first == CInfoDisplay::Footnote))
            mc = "<center><small>" + info.left(info.indexOf('<', 1)).mid(
                info.indexOf('>') + 1) + "</small></center>" + mc;

        BtMiniModuleTextModel * m = new BtMiniModuleTextModel(modules, parent);

        for(int i = 0; i < modules.size(); ++i)
        {
            if(modules[i] == "[Contents]")
            {
                m->d_func()->_lists[i].setContents(mc);
            }
            if(modules[i] == "[Commentary]")
            {
                QString place;

                foreach(CInfoDisplay::InfoData d, list)
                    if(d.first == CInfoDisplay::Key)
                        place = d.second;

                if(!place.isEmpty())
                {
                    CSwordVerseKey key(m->d_func()->_lists[i]._module);
                    m->d_func()->_lists[i].setScope(key.ParseVerseList(
                        (const char*)place.toUtf8()));
                }
            }
        }

        return m;
    }

    QList<List>                 _lists;

    BtMiniLayoutDelegate       *_ld;

    QPointer<QWidget>           _leftIndicator;
    QPointer<QAbstractButton>   _moduleIndicator;
    QPointer<QAbstractButton>   _placeIndicator;
    QPointer<QWidget>           _rightIndicator;

	QString                     _searchText;
	bool                        _isSearch;

	mutable QMutex              _mutex;
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

            if(d->_lists[parent.internalId()]._hasContents)
                return 1;

			return d->_lists[parent.internalId()]._maxEntries;
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
	case BtMini::PreviewRole:
	//	{
	//		// temporary solution
	//		return QString(" ");
	//	}
	//	break;

	case Qt::DisplayRole:
        {
            QString r;

            switch(d->indexDepth(index))
            {
            case 1:
                return d->_lists[index.internalId()]._module->name();
            case 2:
                {
					const BtMiniModuleTextModelPrivate::List *l = d->indexList(index);

                    if(l->_module)
					{
						// prevent access to sword from thread
						//QMutexLocker locker(&d->_mutex);

                        CSwordVerseKey key(d->indexToVerseKey(index));

						if(!d->_isSearch && key.getVerse() == 1)
						{
							r += "<center><b><big>" + key.book() + " " +
								QString::number(key.getChapter()) + "</big></b></center>";
						}

						if(role == BtMini::PreviewRole)
						{
							int v = key.getVerse();
							if(v > 0)
								r += QString("<center><b>%1</b></center>").arg(v);
						}
						else if(key.getBook() > 0 && key.getChapter() > 0 && key.getVerse() > 0 && 
                            key.haveText())
						{
                            using namespace Rendering;

						    CDisplayRendering render(l->_displayOptions, l->_filterOptions);
    
                            CTextRendering::KeyTreeItem::Settings settings(false, d->_isSearch ?
                                CTextRendering::KeyTreeItem::Settings::CompleteShort :
                                CTextRendering::KeyTreeItem::Settings::SimpleKey);

                            CTextRendering::KeyTree tree;

                            QList<const CSwordModuleInfo*> modules;
                            modules << l->_module;

                            QString keyName = key.key();

                            if(!d->_isSearch)
                            {
                                if(l->_module->type() == CSwordModuleInfo::Bible ||
                                    l->_module->type() == CSwordModuleInfo::Commentary)
                                {
                                    ((sword::VerseKey*)(l->_module->module()->getKey()))->Headings(1);

                                    CSwordVerseKey k1(l->_module);
                                    k1.Headings(1);
                                    k1.setKey(keyName);

                                    CTextRendering::KeyTreeItem::Settings preverse_settings(false,
                                        CTextRendering::KeyTreeItem::Settings::NoKey);

                                    if(k1.getVerse() == 1)
                                    {
                                        if (k1.getChapter() == 1)
                                        {
                                            k1.setChapter(0);
                                            k1.setVerse(0);
                                            if(k1.rawText().length() > 0)
                                            {
                                                tree.append(new Rendering::CTextRendering::KeyTreeItem(k1.key(), modules, preverse_settings));
                                            }
                                            k1.setChapter(1);
                                        }
                                        k1.setVerse(0);
                                        if(k1.rawText().length() > 0)
                                        {
                                            tree.append(new Rendering::CTextRendering::KeyTreeItem(k1.key(), modules, preverse_settings));
                                        }
                                    }
                                }
                            }
                            
                            tree.append(new Rendering::CTextRendering::KeyTreeItem(keyName, modules, settings));
                            r += render.renderKeyTree(tree);

                            qDeleteAll(tree);
                            
                            if(d->_isSearch && !d->_searchText.isEmpty())
    							r = CSwordModuleSearch::highlightSearchedText(r, d->_searchText);
                        }
                        else
                            Q_ASSERT(!d->_isSearch);
                    }

                    if(l->_hasContents)
                        r += l->_contents.toString();
                }
                break;
            }
            return r;
        }
        
    case BtMini::PlaceRole:
        if(d->indexDepth(index) == 2 && d->indexList(index)->_module)
            return d->indexToVerseKey(index).key();
        break;
        
    case BtMini::ModuleRole:
        switch(d->indexDepth(index))
        {
        case 1:
            return d->_lists[index.internalId()]._module->name();
        case 2:
            if(d->indexList(index)->_module)
                return d->indexList(index)->_module->name();
            break;
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

	BtMiniSwordMutex.lock();

    CSwordVerseKey verse(d->_lists[i]._module);
    
    verse.setKey(key);

	const BtMiniModuleTextModelPrivate::List *l = &d->_lists[i];
    const int r = verse.getIndex() - l->_firstEntry;
	
	BtMiniSwordMutex.unlock();

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
    Q_D(BtMiniModuleTextModel);

    BtMiniView *v = qobject_cast<BtMiniView *>(sender());
    QString contents = v->currentContents();

    const QString place = v->currentIndex().data(BtMini::PlaceRole).toString();
    if(!place.isEmpty())
    {
        if(contents[0] == '<')
            contents.insert(contents.indexOf(' ') + 1, "key=\"" + place + "\" ");
        else
            contents = "<verse key=\"" + place + "\">" + contents + "</verse>";
    }

    if(!contents.isEmpty())
    {
        BtMiniMenu menu;

        QFont f(menu.font());
        f.setPixelSize(f.pixelSize() * 0.65f);
        menu.setFont(f);

        BtMiniView *view = new BtMiniView(&menu);
        
        BtMiniModuleTextModel *m = d->fromContentsInfo(contents, &menu);

        if(m)
        {
            connect(view, SIGNAL(longPressed(const QModelIndex&)), m, SLOT(openMenu(const QModelIndex&)));
            connect(view, SIGNAL(shortPressed(const QModelIndex&)), m, SLOT(openContext(const QModelIndex&)));

            view->setModel(m);

            QVBoxLayout *l = new QVBoxLayout;
            l->addWidget(view);
            menu.setLayout(l);

            view->scrollTo(m->index(CBTConfig::get(CBTConfig::openInfoModule), 0));
            
            menu.exec();

            // Sync context modules to config
            QModelIndexList list = view->currentIndexes();
            QStringList modules;

            for(int i = 0; i < list.size(); ++i)
                modules.append(m->d_func()->_lists[i]._name);

            CBTConfig::set(CBTConfig::openInfoModule, view->currentLevel());
            CBTConfig::set(CBTConfig::openInfoModules, modules);

            //CBTConfig::syncConfig();
        }
    }
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
    f.setPixelSize(f.pixelSize() * 0.65);
    menu.setFont(f);

    BtMiniView *view = new BtMiniView(&menu);
    view->setInteractive(true);

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(view);
    menu.setLayout(l);

    BtBookshelfTreeModel * m = new BtBookshelfTreeModel(BtBookshelfTreeModel::Grouping(true), view);
    m->setSourceModel(CSwordBackend::instance()->model());
    m->setDisplayFormat(QList<QVariant>() << "<p>" << BtBookshelfModel::ModuleNameRole << "</p><p>"
        "<font size=\"50%\" color=\"#555555\">" << BtBookshelfModel::ModuleDescriptionRole << "</font></p>");

    view->setModel(m);

    QModelIndexList list = m->match(m->index(0, 0), BtBookshelfModel::ModuleNameRole,
	    cm, 1, Qt::MatchExactly | Qt::MatchRecursive);
	    
	if(list.size() > 0 && list[0].isValid())
	    view->scrollTo(list[0]);

    connect(view, SIGNAL(longPressed(const QModelIndex&)), this, SLOT(openModuleMenu(const QModelIndex&)));

    menu.exec();

    if(menu.wasCanceled())
        return;

    QString nm = view->currentIndex().data(BtBookshelfModel::ModuleNameRole).toString();

    if(cm == nm)
        return;

    CSwordVerseKey place(CSwordBackend::instance()->findModuleByName(cm));

    if(cm.size() > 0)
        place.setKey(works->currentIndex().data(BtMini::PlaceRole).toString());

    // Change view current module
    if(!works->model()->setData(works->currentIndex(), nm, BtMini::ModuleRole))
    {
        qDebug() << "BtMiniModuleTextModel::openModuleSelection: failed to change module";
    }
    else
    {
        // Restore module place
        place.setModule(CSwordBackend::instance()->findModuleByName(nm));
	
		QModelIndex index = keyIndex(works->currentLevel(), place.key());

		if(index.isValid())
			works->scrollTo(index);
	}
}

void BtMiniModuleTextModel::openPlaceSelection()
{
	Q_D(BtMiniModuleTextModel);

    BtMiniView *works = BtMini::findView(BtMini::worksWidget());

    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString();
    QString cp = works->currentIndex().data(BtMini::PlaceRole).toString();

    BtMiniMenu menu;

    QFont f(menu.font());
    f.setPixelSize(f.pixelSize() * 0.9);
    menu.setFont(f);

    BtMiniView *view = new BtMiniView(&menu);

    view->setInteractive(true);

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(view);
    menu.setLayout(l);

	// prevent access to sword
	//QMutexLocker locker(&d->_mutex);
	works->setSleep(true);

    BtMiniModuleNavigationModel * m = new BtMiniModuleNavigationModel(cm, view);

    view->setModel(m);

	// setup current place and scroll to proper place
	QModelIndex pi = m->keyToIndex(cp);
    view->setCurrentIndex(pi);
	while(pi.parent() != QModelIndex())
		pi = pi.parent();
	view->scrollTo(pi);

    menu.exec();

    QString np = view->currentIndex().data(BtMini::PlaceRole).toString();

    if(!menu.wasCanceled())
        works->scrollTo(keyIndex(works->currentLevel(), np));

	works->setSleep(false);
}

void BtMiniModuleTextModel::updateIndicators(const QModelIndex &index)
{
    Q_D(BtMiniModuleTextModel);

    if(d->_placeIndicator)
	{
		QString place(index.data(BtMini::PlaceRole).toString());

        // Shorten text
        if(!place.isEmpty())
        {
            CSwordVerseKey key(d->indexToVerseKey(index));
			
			BtMiniSwordMutex.lock();

            place = QString::fromLocal8Bit(key.getShortText());

			BtMiniSwordMutex.unlock();
        }

        d->_placeIndicator->setEnabled(!place.isEmpty());
        d->_placeIndicator->setText(place);
	}

    if(d->_moduleIndicator)
    {
        QString module(index.data(BtMini::ModuleRole).toString());
        d->_moduleIndicator->setText(module.isEmpty() ? "No Module" : module);
    }

    if(d->_leftIndicator)
        d->_leftIndicator->setEnabled(d->indexListId(index) > 0);

    if(d->_rightIndicator)
        d->_rightIndicator->setEnabled(d->indexListId(index) < d->_lists.size() - 1);
}

bool BtMiniModuleTextModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(BtMiniModuleTextModel);

    switch(role)
    {
    case BtMini::ModuleRole:
        {
            QModelIndex i(d->indexDepth(index) == 2 ? index.parent() : index);
            const int l = d->indexListId(i);
            d->eraseModule(l);
            d->insertModule(l, value.toString());
            emit dataChanged(i, i);
        }
        return true;
    default:
        return false;
    }
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
			&scope, 0, &BtMiniModuleTextModelPrivate::searchProgress,
            &qMakePair(dialog.data(), m));

		if(dialog->wasCanceled() || m->module()->terminateSearch)
			results = sword::ListKey();
	}
	else
	{
		CSwordModuleSearch searcher;
		bool ok = true;

		// build index if ascent
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

void BtMiniModuleTextModel::openModuleMenu(const QModelIndex &index)
{
    CSwordModuleInfo *m = static_cast<CSwordModuleInfo*>(
        index.data(BtBookshelfModel::ModulePointerRole).value<void*>());

    if(!m)
        return;

    QStringList actions;
    
    QString category(m->categoryName(m->category()));
    if(category.size() > 11)
        category = category.left(10) + "...";
    
    actions << tr("Set default ") + "\n" + category;
    
    const int r = BtMiniMenu::execMenu(actions);

    if(r == 0)
    {
        switch(m->category())
        {
        case CSwordModuleInfo::Bibles:
            CBTConfig::set(CBTConfig::standardBible, m);
            break;
        case CSwordModuleInfo::Commentaries:
            CBTConfig::set(CBTConfig::standardCommentary, m);
            break;
        case CSwordModuleInfo::Lexicons:
            CBTConfig::set(CBTConfig::standardLexicon, m);
            if(m->has(CSwordModuleInfo::HebrewDef))
                CBTConfig::set(CBTConfig::standardHebrewStrongsLexicon, m);
            if(m->has(CSwordModuleInfo::GreekDef))
                CBTConfig::set(CBTConfig::standardGreekStrongsLexicon, m);
            if(m->has(CSwordModuleInfo::HebrewParse))
                CBTConfig::set(CBTConfig::standardHebrewMorphLexicon, m);
            if(m->has(CSwordModuleInfo::GreekParse))
                CBTConfig::set(CBTConfig::standardGreekMorphLexicon, m);
            break;
        case CSwordModuleInfo::DailyDevotional:
            CBTConfig::set(CBTConfig::standardDailyDevotional, m);
            break;
        }
    }
}

void BtMiniModuleTextModel::modulesReloaded()
{
	Q_D(BtMiniModuleTextModel);

	for(int i = 0; i < d->_lists.size(); ++i)
		d->_lists[i].setModule(d->_lists[i]._name);
}
