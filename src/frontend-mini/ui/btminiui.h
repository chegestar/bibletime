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

class BtMiniUiPrivate;

/** Class to manage all user interface elements.
 *  No basic logic should be in this object, except interface logic.
 *  All switching slots, signals, methods should go here.
 *  Models would have own logic for ui e.g. fill context popup with custom data.
 */
class BtMiniUi : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(BtMiniUi)

    BtMiniUi(QObject *parent);

public:
    ~BtMiniUi();

    static BtMiniUi* instance();

    void openNewContext();
    void setModelForCurrentView();

public slots:
    void switchToWorks();
    void switchToSearch();
    void switchToInstaller();
    void switchToSettings();

    void saveSession();


private:
    Q_DECLARE_PRIVATE(BtMiniUi)
    BtMiniUiPrivate * const d_ptr;

};

#endif // BTMINIUI_H
