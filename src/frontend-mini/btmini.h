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
#include <QModelIndex>

class BtMiniView;
class QWidget;

class BtMini : public QObject
{
	Q_OBJECT

	BtMini();

public:
	~BtMini();

	static BtMini& instance();

public:
    enum ItemDataRole
    {
        /** Referenced Sword module or key (QString). */
        ModuleRole = Qt::UserRole + 256,
        PlaceRole,
        /** Preview before thread will calculate item DisplayRole (QString). */
        PreviewRole
    };
    
    /** Main widget. */
    static QWidget * mainWidget();
    
    /** Module text view. */
    static QWidget * worksWidget(bool showTip = true);

    /** */
    static QWidget * searchWidget();

    /** Manage modules, repositories, search indecies, install manager. */
    static QWidget * installerWidget(bool firstTime = false);
    
    /** */
    static QWidget * settingsWidget();
    
    /** The only way to switch interface states. Works with functions above. */
    static void setActiveWidget(QWidget *widget);
    
    /** Cast widget into type of BtMiniView or search for child of that type and
        return. This function should throw exception if view not found, no need
        to check pointer. */
    static BtMiniView * findView(QWidget *widget);

    static void vibrate(int miliseconds);

public slots:
	/** */
	void installerQuery(const QModelIndex &index);
    
};

#endif
