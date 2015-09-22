/*************************************************************************
* view.h - contents widget
*
* author: Konstantin Maslyuk "Kalemas" mailto:kalemas@mail.ru
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation version 2.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
************************************************************************/

#ifndef _SB_VIEW_H_
#define _SB_VIEW_H_

#include <QVariant>
#include <QWidget>

class ViewPrivate;

namespace View
{
    enum Type
    {
        TYPE_NONE = 0,
        TYPE_CURRENT,                 // virtual, to update current list
        PIN_NEXT,                     // pin
        PIN_PREV,                     // pin
        TYPE_BACK,                    // previous view state
        TYPE_MODULE_VIEW,
        TYPE_SELECT_MODULE,
        TYPE_SELECT_BOOK,
        TYPE_HOME,
        TYPE_READINGS,
        TYPE_HISTORY,
        TYPE_MENU_OPTIONS,
        TYPE_MODULE_INSTALLER_DISCLAIMER,
        TYPE_MODULE_INSTALLER,
        TYPE_SEARCH,
        TYPES_COUNT
    };
}

class ContentsWidget : public QWidget
{
    Q_DECLARE_PRIVATE(View)
	Q_OBJECT

public:
    ContentsWidget(QWidget *parent=0);
    ~ContentsWidget();

	QSize getSize() const;

public slots:
	void switchView(QVariant to);
	void performSearch();

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    ViewPrivate * const d_ptr;

};

#endif