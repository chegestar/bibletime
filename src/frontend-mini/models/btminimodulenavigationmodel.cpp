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

#include <QMutex>
#include <QtDebug>

#include "backend/managers/cswordbackend.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"

#include "btmini.h"
#include "btminimodulenavigationmodel.h"
#include "view/btminilayoutdelegate.h"


extern QMutex BtMiniSwordMutex;

class BtMiniModuleNavigationModelPrivate
{
public:
	BtMiniModuleNavigationModelPrivate() : _key(0), _parentKey(0)
    {
        _bm = 0;
		_lm = 0;
    }
    
    ~BtMiniModuleNavigationModelPrivate()
    {
        ;
    }

    void setupVerseKey(const QModelIndex &index) const
    {
		QMutexLocker locker(&BtMiniSwordMutex);

        Q_ASSERT(_bm);

		if(!index.isValid())
		{
			_key.setIndex(0);
			return;
		}

		_key.setIndex(index.internalId());
	}

	void setupVerseKey(const QModelIndex &parent, int child) const
	{
		Q_ASSERT(_bm);

		QMutexLocker locker(&BtMiniSwordMutex);

		if(_parentIndex != parent)
		{
			_parentIndex = parent;
			locker.unlock();
			setupVerseKey(_parentIndex);
			locker.relock();
			_parentKey.copyFrom(_key);
		}

		if(_parentKey.getBook() == 0)
		{
			_key.setAutoNormalize(false);
			_key.setTestament(1);
			_key.book(_bm->books()->at(child));
			_key.setChapter(0);
			_key.setVerse(0);
			_key.Normalize();
		}
		else if(_parentKey.getChapter() == 0)
		{
			_key.copyFrom(_parentKey);
			_key.setAutoNormalize(false);
			_key.setChapter(child + 1);
			_key.setVerse(0);
			_key.Normalize();
		}
		else if(_parentKey.getVerse() == 0)
		{
			_key.copyFrom(_parentKey);
			_key.setVerse(child + 1);
		}
	}

	CSwordBibleModuleInfo   *_bm;
	CSwordLexiconModuleInfo *_lm;
    QString                  _module;

	mutable CSwordVerseKey   _key;
	mutable CSwordVerseKey   _parentKey;
	mutable QModelIndex      _parentIndex;

    BtMiniLayoutDelegate    *_ld;
};

BtMiniModuleNavigationModel::BtMiniModuleNavigationModel(QString &module, QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new BtMiniModuleNavigationModelPrivate)
{
    Q_D(BtMiniModuleNavigationModel);

    setModule(module);

    d->_ld = new BtMiniLayoutDelegate(this);

    BtMiniLayoutOption o = d->_ld->levelOption(0);

    o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
    o.perCycle = 0;
    o.perLine = 1;
    o.preText = "<b><center>";
    o.postText = "</center></b>";

	if(d->_lm && d->_lm->entries().size() > 100)
	{
		o.scrollPerItem = true;
		o.limitItems    = true;
		o.perCycle      = 5;
	}

    d->_ld->setLevelOption(0, o);

    o.perLine = 3;

    d->_ld->setLevelOption(1, o);
}

BtMiniModuleNavigationModel::~BtMiniModuleNavigationModel()
{
    Q_D(BtMiniModuleNavigationModel);
    
    delete d;
}

void BtMiniModuleNavigationModel::setModule(QString &module)
{
    Q_D(BtMiniModuleNavigationModel);

	CSwordModuleInfo *mi = qobject_cast<CSwordModuleInfo*>(CSwordBackend::instance()->findModuleByName(module));
    
    d->_module = module;

	if(mi->type() == CSwordModuleInfo::Bible || mi->type() == CSwordModuleInfo::Commentary)
	{
		d->_bm = qobject_cast<CSwordBibleModuleInfo*>(mi);

		Q_CHECK_PTR(d->_bm);

		d->_key.setModule(d->_bm);
		d->_key.Headings(1);

		d->_parentIndex = QModelIndex();
		d->setupVerseKey(d->_parentIndex);
		d->_parentKey.copyFrom(d->_key);
	}
	else if(mi->type() == CSwordModuleInfo::Lexicon)
	{
		d->_lm = qobject_cast<CSwordLexiconModuleInfo*>(mi);
	}
}

int BtMiniModuleNavigationModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int BtMiniModuleNavigationModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const BtMiniModuleNavigationModel);
    
    if(d->_bm)
    {
        // book count
        if(!parent.isValid())
            return d->_bm->books()->size();
            
        d->setupVerseKey(parent);
        
        // chapter count or verse count
        if(d->_key.getChapter() == 0)
            return d->_key.getChapterMax();
        else if(d->_key.getVerse() == 0)
            return d->_key.getVerseMax();
    }

	if(d->_lm && !parent.isValid())
		return d->_lm->entries().size();

    return 0;
}

QModelIndex BtMiniModuleNavigationModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const BtMiniModuleNavigationModel);

	if(d->_bm)
	{
	    d->setupVerseKey(parent, row);
        return createIndex(row, column, (int)d->_key.getIndex());
	}

	if(d->_lm)
		return createIndex(row, column);

	return QModelIndex();
}

QModelIndex BtMiniModuleNavigationModel::parent(const QModelIndex &index) const
{
    Q_D(const BtMiniModuleNavigationModel);

	if(d->_bm)
	{
		d->setupVerseKey(index);
		int row = 0;

		QMutexLocker locker(&BtMiniSwordMutex);

		Q_ASSERT(d->_key.getBook() != 0);

		if(d->_key.getBook() == 0 || d->_key.getChapter() == 0)
		{
			return QModelIndex();
		}
		else if(d->_key.getVerse() == 0)
		{
			d->_key.setAutoNormalize(false);
			d->_key.setChapter(0);
			d->_key.setVerse(0);
			d->_key.Normalize();

	#ifdef QT_DEBUG
			if(d->_bm->books()->indexOf(d->_key.book()) == -1)
			{
				qDebug() << d->_bm->books() << d->_key.book();
			}
	#endif

			row = d->_bm->books()->indexOf(d->_key.book());
		}
		else
		{
			d->_key.setVerse(0);
			row = d->_key.getChapter() - 1;
		}

		Q_ASSERT(row >= 0);

        return createIndex(row, 0, (int)d->_key.getIndex());
	}

	if(d->_ld)
		return QModelIndex();

	return QModelIndex();
}

QVariant BtMiniModuleNavigationModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BtMiniModuleNavigationModel);

	if(d->_lm)
	{
		QString r;

		if((role == Qt::DisplayRole && index.isValid()) || role == BtMini::PlaceRole)
			r += d->_lm->entries()[index.row()];

		return r;
	}

	if(d->_bm)
	{
		d->setupVerseKey(index);

		QMutexLocker locker(&BtMiniSwordMutex);

		switch(role)
		{
		case Qt::DisplayRole:
			{
				QString r;

				if(d->_key.getChapter() == 0)
					r = d->_key.book();
				else if(d->_key.getVerse() == 0)
					r = QString::number(d->_key.getChapter());
				else
					r = QString::number(d->_key.getVerse());
	            
				return QVariant(r);
			}
		case BtMini::PlaceRole:
			return d->_key.key();
		}
	}

    return QVariant();
}

bool BtMiniModuleNavigationModel::hasChildren(const QModelIndex &parent) const
{
    return (rowCount(parent) > 0);
}

Qt::ItemFlags BtMiniModuleNavigationModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QVariant BtMiniModuleNavigationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
        return "Module";
    return QVariant();
}

QModelIndex BtMiniModuleNavigationModel::keyToIndex(QString key) const
{
    Q_D(const BtMiniModuleNavigationModel);

	if(d->_bm)
	{
		d->setupVerseKey(QModelIndex());

		QMutexLocker locker(&BtMiniSwordMutex);

		d->_key.setKey(key);

		int row = d->_key.getVerse() - 1;

		if(d->_key.getChapter() == 0)
			row = d->_key.getBook() - 1;
		else if(d->_key.getVerse() == 0)
			row = d->_key.getChapter() - 1;

        return createIndex(row, 0, (int)d->_key.getIndex());
	}

	if(d->_lm)
	{
		int i = d->_lm->entries().indexOf(key);
		if(i >= 0)
			return createIndex(i, 0);
	}

	return QModelIndex();
}
