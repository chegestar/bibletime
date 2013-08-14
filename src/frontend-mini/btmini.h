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
class BtMiniPrivate;

class BtMini : public QObject
{
	Q_OBJECT
    Q_DISABLE_COPY(BtMini)

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
        PlaceShortRole,
		RepositoryRole,
        /** Preview before thread will calculate item DisplayRole (QString). */
        PreviewRole
    };

    /** */
    enum BtMiniState
    {
        Main      = 0x01,
        Works     = 0x02,
        Search    = 0x04,
        Settings  = 0x08,
        Installer = 0x10,
        All       = 0xff
    };

    /** Get window of specified type. */
    static QWidget * widget(BtMiniState type);
    static BtMiniView * view(BtMiniState type);

    /** Set flag to recreate widgets on demand. */
    static void reset(BtMiniState mask);

    /** Switch interface states. */
    static void activate(BtMiniState type);

    /** Make device vibrate. This should be reimpemented. */
    static void vibrate(int miliseconds);

    /** */
    static void updateRemoteSources(bool download = true);


public slots:
    /** Install module. This should be moved to BtMiniLogic. */
    void installerQuery(const QModelIndex &index);


    // below are old functions intended to be removed
public:
    /** Main widget. */
    static QWidget * mainWidget(bool fontSizeChanged = false, QString newStyle = QString());
    
    /** Module text view. */
    static QWidget * worksWidget(bool showTip = true, bool reset = false);

    /** */
    static QWidget * searchWidget(bool reset = false);

    /** Manage modules, repositories, search indecies, install manager. */
    static QWidget * installerWidget(bool firstTime = false, bool reset = false);
    
    /** */
    static QWidget * settingsWidget(bool reset = false);
    
    /** The only way to switch interface states. Works with functions above. */
    static void setActiveWidget(QWidget *widget);
    
    /** Cast widget into type of BtMiniView or search for child of that type and
        return. This function should throw exception if view not found, no need
        to check pointer. */
    static BtMiniView * findView(QWidget *widget);
};

#endif
