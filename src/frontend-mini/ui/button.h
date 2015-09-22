/*************************************************************************
 * button.h - common button
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

#ifndef _SB_BUTTON_H_
#define _SB_BUTTON_H_

#include <QPushButton>
#include <QVariant>

class Button : public QPushButton
{
	Q_OBJECT
public:
	Button(QString text, QVariant &data, QWidget *parent=0);
    Button(QIcon &icon, QString &text, QVariant &data, QWidget *parent=0);
	~Button();

private slots:
	void doClicked();

signals:
	void clicked (QVariant);

private:
	QVariant _data;

};

#endif