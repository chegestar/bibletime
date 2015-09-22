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

#ifndef BTMINIMODULETEXTMODEL_H
#define BTMINIMODULETEXTMODEL_H

#include <QAbstractItemModel>

class BtMiniModuleTextModelPrivate;

/** Standard model for module contents display.
    This model have own layout delegate.
*/
class BtMiniModuleTextModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    BtMiniModuleTextModel(QStringList &modules, QObject *parent = 0);
    ~BtMiniModuleTextModel();
    
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
    QString indexModule(const QModelIndex &index) const;
    
    /** */
    QString indexKey(const QModelIndex &index) const;

    /** */
    QModelIndex keyIndex(int i, QString key) const;

    /** */
    void setIndicators(QWidget *left, QWidget *module, QWidget *place, QWidget *right);

public slots:
    /** */
    void openContext(const QModelIndex &index);

    /** */
    void openMenu(const QModelIndex &index);

    /** For pressed module in BtBookshelfModel. */
    void openModuleMenu(const QModelIndex &index);

    /** This function works with worksView only. */
    void openModuleSelection();

    /**  This function works with worksView only. */
    void openPlaceSelection();

    /** */
    void updateIndicators(const QModelIndex &index);

	/** Set text to search. */
	void setSearchText(const QString &text);

	/** Perform search and fill model with results. Model should be initialized
		with only one "[Search]" item. Search module will be taken from current
		workWidget module. */
	void startSearch();

protected:

private:
    Q_DECLARE_PRIVATE(BtMiniModuleTextModel)
    BtMiniModuleTextModelPrivate * const d_ptr;

};

#endif
