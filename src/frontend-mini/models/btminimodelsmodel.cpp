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

#include "btminimodelsmodel.h"

#define INSTALL_MODEL

#ifdef INSTALL_MODEL
#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "util/geticon.h"
#include "btmini.h"

struct ModuleItem
{
	ModuleItem() {;}
	ModuleItem(QModelIndex index, QString repository) : _index(index), _repository(repository) {;}
	ModuleItem(QString text) : _text(text) {;}
	ModuleItem(const ModuleItem &copy) : _text(copy._text), _index(copy._index), _repository(copy._repository) {;}

	QString     _text;
	QString     _repository;
    QModelIndex _index;

    bool operator < (const ModuleItem &s2) const
    {
        return _index.data().toString() < s2._index.data().toString();
    }
};

struct CategoryItem
{
	CategoryItem() {;}
	CategoryItem(QString language, QIcon icon) : _name(language), _icon(icon) {;}
	CategoryItem(const CategoryItem &copy) : _name(copy._name), _children(copy._children), _icon(copy._icon) { ; }

	QString                    _name;
	QIcon					   _icon;
	QVector<ModuleItem>        _children;

	bool operator<(const CategoryItem &s2) const
	{
		return _name < s2._name;
	}
};

struct LanguageItem
{
    LanguageItem() {;}
    LanguageItem(QString language) : _language(language) {;}
    LanguageItem(const LanguageItem &copy) : _language(copy._language), _children(copy._children) { ; }

    QString                _language;
    QVector<CategoryItem>  _children;

    bool operator<(const LanguageItem &s2) const
    {
        return _language < s2._language;
    }
};
#endif


class BtMiniModelsModelPrivate
{
public:
	BtMiniModelsModelPrivate() {;}
	~BtMiniModelsModelPrivate() {;}


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
			level == 2 ? (((id & 0xff000)) - 1 >> (4*3)) : (id & 0xfff) - 1;
	}

    QVector<QPair<QAbstractItemModel*, QString> >  _models;
    mutable QVector<QModelIndex>                   _mapping;
    QLabel                                        *_indicator;

#ifdef INSTALL_MODEL
    // install model
    QVector<LanguageItem>                          _languages;
#endif
};


BtMiniModelsModel::BtMiniModelsModel(QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new BtMiniModelsModelPrivate())
{
    ;
}

BtMiniModelsModel::~BtMiniModelsModel()
{
    ;
}

int BtMiniModelsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int BtMiniModelsModel::rowCount(const QModelIndex &parent) const
{
	Q_D(const BtMiniModelsModel);

#ifdef INSTALL_MODEL
    // install model
    if(!parent.isValid())
        return d->_languages.size();
	else if(d->indexDepth(parent.internalId()) == 1)
	{
		return d->_languages[d->indexLevel(parent.internalId(), 1)]._children.size();
	}
	else if(d->indexDepth(parent.internalId()) == 2)
	{
		return d->_languages[d->indexLevel(parent.internalId(), 1)]._children[
			d->indexLevel(parent.internalId(), 2)]._children.size();
	}
	else
		return 0;
#endif


    // models model
    if(!parent.isValid())
        return d->_models.size();

	if (parent.internalId() == 0)
		return d->_models[parent.row()].first->rowCount(QModelIndex());

	QModelIndex mi = d->_mapping[parent.internalId() - 1];
	return mi.model()->rowCount(mi);
}

QModelIndex BtMiniModelsModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_D(const BtMiniModelsModel);

#ifdef INSTALL_MODEL
	// install model
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
	else
		Q_ASSERT(false);
#endif

	// models model
    if (!parent.isValid())
	{
		//Q_ASSERT(row < d->_models.size() && row >= 0);
        return createIndex(row, column);
	}

	QModelIndex mi;
	
	if (parent.internalId() == 0)
	{
		QAbstractItemModel *m = d->_models[parent.row()].first;
		mi = m->index(row, column, QModelIndex());
	}
	else
	{
		QModelIndex pi = d->_mapping[parent.internalId() - 1];
		mi = pi.child(row, column);
	}

	Q_ASSERT(mi.isValid());

	int i = d->_mapping.indexOf(mi) + 1;

	if (i == 0)
	{
		d->_mapping.append(mi);
		i = d->_mapping.size();
	}

	return createIndex(row, column, i);
}

QModelIndex BtMiniModelsModel::parent(const QModelIndex &index) const
{
	Q_D(const BtMiniModelsModel);

#ifdef INSTALL_MODEL
	// install model
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

#endif

	// models model
	if (index.internalId() > 0)
	{
		QModelIndex mi = d->_mapping[index.internalId() - 1];

		if (mi.parent() == QModelIndex())
		{
			for (int i = 0; i < d->_models.size(); ++i)
				if (mi.model() == d->_models[i].first)
					return createIndex(i, 0);
			Q_ASSERT(false);
		}
		else
		{
			return createIndex(mi.parent().row(), 0, d->_mapping.indexOf(mi.parent()) + 1);
		}
	}

    return QModelIndex();
}

