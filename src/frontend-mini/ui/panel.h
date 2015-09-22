/*************************************************************************
 * panel.h - items on screen
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

#ifndef _SB_PANEL_H_
#define _SB_PANEL_H_

#include <QWidget>

class Panel : public QWidget
{
    Q_OBJECT

public:
    Panel(QWidget *parent=0);
    ~Panel();

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void paintEvent(QPaintEvent *event);

};

#endif