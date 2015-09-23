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

#ifndef BTMINI_H
#define BTMINI_H

#include <QtCore/Qt>

class BtMiniView;
class QWidget;

namespace BtMini
{
    enum ItemDataRole
    {
        /** Referenced Sword module or key (QString). */
        ModuleRole = Qt::UserRole + 256,
        PlaceRole,
        /** Preview before thread will calculate item DisplayRole (QString). */
        PreviewRole
    };
    
    /** Main widget. */
    QWidget * mainWidget();
    
    /** Module text view. */
    QWidget * worksWidget();

    /** */
    QWidget * searchWidget();

    /** Manage modules, repositories, search indecies, install manager. */
    QWidget * bookshelfWidget();
    
    /** */
    QWidget * settingsWidget();

    /** */
    QWidget * aboutWidget();

    /** User data: bookmarks, history, favorites. */
    QWidget * bookmarksWidget();

    /** Raised if user tapped on verse. */
    QWidget * contextWidget();
    
    /** The only way to switch interface states. Works with functions above. */
    void setActiveWidget(QWidget *widget);
    
    /** Cast widget into type of BtMiniView or search for child of that type and
        return. This function should throw exception if view not found, no need
        to check pointer. */
    BtMiniView * findView(QWidget *widget);
    
}

#endif
