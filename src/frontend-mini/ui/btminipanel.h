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

#ifndef BTMINIPANEL_H
#define BTMINIPANEL_H

#include <QWidget>

class BtMiniPanelPrivate;

class BtMiniPanel : public QWidget
{
    Q_OBJECT

public:
    enum Activity
    {
        None = 0,
        Search,
        Exit,
		Close
    };

    typedef QVector<Activity> Activities;

    BtMiniPanel(Activities activities, QWidget *parent=0);
    ~BtMiniPanel();

protected slots:
    /** Handle interaction with activities's controls. */
    void controlActivated();

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void paintEvent(QPaintEvent *event);

private:
    Q_DECLARE_PRIVATE(BtMiniPanel)
    BtMiniPanelPrivate * const d_ptr;

};

#endif
