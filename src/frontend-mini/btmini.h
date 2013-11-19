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
        PreviewRole,
        /** Like preview but then chage data via signal. */
        PreviewUpdateRole
    };

    /** */
    enum BtMiniState
    {
        Main       = 1 << 1,
        Works      = 1 << 2,
        Search     = 1 << 3,
        Settings   = 1 << 4,
        Installer  = 1 << 5,
        Context    = 1 << 6,
        NewContext = 1 << 7,
        Previous   = 1 << 8,
        All        = 0xffff
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

    /** */
    void applicationStateChanged();

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
