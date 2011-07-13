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

#ifndef BTMINIVIEW_H
#define BTMINIVIEW_H

#include <QAbstractItemView>

#include "btmini.h"

class BtMiniLayoutDelegate;
class BtMiniViewPrivate;

/** View for small screens with sliding views. Features: rendering html data,
    threaded data acquisition, kinetic scrolling.
*/
class BtMiniView : public QAbstractItemView
{
    Q_OBJECT

public:
    BtMiniView(QWidget *parent = 0);
    virtual ~BtMiniView();

    /** Reimplemented from QAbstractItemView. */
    const QModelIndex currentIndex() const;
    QModelIndex       indexAt(const QPoint &point) const;
    QRect             visualRect(const QModelIndex &index) const;

    /** Reimplemented from QAbstractItemView. This function will activate target subview
        fill it with items and then center view on \param index. Specified index will be 
        also current. */
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    
    /** Return content under pointer, used for word/strong lookup. */
    QString currentContents() const;
    
    /** */
    const QModelIndexList currentIndexes() const;

    /** */
    int currentLevel() const;
    
    /** Set batch mode for layout items. \param useLimits Respect items amount and
        list height to limit items count. Otherwise layout whole list. This function
        change level options. */
    void setBatchedLayout(int itemsPerCycle = 1, bool useLimits = true);

    /** Set mode of view to contain action items. Activating any item in view will 
        cause view to close and currentIndex() will return pressed model index. */
    void setInteractive(bool mode = true);

    /** This function does not take ownership of delegate. Also layout delegate can
        be attached automatic if model has children of BtMiniLayoutDelegate type. */
    void setLayoutDelegate(BtMiniLayoutDelegate *layoutDelagate);

    /** Set specified options for given level. */
    void setLevelOptions(const int level, int itemsOnLine, QString preText, QString postText);
    
    /** Using thread will work for active view only. \param previewRole Make preview
        before thread calculates contents. This function change level options. */
    void setThreadedLayout(bool useThread = true, const int previewRole = BtMini::PreviewRole);
    
    /** */
    void setRenderCaching(bool mode);

    /** */
    void setTopShadowEnabled(bool mode);

public slots:
    /** Reimplemented from QAbstractItemView. */
    void reset();
    void setRootIndex(const QModelIndex &index);

    /** Reimplemented from QAbstractItemView. This function only set current index for
        subview and create \param index item only, clear if index is not exists. */
    void setCurrentIndex(const QModelIndex &index);

    /** Activate and switch next/prev subview with animation. */
    void slideLeft();
    void slideRight();

	/** Very strange function. */
	void doItemsLayout();

//protected slots:
//    /** Reimplemented from QAbstractItemView. */
//    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

signals:    
    /** Emitted when user have long pressed item.
        In Bible Place selection or Search Results long press should follow place.
        In Text reader should popup Text Context (commentary/lexicon/dictionary), then
            long press should popup module selection dialog.
        In Module selection - popup menu with Remove, Set Default, About.*/
    void longPressed(const QModelIndex &index);
    
    /** Very long press. */
    void servicePressed(const QModelIndex &index);

    /** */
    void currentChanged(const QModelIndex &index);
    

protected:
    /** Reimplemented from QWidget. */
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void timerEvent(QTimerEvent *e);
    void showEvent(QShowEvent *e);

    /** Reimplemented from QAbstractItemView. */
    void        dataChanged(const QModelIndex &, const QModelIndex &);
    int         horizontalOffset() const;
    bool        isIndexHidden(const QModelIndex &index) const;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    void        setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    int         verticalOffset() const;
    bool        viewportEvent(QEvent *e);
    QRegion     visualRegionForSelection(const QItemSelection &selection) const;
    void        rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void        rowsInserted(const QModelIndex &parent, int start, int end);
    
    /** Make or update view. */
    void makeSubView(int id, const QModelIndex &parent);

    /** Activate or create new view if there is no view. */
    void activateSubView(int id);

    /** Scroll contents. Vertical scrolling will slide active subview up/down, 
        horizontal will slide subviews left/right. */
    void scroll(float horizontal, float vertical);
    

private:
    Q_DECLARE_PRIVATE(BtMiniView)
    BtMiniViewPrivate * const d_ptr;

};

#endif
