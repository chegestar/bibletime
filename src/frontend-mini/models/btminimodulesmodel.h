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

#ifndef BTMINIMODELSMODEL_H
#define BTMINIMODELSMODEL_H

#include <QAbstractItemModel>
#include <QVector>

class BtMiniModulesModelPrivate;

/** 
*/
class BtMiniModulesModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    BtMiniModulesModel(QObject *parent = 0);
    ~BtMiniModulesModel();
    
    /** Reimplemented from QAbstractItemModel. */
    int           columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex   index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex   parent(const QModelIndex &index) const;
    int           rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool          hasChildren(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool          setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /** */
    void          addModel(QAbstractItemModel *model, QString name);

    /** */
    void          setIndicator(QWidget *w);

public slots:
    /** */
    void          updateIndicators(QModelIndex index);

protected:

private:
	Q_DECLARE_PRIVATE(BtMiniModulesModel)
	BtMiniModulesModelPrivate * const d_ptr;

};

#endif
