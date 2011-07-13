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

#ifndef BTMINILAYOUTDELEGATE_H
#define BTMINILAYOUTDELEGATE_H

#include <QAbstractItemModel>
#include <QVector>

#include "btmini.h"

/** Options for subviews. There are always options for at least
    zero (default) level.
*/
struct BtMiniLayoutOption
{
    BtMiniLayoutOption();
    
    QString preText;
    QString postText;

    int     perLine;
    int     maxItems;
    int     perCycle;
    bool    limitItems;
    
    bool    allowScrollBar;
    bool    allowInputBox;

    bool    useThread;
    int     previewRole;

    QString backgroundText;

	bool    contentsNumbers;
};

/** Class that controls layout of subviews and items in view.
    This class doesn't know anything of view or model, isn't it good?
*/
class BtMiniLayoutDelegate : public QObject
{
    Q_OBJECT

public:
    BtMiniLayoutDelegate(QObject *parent = 0);
    virtual ~BtMiniLayoutDelegate();
    
    /** Get/set options for given layout level. Levels are usually represents
        subviews. */
    const BtMiniLayoutOption & levelOption(const int level = 0) const;
    int levelOptionsCount() const;
    void setLevelOption(const int level, BtMiniLayoutOption &option);
    void setLevelOption(BtMiniLayoutOption &option);
    void eraseLevelOption(const int level);
    
    /** Plain or Tree layout mode.
        Layout all subviews at time using first child of every item in first row.
        Otherwise use tree mode, layout first row, on item activation layout
        subview of activated item children, allow any folding depth. */
    bool plainMode() const;
    void setPlainMode(bool mode);
    

protected:
    QVector<BtMiniLayoutOption> _options;
    bool                        _plain;

};

#endif