QVariant BtMiniModelsModel::data(const QModelIndex &index, int role) const
{
	Q_D(const BtMiniModelsModel);

    Q_ASSERT(index.model() == this);

#ifdef INSTALL_MODEL
	// install model
	if(!index.isValid())
		return QVariant();
	if(d->indexDepth(index.internalId()) == 1)
	{
		LanguageItem l(d->_languages[d->indexLevel(index.internalId(), 1)]);
		if(role == Qt::DisplayRole)
			return l._language;
		if(role == Qt::DecorationRole)
			return util::getIcon("flag.svg");
		return QVariant();
	}
	if(d->indexDepth(index.internalId()) == 2)
	{
		CategoryItem c(d->_languages[d->indexLevel(index.internalId(), 1)]._children[d->indexLevel(index.internalId(), 2)]);
		if(role == Qt::DisplayRole)
			return c._name;
		if(role == Qt::DecorationRole)
			return c._icon;
		return QVariant();
	}
	if(d->indexDepth(index.internalId()) == 3)
	{
		ModuleItem m(d->_languages[d->indexLevel(index.internalId(), 1)]._children[d->indexLevel(index.internalId(), 2)]._children[d->indexLevel(index.internalId(), 3)]);
		if(m._text.isEmpty())
		{
			if(role == BtMini::RepositoryRole)
				return m._repository;
			return m._index.data(role);
		}
		else if(role == Qt::DisplayRole)
			return m._text;
		return QVariant();
	}
#endif

	// models model
    if(index.internalId() == 0)
	{
		if (role == Qt::DisplayRole)
	        return d->_models[index.row()].second;
		else
			return QVariant();
	}
	else
	{
		QModelIndex mi = d->_mapping[index.internalId() - 1];
		return mi.data(role);
	}
}

bool BtMiniModelsModel::hasChildren(const QModelIndex &parent) const
{
    return (rowCount(parent) > 0);
}

Qt::ItemFlags BtMiniModelsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QVariant BtMiniModelsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
        return "Module";
    return QVariant();
}

bool BtMiniModelsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

void BtMiniModelsModel::addModel(QAbstractItemModel *model, QString name)
{
	Q_D(BtMiniModelsModel);

    d->_models.append(QPair<QAbstractItemModel*, QString>(model, name));

#ifdef INSTALL_MODEL
	for(int i = 0, s = model->rowCount(); i < s; ++i)
	{
		// language 
		QModelIndex li = model->index(i, 0);
		QString language = model->data(li).toString();

		int lid = -1;
		for(int ii = 0; ii < d->_languages.size(); ++ii)
		{
			if(d->_languages[ii]._language == language)
			{
				lid = ii;
				break;
			}
		}

		// add new language
		if(lid == -1)
		{
			d->_languages.append(LanguageItem(language));
			lid = d->_languages.size() - 1;
		}

		// add categories
		for(int ii = 0, s = model->rowCount(li); ii < s; ++ii)
		{
			QModelIndex ci = model->index(ii, 0, li);
			QString category = model->data(ci).toString();


			int cid = -1;
			for(int iii = 0; iii < d->_languages[lid]._children.size(); ++iii)
			{
				if(d->_languages[lid]._children[iii]._name == category)
				{
					cid = iii;
					break;
				}
			}

			// add new category
			if(cid == -1)
			{
				d->_languages[lid]._children.append(CategoryItem(category, ci.data(Qt::DecorationRole).value<QIcon>()));
				cid = d->_languages[lid]._children.size() - 1;
			}

			d->_languages[lid]._children[cid]._children.append(ModuleItem("<font size=\"66%\"><center><b>" + name + "</b></center></font>"));

			// add modules
			for(int iii = 0, s = model->rowCount(ci); iii < s; ++iii)
			{
				QModelIndex mi = model->index(iii, 0, ci);
				d->_languages[lid]._children[cid]._children.append(ModuleItem(mi, name));
			}
		}
	}

    qSort(d->_languages);
#endif
}

void BtMiniModelsModel::setIndicator(QWidget *w)
{
    d_ptr->_indicator = qobject_cast<QLabel*>(w);
}

void BtMiniModelsModel::updateIndicators(QModelIndex index)
{
    if(d_ptr->_indicator)
    {
        if(!index.isValid() || !index.parent().isValid())
            d_ptr->_indicator->setText(tr("Select language:"));
        else
            d_ptr->_indicator->setText(index.parent().data().toString().remove(QRegExp("<[^>]*>")));
    }
}
