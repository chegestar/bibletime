/*************************************************************************
 * dialog.h - common application dialog
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

#ifndef BT_MINI_DIALOG_H
#define BT_MINI_DIALOG_H

#include <QMouseEvent>
#include <QWidget>

class MenuPrivate;

class Menu : public QWidget
{
	Q_OBJECT
	
public:
	Menu();
	~Menu();

    static Menu * createQuery(QString text, QStringList actions = QStringList());
    static Menu * createProgress(QString text);
    
	static int execQuery(QString text, QStringList actions = QStringList());
	static int execMenu(QStringList actions);
	
	QWidget * buttonAt(int id) const;

    bool wasCanceled();

	bool eventFilter(QObject *watched, QEvent *e);

signals:
    void canceled();

public slots:
	void exec();
    void show();
    void hide();

    void finish();

    void setPercent(int percent);
    void setText(QString text);
	
private slots:
    void buttonTrigger();

protected:
	bool event(QEvent *e);

	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);

    void paintEvent(QPaintEvent *e);

	void hideEvent(QHideEvent *e);

	QSize minimumSizeHint () const;
	QSize sizeHint () const;

private:
	MenuPrivate * const d_ptr;

};

#endif