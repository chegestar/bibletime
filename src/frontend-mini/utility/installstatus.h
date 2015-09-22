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
    InstallStatus(){;}
    ~InstallStatus(){;}

    void preStatus(long totalBytes, long completedBytes, const char *message)
    {
        QProgressDialog *dialog = qobject_cast<QProgressDialog*>(_dialog);
        if(dialog)
        {
            QApplication::processEvents();
            if(dialog->wasCanceled())
                Core_->getInstallMgr()->terminate();
            dialog->setLabelText(message);
        }
    }

    void statusUpdate(double dtTotal, double dlNow)
    {
        QProgressDialog *dialog = qobject_cast<QProgressDialog*>(_dialog);
        if(dialog)
        {
            QApplication::processEvents();
            if(dialog->wasCanceled())
                Core_->getInstallMgr()->terminate();
            dialog->setMaximum(dtTotal);
            dialog->setValue(dlNow);
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