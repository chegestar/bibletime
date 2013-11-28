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

#ifndef BTMINIUI_H
#define BTMINIUI_H

#include <QObject>

class BtMiniView;
class BtMiniUiPrivate;
class QPushButton;

#if !(defined Q_OS_ANDROID || defined Q_OS_IOS || defined Q_OS_LINUX || defined Q_OS_WIN32 || defined Q_OS_BLACKBERRY)
#define BT_MINI_EXIT_BUTTON
#endif

/**
* Manage all user interface elements, and responsible for:
*   creation of all basic windows
*   setting up basic logic for created elements
*   provide interfaces for sofisticated interface building in logic/models objects
*/
class BtMiniUi : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(BtMiniUi)

    BtMiniUi(QObject *parent = 0);

public:
    ~BtMiniUi();

    inline static BtMiniUi* instance()
    {
        static BtMiniUi ui;
        return &ui;
    }

    void show();

    QWidget* mainWidget();
    QWidget* worksWidget();
    BtMiniView* worksView();
    BtMiniView* searchView();

    /** Mark groups of widgets to reset, this will be done when any widget activated. */
    void resetWidgets(bool main, bool works, bool rest);

    /** */
    QWidget* activateNewContextWidget();
    QWidget* currentContextWidget();

    /** Routines.
     *  Make button suitable fot Mini interface. if invertedIcon is supplied detection of
     *  current color scheme is performed.
    */
    QPushButton* makeButton(QString text, QString icon = QString(), QString invertedIcon = QString());
    QPushButton* makeButton(QString text, QIcon icon);

public slots:
    /** Make window wisible to user in foreground, those function would do nothing when multiple
     *  windows displayed. */
    void activateWorks();
    void activateInstaller();
    void activateSearch();
    void activateSettings();

    /** Go back, return false if nothing was done. */
    bool activatePreviousWidget();

    /** */
    void applicationStateChanged();

    /** */
    void modulesReloaded();


private:
    Q_DECLARE_PRIVATE(BtMiniUi)
    BtMiniUiPrivate * const d_ptr;

};

#endif // BTMINIUI_H
