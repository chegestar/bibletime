/*************************************************************************
* installstatus.cpp - install status reporter
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

#ifndef BT_MINI_INSTALLSTATUS_H
#define BT_MINI_INSTALLSTATUS_H

#include <QApplication>
#include <QProgressDialog>

#include <ftptrans.h>

class InstallStatus : public sword::StatusReporter
{
public:
	InstallStatus() : _dialog(0)
	{
		;
	}

    ~InstallStatus()
	{
		;
	}

    void preStatus(long totalBytes, long completedBytes, const char *message)
    {
        Menu *dialog = qobject_cast<Menu*>(_dialog);
        if(dialog)
		{
			dialog->setText(message);
            if(dialog->wasCanceled())
                Core_->getInstallMgr()->terminate();
        }
    }

    void statusUpdate(double dtTotal, double dlNow)
    {
        Menu *dialog = qobject_cast<Menu*>(_dialog);
        if(dialog)
		{
			dialog->setValue(dlNow*100/dtTotal);
            if(dialog->wasCanceled())
                Core_->getInstallMgr()->terminate();
        }
    }

    void setDialog(QWidget *progressDialog)
    {
        _dialog = progressDialog;
    }

private:
    QWidget *_dialog;
};

#endif