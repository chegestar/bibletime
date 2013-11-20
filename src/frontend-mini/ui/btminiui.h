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

public slots:
    /** Make window wisible to user in foreground, those function would do nothing when multiple
     *  windows displayed. */
    void activateWorks();
    void activateInstaller();
    void activateSearch();
    void activateSettings();

    /** */
    void applicationStateChanged();

    /** */
    void modulesReloaded();


private:
    Q_DECLARE_PRIVATE(BtMiniUi)
    BtMiniUiPrivate * const d_ptr;

};

#endif // BTMINIUI_H
