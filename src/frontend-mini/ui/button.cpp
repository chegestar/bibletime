/*************************************************************************
 * button.cpp - common button
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

#include "button.h"

Button::Button(QString text, QVariant &data, QWidget *parent) : QPushButton(text, parent)
{
    _data = data;
    connect(this, SIGNAL(released()), this, SLOT(doClicked()));
}

Button::Button( QIcon &icon, QString &text, QVariant &data, QWidget *parent) : QPushButton(icon, text, parent)
{
    _data = data;
    connect(this, SIGNAL(released()), this, SLOT(doClicked()));
}

Button::~Button()
{
	;
}

void Button::doClicked()
{
	emit clicked(_data);
}
