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
#include "backend/config/btconfig.h"
#include "backend/cswordmodulesearch.h"
#include "backend/keys/cswordldkey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/cdisplayrendering.h"
#include "backend/rendering/centrydisplay.h"
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

            _displayOptions = btConfig().getDisplayOptions();
            _filterOptions = btConfig().getFilterOptions();

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
				setModule(btConfig().getDefaultSwordModuleByType("standardCommentary"));
            else if(module.contains(','))
                setModule(CSwordBackend::instance()->findModuleByName(module.section(',', 0, 0)));
			else
				setModule(CSwordBackend::instance()->findModuleByName(module));
		}

		void setModule(CSwordModuleInfo *module)
		{
			_module = module;

			if(!_module)
            {
                qDebug() << "Can't find module for list" << _name;
				return;
            }

			if(_module->type() == CSwordModuleInfo::Bible ||
				_module->type() == CSwordModuleInfo::Commentary)
			{
				CSwordBibleModuleInfo *bm = qobject_cast<CSwordBibleModuleInfo*>(_module);

				_firstEntry = bm->lowerBound().getIndex();
				_maxEntries = bm->upperBound().getIndex() - _firstEntry + 1;

                _displayOptions.verseNumbers = true;
                _displayOptions.simpleVerseNumber = true;
			}

            if(_module->type() == CSwordModuleInfo::Lexicon)
			{
				CSwordLexiconModuleInfo *lm = qobject_cast<CSwordLexiconModuleInfo*>(_module);
				_maxEntries = lm->entries().size();
			}

            if(_module->type() == CSwordModuleInfo::GenericBook)
            {
                CSwordBookModuleInfo *bm = qobject_cast<CSwordBookModuleInfo*>(_module);

                sword::TreeKeyIdx tk(*bm->tree());
                tk.root();
                tk.firstChild();

                Q_ASSERT(tk.getOffset() == 4);

                tk.setPosition(sword::BOTTOM);

                _maxEntries = tk.getOffset() / 4;
            }
		}

		/** Set list contents to specified text. */
        void setContents(QString contents)
		{
			_hasContents = true;
			_contents = contents;

			_maxEntries = 1;
			_firstEntry = 0;
		}

		/** For sword module lists set scope of verses. Scope will not be set if \list is empty. */
        void setScope(sword::ListKey list)
		{
			_hasScope = list.Count() > 0;
			_scopeMap.clear();

            if(!_module)
                return;

            if(_module->type() == CSwordModuleInfo::Lexicon)
            {
                CSwordLexiconModuleInfo *mi = qobject_cast<CSwordLexiconModuleInfo*>(_module);

                for(int i = 0; i < list.Count(); ++i)
                {
                    CSwordLDKey k(list.GetElement(i), mi);
                    _scopeMap.append(mi->entries().indexOf(k.key()));
                }
            }
            else
            {
                for(int i = 0; i < list.Count(); ++i)
                    _scopeMap.append(list.GetElement(i)->getIndex());
            }

            Q_ASSERT(!_scopeMap.contains(-1));
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

        QMutexLocker locker(&BtMiniSwordMutex);
        
		CSwordVerseKey key(l->_module);

		key.setIntros(true);
		key.setIndex(l->_hasScope ? l->_scopeMap[index.row()] : index.row() + l->_firstEntry);
		
        return key;
    }

    /** Parents count. */
    inline int indexDepth(const QModelIndex &index) const
    {
        return index.isValid() ? ((int)index.internalId() <= _lists.size() ? 1 : 2) : 0;
    }

    /** List for index. */
    const List * indexList(const QModelIndex &index) const
    {
        if(indexDepth(index) == 1)
            return &_lists[index.internalId()];
        else
        {
			const List *l = reinterpret_cast<const List*>(index.internalId());
#ifdef QT_DEBUG
			for(int i = 0; i < _lists.size(); ++i)
				if(&_lists[i] == l)
					break;
				else if(i == _lists.size() - 1)
					Q_ASSERT(false);
#endif
            return l;
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
        
		qDebug() << l->_module->name();
        Q_ASSERT(false);
        return -1;
    }

    /** */
    void insertModule(int i, QString module)
    {
		_lists.insert(i, List());

		// insert new option
		if(_lists.size() > _ld->levelOptionsCount())
		{
            QVector<BtMiniLevelOption> os;
			for(int ii = 0; ii < _ld->levelOptionsCount(); ++ii)
				os.append(_ld->levelOption(ii));

			os.insert(i, _ld->levelOption(i));

			for(int ii = 0; ii < os.size(); ++ii)
				_ld->setLevelOption(ii, os[ii]);
		}

		setupModule(i, module);
    }

    /** */
    void eraseModule(int i)
    {
        _ld->eraseLevelOption(i);
        _lists.erase(_lists.begin() + i);
    }

	/** */
	void setupModule(int i, QString module)
	{
		_lists[i] = List(module);

		BtMiniLevelOption o;

		_isSearch = false;

		if(module.isEmpty())
		{
			;
		}
		else if(module == "[Search]")
		{
			o.scrollBarPolicy = Qt::ScrollBarAsNeeded;
			o.limitItems      = true;
			o.perCycle        = 1;
			o.scrollPerItem   = true;
			o.allowStaticText = false;

			_isSearch         = true;

            _lists[i]._displayOptions.simpleVerseNumber = false;
            _lists[i]._displayOptions.introdutions = false;
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
			o.perCycle        = 1;

			if(_lists[i]._module->type() == CSwordModuleInfo::Lexicon)
				;
            else if(btConfig().value<bool>("mini/threadedTextRetrieving"))
			{
				o.perCycle    = 3;
				o.useThread   = true;
			}
		}
		
		Q_ASSERT(i < _ld->levelOptionsCount());
		_ld->setLevelOption(i, o);
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

		QStringList modules = btConfig().value<QStringList>("mini/openInfoModules", QStringList() << "[Contents]" << "[Commentary]");

        if(!(list.size() == 1 && list[0].first == CInfoDisplay::Footnote))
            mc = "<center><small>" + info.left(info.indexOf('<', 1)).mid(
                info.indexOf('>') + 1) + "</small></center>" + mc;

        BtMiniModuleTextModel * m = new BtMiniModuleTextModel(modules, parent);

        for(int i = 0; i < modules.size(); ++i)
        {
			List *l = &m->d_func()->_lists[i];

            if(modules[i] == "[Contents]")
            {
                l->setContents(mc);
            }
            if(modules[i] == "[Commentary]")
            {
                QString place;

                foreach(CInfoDisplay::InfoData d, list)
                    if(d.first == CInfoDisplay::Key)
                        place = d.second;

                if(!place.isEmpty())
                {
                    CSwordVerseKey key(l->_module);
                    l->setScope(key.parseVerseList((const char*)place.toUtf8()));
                }

				if(l->_scopeMap.size() == 0)
				{
					l->_hasScope = false;
					l->setContents(QString());
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
			const BtMiniModuleTextModelPrivate::List *l = &d->_lists[parent.internalId()];

			if(l->_hasScope)
				return l->_scopeMap.size();
            if(l->_hasContents)
                return 1;
			return l->_maxEntries;
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
		{
            const BtMiniModuleTextModelPrivate::List *list = d->indexList(index);


            if(list->_module->type() == CSwordModuleInfo::GenericBook)
            {
                CSwordBookModuleInfo *b = qobject_cast<CSwordBookModuleInfo*>(list->_module);
                CSwordTreeKey k(b->tree(), b);

                k.setIndex(index.row() * 4);

                return QString("<word-breaks/><font color='#aaaaaa'><center>%1</center></font><font size='1'><br>"
                               "&nbsp;</br></font>").arg(k.key().replace('/', ' '));
            }
            else
            {

                // there was issue with VerseKey::freshtext at simoultaneous call
                static CSwordVerseKey vk(list->_module);
                vk.setModule(list->_module);
                vk.setIndex(index.row() + list->_firstEntry);
                int v = vk.getVerse();

                if(v == 0)
                    return QString();

                QString r("<font color='#aaaaaa'>");

                if(v == 1)
                    r += QString("<center><b><font size='+1'>%1 %2</font></b></center>"
                    "<font size='1'><br>&nbsp;</br></font>").arg(vk.book()).arg(vk.getChapter());

                if(v > 0)
                    r += QString("<center>%1</center></font><font size='1'><br>&nbsp;</br>").arg(v);

                return r + "</font>";
            }
		}

	case Qt::DisplayRole:
        {
			switch(d->indexDepth(index))
			{
			case 1:
				return d->_lists[index.internalId()]._module->name();
			case 2:
				{
					QString r;
					const BtMiniModuleTextModelPrivate::List *l = d->indexList(index);

					if(l->_module && (l->_module->type() == CSwordModuleInfo::Bible ||
						l->_module->type() == CSwordModuleInfo::Commentary))
					{
						CSwordVerseKey key(d->indexToVerseKey(index));
						const int v = key.getVerse();

                        QList<const CSwordModuleInfo*> modules;
                        modules << l->_module;

                        // parallel display
                        if(l->_name.contains(','))
                        {
                            foreach(QString s, l->_name.split(',').mid(1))
                            {
                                CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(s);
                                if(m)
                                    modules << m;
                            }
                        }


                        if(!d->_isSearch && v == 1)
                            r += "<center><b><font size='+1'>" + key.book() + " " +
                                    QString::number(key.getChapter()) + "</font></b></center>";

                        if(v != 0)
                            r += Rendering::CEntryDisplay().text(modules, key.key(), l->_displayOptions, l->_filterOptions);

                        if(!d->_isSearch && v == key.getVerseMax())
                            r += "<font size='1'><br>&nbsp;</font>";
					}
					else if(l->_module && l->_module->type() == CSwordModuleInfo::Lexicon)
					{
						CSwordLexiconModuleInfo *lm = qobject_cast<CSwordLexiconModuleInfo*>(l->_module);

						Rendering::CEntryDisplay ed;
                        r += ed.text(QList<const CSwordModuleInfo*>() << lm, lm->entries()[l->_hasScope ?
							l->_scopeMap[index.row()] : index.row()], l->_displayOptions, l->_filterOptions);
					}
                    else if(l->_module && l->_module->type() == CSwordModuleInfo::GenericBook)
                    {
                        CSwordBookModuleInfo *b = reinterpret_cast<CSwordBookModuleInfo*>(l->_module);
                        CSwordTreeKey key(b->tree(), b);
                        key.setIndex(l->_hasScope ? l->_scopeMap[index.row()] : index.row() * 4);

						Rendering::CEntryDisplay ed;
                        r += ed.text(QList<const CSwordModuleInfo*>() << l->_module, key.key(), l->_displayOptions, l->_filterOptions);
                    }

                    if(d->_isSearch && !d->_searchText.isEmpty())
                        r = CSwordModuleSearch::highlightSearchedText(r, d->_searchText);

					if(l->_hasContents)
						r += l->_contents.toString();

					return r;
				}
			}
        }
        
    case BtMini::PlaceRole:
		{
			const BtMiniModuleTextModelPrivate::List *l = d->indexList(index);

			if(l->_module)
			{
                if(l->_module->type() == CSwordModuleInfo::GenericBook)
                {
                    CSwordBookModuleInfo *b = qobject_cast<CSwordBookModuleInfo*>(l->_module);
                    CSwordTreeKey key(b->tree(), b);

                    key.setIndex(l->_hasScope ? l->_scopeMap[index.row()] : index.row() * 4);
                    return key.key();
                }
                else if(l->_module->type() == CSwordModuleInfo::Lexicon)
				{
					CSwordLexiconModuleInfo *lm = qobject_cast<CSwordLexiconModuleInfo*>(l->_module);
                    return lm->entries()[l->_hasScope ? l->_scopeMap[index.row()] : index.row()];
                }
                else if(d->indexDepth(index) == 2)
					return d->indexToVerseKey(index).key();
			}
		}
        break;
        
    case BtMini::ModuleRole:
        return d->indexList(index)->_name;
//        switch(d->indexDepth(index))
//        {
//        case 1:
//            return d->_lists[index.internalId()]._module->name();
//        case 2:
//            if(d->indexList(index)->_module)
//                return d->indexList(index)->_module->name();
//            break;
//        }
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

QModelIndex BtMiniModuleTextModel::keyIndex(int i, QString keyName) const
{
    Q_D(const BtMiniModuleTextModel);

    if(keyName.isEmpty())
        return QModelIndex();

	Q_ASSERT(!d->_lists[i]._hasScope);

	const BtMiniModuleTextModelPrivate::List *l = &d->_lists[i];

	if(l->_module->type() == CSwordModuleInfo::Lexicon)
	{
		CSwordLexiconModuleInfo *li = qobject_cast<CSwordLexiconModuleInfo*>(l->_module);
        int i = li->entries().indexOf(keyName);
		if(i >= 0)
            return createIndex(i, 0, (void*)l);
	}
    else if(l->_module->type() == CSwordModuleInfo::GenericBook)
    {
        CSwordBookModuleInfo *m = qobject_cast<CSwordBookModuleInfo*>(l->_module);
        CSwordTreeKey k(m->tree(), m);
        k.setKey(keyName);

        CSwordTreeKey p(k);
        p.root();

        if(p != k)
            return createIndex(k.getIndex() / 4, 0, (void*)l);
    }
    else
    {
        BtMiniSwordMutex.lock();

        CSwordVerseKey verse(l->_module);
        verse.setKey(keyName);
        const int r = verse.getIndex() - l->_firstEntry;

        BtMiniSwordMutex.unlock();

        if(r >= 0 && r < l->_maxEntries)
            return createIndex(r, 0, (void*)l);
    }

    return QModelIndex();
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

	Q_CHECK_PTR(v);

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

        v->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));
        
        BtMiniModuleTextModel *m = d->fromContentsInfo(contents, &menu);

        if(m)
        {
            connect(view, SIGNAL(longPressed(const QModelIndex&)), m, SLOT(openMenu(const QModelIndex&)));
            connect(view, SIGNAL(shortPressed(const QModelIndex&)), m, SLOT(openContext(const QModelIndex&)));

            view->setModel(m);

            QVBoxLayout *l = new QVBoxLayout;
            l->addWidget(view);
            menu.setLayout(l);

            view->scrollTo(m->index(btConfig().value<int>("mini/openInfoModule", 0), 0));
            
            menu.exec();

            // Sync context modules to config
            QModelIndexList list = view->currentIndexes();
            QStringList modules;

            for(int i = 0; i < list.size(); ++i)
                modules.append(m->d_func()->_lists[i]._name);

            btConfig().setValue<int>("mini/openInfoModule", view->currentLevel());
            btConfig().setValue<QStringList>("mini/openInfoModules", modules);
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
	works->setSleep(true);

    BtMiniMenu menu;

    QFont f(menu.font());
    f.setPixelSize(f.pixelSize() * 0.68);
    menu.setFont(f);

    BtMiniView *view = new BtMiniView(&menu);
    view->setInteractive(true);

    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(view);
    menu.setLayout(l);

    BtBookshelfTreeModel * m = new BtBookshelfTreeModel(BtBookshelfTreeModel::Grouping(true), view);
    m->setSourceModel(CSwordBackend::instance()->model());
    m->setDisplayFormat(QList<QVariant>() << BtBookshelfModel::ModuleNameRole << "<font size='60%' color='#555555'>"
		"<br>&nbsp;</br><word-breaks/>" << BtBookshelfModel::ModuleDescriptionRole << "</font>");

	// if modules more than 4, scrollbar always visible
	if(m->modules().size() > 4)
	{
		BtMiniLevelOption o = view->layoutDelegate()->levelOption();
		o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
		view->layoutDelegate()->setLevelOption(o);
	}

    view->setModel(m);

    QModelIndexList list = m->match(m->index(0, 0), BtBookshelfModel::ModuleNameRole,
	    cm, 1, Qt::MatchExactly | Qt::MatchRecursive);
	    
	if(list.size() > 0 && list[0].isValid())
	    view->scrollTo(list[0]);

    connect(view, SIGNAL(longPressed(const QModelIndex&)), this, SLOT(openModuleMenu(const QModelIndex&)));

	menu.exec();

	works->setSleep(false);

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
		CSwordModuleInfo *mi = CSwordBackend::instance()->findModuleByName(nm);
        
		// Restore module place
		if(mi->type() == CSwordModuleInfo::Bible || mi->type() == CSwordModuleInfo::Commentary)
		{
	        place.setModule(mi);
	
			QModelIndex index = keyIndex(works->currentLevel(), place.key());
			if(index.isValid())
				works->scrollTo(index);
		}
	}
}

void BtMiniModuleTextModel::openPlaceSelection()
{
	Q_D(BtMiniModuleTextModel);

    BtMiniView *works = BtMini::findView(BtMini::worksWidget());

    QString cm = works->currentIndex().data(BtMini::ModuleRole).toString().section(',', 0, 0);
	QString cp = works->currentIndex().data(BtMini::PlaceRole).toString();

	works->setSleep(true);

    BtMiniMenu menu;

    QFont f(menu.font());
    f.setPixelSize(f.pixelSize() * 0.85);
    menu.setFont(f);

    BtMiniView *view = new BtMiniView(&menu);
    view->setInteractive(true);

    BtMiniModuleNavigationModel * m = new BtMiniModuleNavigationModel(cm, view);
    view->setModel(m);

    QVBoxLayout *l = new QVBoxLayout;

    CSwordModuleInfo *mi = CSwordBackend::instance()->findModuleByName(cm);

    if(mi && mi->type() != CSwordModuleInfo::Lexicon)
    {
        view->setTopShadow(true);

        QLabel *c = new QLabel("", view);
        f.setPixelSize(f.pixelSize() * 0.75);
        f.setWeight(QFont::Normal);
        c->setFont(f);
        c->setMargin(f.pixelSize() / 2);
        c->setAlignment(Qt::AlignCenter);

        m->setIndicator(c);
        QObject::connect(view, SIGNAL(currentChanged(QModelIndex)), m, SLOT(updateIndicator(QModelIndex)));

        l->addWidget(c, Qt::AlignCenter);
    }

    l->addWidget(view);
    menu.setLayout(l);


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
            if(d->indexList(index)->_module->type() == CSwordModuleInfo::Bible ||
                d->indexList(index)->_module->type() == CSwordModuleInfo::Commentary)
            {
                CSwordVerseKey key(d->indexToVerseKey(index));
                QMutexLocker locker(&BtMiniSwordMutex);

                place = QString::fromLocal8Bit(key.getShortText());
            }
            else if(d->indexList(index)->_module->type() == CSwordModuleInfo::GenericBook)
                place = place.replace("/", " "). replace("Book ", "").replace("Section ", "");
        }

        d->_placeIndicator->setEnabled(!place.isEmpty());
        d->_placeIndicator->setText(place);
	}

    if(d->_moduleIndicator)
    {
        //Q_ASSERT(index.model() == this);
        //QString module(d->indexList(index)->_name);
        QString module(index.data(BtMini::ModuleRole).toString());
        d->_moduleIndicator->setText(module.isEmpty() ? tr("No Module") : module);
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
            d->setupModule(l, value.toString());
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
		key.setLowerBound(bm->lowerBound());
		key.setUpperBound(bm->upperBound());
		scope = key;
	}

	if(d->_searchText.isEmpty())
		;
    else if(btConfig().value<bool>("mini/searchUsingSword"))
	{
		QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress(tr("Search ...")));
		dialog->show();

		m->module()->createSearchFramework();

        QPair<BtMiniMenu*, CSwordModuleInfo*> p(dialog.data(), m);
		results = m->module()->search(d->_searchText.replace(tr("strong:", "Strongs search keyword"), "strong:").toUtf8(),
            0, 0, &scope, 0, &BtMiniModuleTextModelPrivate::searchProgress, &p);

		if(dialog->wasCanceled() || m->module()->terminateSearch)
			results = sword::ListKey();
	}
	else
	{
        bool ok = true;

		// build index if ascent
		if(!m->hasIndex())
		{
			if(BtMiniMenu::execQuery(tr("Build index for module?"), QStringList() << tr("Yes") << tr("No")) != 0)
				ok = false;
			else
			{
				QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress(tr("Indexing...")));

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
            CSwordModuleSearch searcher;

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

    QModelIndex wi = BtMini::findView(BtMini::worksWidget())->currentIndex();
    QString wm(wi.data(BtMini::ModuleRole).toString());
    CSwordModuleInfo *md = CSwordBackend::instance()->findModuleByName(wm.section(',', 0, 0));

    QStringList actions;
    
    QString category(m->categoryName(m->category()));
    if(category.size() > 11)
        category = category.left(10) + "...";
    
    actions << tr("Set default ") + "\n" + category;

    if(md && md->category() == m->category() == CSwordModuleInfo::Bibles)
        actions << tr("Add Parallel");
    
    const int r = BtMiniMenu::execMenu(actions);

    if(r == 0)
    {
        switch(m->category())
        {
        case CSwordModuleInfo::Bibles:
            btConfig().setDefaultSwordModuleByType("standardBible", m);
            break;
        case CSwordModuleInfo::Commentaries:
            btConfig().setDefaultSwordModuleByType("standardCommentary", m);
            break;
        case CSwordModuleInfo::Lexicons:
            btConfig().setDefaultSwordModuleByType("standardLexicon", m);
            if(m->has(CSwordModuleInfo::HebrewDef))
                btConfig().setDefaultSwordModuleByType("standardHebrewStrongsLexicon", m);
            if(m->has(CSwordModuleInfo::GreekDef))
                btConfig().setDefaultSwordModuleByType("standardGreekStrongsLexicon", m);
            if(m->has(CSwordModuleInfo::HebrewParse))
               btConfig().setDefaultSwordModuleByType("standardHebrewMorphLexicon", m);
            if(m->has(CSwordModuleInfo::GreekParse))
                btConfig().setDefaultSwordModuleByType("standardGreekMorphLexicon", m);
            break;
        case CSwordModuleInfo::DailyDevotional:
            btConfig().setDefaultSwordModuleByType("standardDailyDevotional", m);
            break;
        }
    }
    else if(r == 1)
    {
        setData(wi, wm + ',' + m->name(), BtMini::ModuleRole);
        BtMiniMenu::closeMenus();
    }
}

void BtMiniModuleTextModel::modulesReloaded()
{
	Q_D(BtMiniModuleTextModel);

	for(int i = 0; i < d->_lists.size(); ++i)
		d->_lists[i].setModule(d->_lists[i]._name);
}

QModelIndexList BtMiniModuleTextModel::match(const QModelIndex &start, int role, const QVariant &value, int hits,
                      Qt::MatchFlags flags) const
{
    if(role == BtMini::PlaceRole)
        return QModelIndexList() << keyIndex(d_ptr->indexListId(start), value.toString());

    return QAbstractItemModel::match(start, role, value, hits, flags);
}