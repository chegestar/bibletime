/*************************************************************************
 * main.cpp - main application entry
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
 
#include <QFile>
#include <QTextStream>
#include <QtDebug>

#ifdef Q_OS_WIN32
#include <windows.h>
//#include <CRTDBG.H>
#endif

#include "core.h"


void MessageHandler(QtMsgType type, const char *msg)
{
	static QFile outFile("log.txt");
	static bool r = outFile.open(QIODevice::WriteOnly);

	QString s(QString::fromLocal8Bit(msg) + QLatin1Char('\n'));
	
	QTextStream ts(&outFile);
	ts << s;

#if defined Q_OS_WINCE || !defined Q_OS_WIN
	printf(s.toLocal8Bit());
#elif defined Q_OS_WIN
	OutputDebugString((wchar_t*)s.utf16());
#endif
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN32
    //_CrtSetBreakAlloc(101307);
    //_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

	qInstallMsgHandler(MessageHandler);
    //QApplication::setGraphicsSystem("opengl");
    QApplication::setStyle("mini");

    Core *c = new Core(argc, argv);
    int r = c->exec();
    delete c;
    return r;
}
