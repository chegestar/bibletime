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

#include <QtDebug>

#include "btminimodelsmodel.h"

BtMiniModelsModel::BtMiniModelsModel(QObject *parent)
    : QAbstractItemModel(parent)
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
    if(!parent.isValid())
        return _models.size();
    else
    {
        Q_ASSERT(parent.model() == this);
        int i = _models[parent.row()].first->rowCount(QModelIndex()); 
        return i;
    }
}

QModelIndex BtMiniModelsModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!parent.isValid() && row < _models.size() && row >= 0)
        return createIndex(row, column);

    return QModelIndex();
}

QModelIndex BtMiniModelsModel::parent(const QModelIndex &index) const
{
    Q_ASSERT(index.model() == this);

    return QModelIndex();
}

QVariant BtMiniModelsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.model() == this);

    if(role == Qt::DisplayRole && index.model() == this && index.row() < _models.size())
        return _models[index.row()].second;

    return QVariant();
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
    _models.append(QPair<QAbstractItemModel*, QString>(model, name));
}
