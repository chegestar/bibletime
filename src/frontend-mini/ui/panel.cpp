/*************************************************************************
 * panel.cpp - items on screen
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

#include <QPainter>
#include <QStyleOption>

#include "panel.h"

Panel::Panel(QWidget *parent) : QWidget(parent)
{
	setBackgroundRole(QPalette::Button);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    setAutoFillBackground(true);
}

Panel::~Panel()
{
    ;
}

QSize Panel::sizeHint() const
{
    return QWidget::sizeHint().boundedTo(parentWidget()->size());
}

QSize Panel::minimumSizeHint() const
{
    return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
}

void Panel::paintEvent(QPaintEvent *event)
{
    //QStyleOption so;
    //so.initFrom(this);
    //style()->drawPrimitive(geometry().top() < geometry().bottom()/2 ? \
    //    QStyle::PE_PanelStatusBar : QStyle::PE_PanelToolBar, &so, &QPainter(this), this);
}
