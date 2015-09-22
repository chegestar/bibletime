/*************************************************************************
 * core.h - application core
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

#ifndef _SB_CORE_H_
#define _SB_CORE_H_

#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include <installmgr.h>
#include <ftptrans.h>
#include <versekey.h>

#include "utility/personalization.h" // move to cpp

#define BT_VERSION "2.9.0"

class CorePrivate;
class InstallStatus;
class sbList;

class Core : public QApplication
{
	Q_OBJECT

public:	
	struct Input
	{
		Input ()
		{
			memset (this, 0, sizeof(Input));
		}

		bool                     mousePressed;
		QPoint                   start;
        QPoint                   last;
		bool                     leaveZone;
		int                      tapCounter;
		mutable bool             block;
		int                      scrollDataY [2];
		int                      scrollDataTicks [2];
	};

	Core(int argc, char *argv[]);
    ~Core();
	
	sword::SWMgr *               getSword          ();
	sword::InstallMgr *          getInstallMgr     ();
	sword::SWConfig *            getOptions        ();

	QRect                        getRect           () const;

    QWidget *                    getMainWidget     ();
    QWidget *                    getControlsWidget ();
    QWidget *                    getContentsWidget ();
    QWidget *                    getHeaderWidget   ();

	// TODO backgroundProcess(sbList *list = 0)
	void                         threadCreate      (sbList *list);
	void                         threadDestroy     ( );
	
    // TODO remove
	const char *                 holdString        ( const char * string );

    Q_DECLARE_PRIVATE(Core)

public slots:
	void                         saveConfiguration ();
    void                         setModulePlace    (QVariant place);
    void                         switchList        (QVariant to);
	void                         desktopResized    (int);

protected:
	bool                         event             (QEvent *e);
    void                         timerEvent        (QTimerEvent *e);

public:
	InstallStatus               *installStatus;

	sbFeatures                   features;

	unsigned int                 scrollStep;
	unsigned int                 versesMax;
	unsigned int                 versesOptimal;

	// Core shared data
	sword::SWModule             *defaultModule;          // base module for all new module views
	sword::SWModule             *currentModule;          // module of active list, null if module view was not activated

	sword::VerseKey              currentVerse;           // last place

	bool                         goVerse;                // necessity of transition, with currentVerse and currentModule
	int                          goReading;              // need attach reading on activate, default -1
	bool                         changedGlobalOptions;   // fixes : Global Options switch in one module cause switch in another

	//bool                         keypadController;       // whether user control with keypad / touchscreen

	std::vector<const char *>    moduleOptions;          // array of option names

	sword::ListKey               searchRange;

	bool                         waitMode;
	QString                      waitMessage;

	qint64                       systemStarted_;

private:
    CorePrivate          * const d_ptr;
    Q_DISABLE_COPY(Core)

};

extern Core *Core_;

#endif
