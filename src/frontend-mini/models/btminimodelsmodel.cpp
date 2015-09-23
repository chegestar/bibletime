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

#include <QPushButton>
#include <QtDebug>

#include "btminimodelsmodel.h"

class BtMiniModelsModelPrivate
{
public:
	BtMiniModelsModelPrivate() {;}
	~BtMiniModelsModelPrivate() {;}

    QVector<QPair<QAbstractItemModel*, QString> >  _models;
    mutable QVector<QModelIndex>                   _mapping;
    QPushButton                                   *_indicator;

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
}

void BtMiniModelsModel::setIndicator(QWidget *w)
{
    d_ptr->_indicator = qobject_cast<QPushButton*>(w);
    updateIndicators(QModelIndex());
}

void BtMiniModelsModel::updateIndicators(QModelIndex index)
{
    if(d_ptr->_indicator)
    {
        if(!index.isValid() || !index.parent().isValid())
            d_ptr->_indicator->setText("Select repository:");
        else
            d_ptr->_indicator->setText(index.parent().data().toString().remove(QRegExp("<[^>]*>")));
    }
}
