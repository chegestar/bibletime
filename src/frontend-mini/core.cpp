/*************************************************************************
 * core.cpp - application
 *            initial initialization and final deinitialization
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

#include <QDateTime>
#include <QDesktopWidget>
#include <QElapsedTimer>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmapCache>
#include <QPushButton>
#include <QSpacerItem>
#include <QStyle>
#include <QTimer>
#include <QTimerEvent>
#include <QThread>
#include <QBoxLayout>

#include "backend/config/cbtconfig.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/btstringmgr.h"
#include "util/directory.h"

#include <algorithm>
#include <markupfiltmgr.h>
#include <localemgr.h>

#include <SWLog.h>

#include "contents.h"
#include "core.h"
#include "state.h"
#include "ui/button.h"
#include "ui/menu.h"
#include "ui/panel.h"
#include "utility/installstatus.h"

// Globals
Core *Core_; // TODO make static member

class MainWidget;
class ContentsWidget;
class Panel;

class CorePrivate
{
public:
	CorePrivate()
	{
		backgroundThread_  = NULL;
		swordMgr           = NULL;
		installManager     = NULL;

		options = new sword::SWConfig(".\\options.conf");
		Q_ASSERT(options);
		options->Load();
	}

	~CorePrivate()
	{
		for (unsigned int i=0; i < strings.size(); i++)
            free((char*)strings[i]);

		if (swordMgr != NULL)
			delete swordMgr;
		if (installManager != NULL)
			delete installManager;
		if (options != NULL)
			delete options;
	}

public:
	QThread                     *backgroundThread_;
	sword::SWMgr                *swordMgr;
	sword::SWConfig             *options;
	std::vector<const char*>     strings;
	sword::InstallMgr           *installManager;

	MainWidget                  *_mainWidget;
	ContentsWidget              *_contentsWidget;
	Panel                       *_controlsWidget;
	Panel                       *_headerWidget;
	
	QSpacerItem                 *_sipSpacer;

    bool                         _activated;
};


class MainWidget : public QWidget
{
public:
	MainWidget(QWidget *parent=0) : QWidget(parent)
	{
		;
    }

    ~MainWidget()
    {
		;
    }

protected:
    bool event(QEvent *e)
    {
        switch(e->type())
        {
        case QEvent::Timer:
            break;
        }
        return QWidget::event(e);
    }

	void keyPressEvent(QKeyEvent *e)
    {
		if(e->key() == Qt::Key_D)
		{
			Core_->dumpObjectTree();
			Core_->dumpObjectInfo();
			Core_->d_func()->_mainWidget->dumpObjectTree();
			Core_->d_func()->_mainWidget->dumpObjectInfo();
		}
    }

    QSize sizeHint() const
    {
        return QWidget::sizeHint().boundedTo(QApplication::desktop()->size());
    }

    QSize minimumSizeHint() const
    {
        return QWidget::minimumSizeHint().boundedTo(QApplication::desktop()->size());
    }

public:

};

class SwordLogger : public sword::SWLog
{
public:
    SwordLogger(){;}
    ~SwordLogger(){;}

    void logMessage(const char *message, int level) const
    {
        if(level == sword::SWLog::LOG_WARN)
            qWarning() << message;
        if(level == sword::SWLog::LOG_ERROR)
            qCritical() << message;
        else
            qDebug() << message;
    }
};

Core::Core(int argc, char *argv[]) : QApplication(argc, argv), d_ptr(new CorePrivate)
{
	Q_D(Core);

	qDebug("Init Core");

	Q_ASSERT(Core_ == NULL);

	Core_                = this;
	systemStarted_       = QDateTime::currentMSecsSinceEpoch();
	currentModule        = NULL;
	changedGlobalOptions = false;
	waitMode             = false;

    setApplicationName("SlideBible");
    setOrganizationName("Crosswire");
	setApplicationVersion(BT_VERSION);
    setAutoSipEnabled(true);

    if((*d->options)["Ui"].find("pixmapCache") != (*d->options)["Ui"].end())
        QPixmapCache::setCacheLimit(QString((*(*d->options)["Ui"].find("pixmapCache")).second.c_str()).toInt());

	scrollStep    = atoi((*d->options)["Ui"].getWithDefault("scrollStep", "1"));
	versesMax     = atoi((*d->options)["Ui"].getWithDefault("versesMax", "500"));
	versesOptimal = atoi((*d->options)["Ui"].getWithDefault("versesOptimal", "50"));

    sword::SWLog::setSystemLog(new SwordLogger);
    sword::SWLog::getSystemLog()->setLogLevel(QString((*d->options)["Sword"].getWithDefault("DebugLevel", "4")).toInt());

	sword::StringMgr::setSystemStringMgr(new BTStringMgr);

	if(!util::directory::initDirectoryCache())
	{
		qFatal("Init Core: Error initializing directory cache!");
		QApplication::quit();
		return;
	}

	// TODO install translators

	QString errorMessage;
	new CDisplayTemplateMgr(errorMessage);
	if(!errorMessage.isNull())
	{
		qFatal(("Init Core: " + errorMessage).toLatin1());
		QApplication::quit();
		return;
	}

	// a new BibleTime version was installed (maybe a completely new installation)
	if (CBTConfig::get(CBTConfig::bibletimeVersion) != BT_VERSION)
		CBTConfig::set(CBTConfig::bibletimeVersion, BT_VERSION);

	CSwordBackend *backend = CSwordBackend::createInstance();
	
	QString locale = QLocale::system().name();
	locale = (*Core_->getOptions())["Sword"].getWithDefault("Locale", locale.toLatin1().data());
	backend->booknameLanguage(locale);

	CSwordBackend::instance()->initModules(CSwordBackend::OtherChange);
	backend->deleteOrphanedIndices();

    //defaultModule = backend->findModuleByName((*d->options)["Sword"].getWithDefault("DefaultModule", "KJV").c_str())->module();
	//if(!defaultModule)
	//{
	//	foreach(CSwordModuleInfo *m, backend->moduleList())
	//	{
	//		if(m->category() == CSwordModuleInfo::Bibles)
	//		{
	//			defaultModule = m->module();
	//			break;
	//		}
	//	}
	//}
	
    CSwordModuleInfo *defaultInfo = CBTConfig::get(CBTConfig::standardBible);
    
    if(defaultInfo)
        defaultModule = defaultInfo->module();
    else
    {
        qWarning("InitCore: failed to get default module");
    	foreach(CSwordModuleInfo *m, backend->moduleList())
    		if(m->category() == CSwordModuleInfo::Bibles)
    		{
    			defaultModule = m->module();
    			break;
    		}
    }

	features.load();

#ifndef Q_OS_WINCE
    QFile styleSheet(util::directory::getPicsDir().absolutePath()+"/application.css");

    if (!styleSheet.open(QIODevice::ReadOnly))
        qWarning(("Init Core: unable to open "+styleSheet.fileName()).toLatin1());
    else
        setStyleSheet(styleSheet.readAll());
#endif

    // select window and font size, according to screen resolution and physical size
    QSize size(300, 400);

#ifdef Q_WS_WINCE
    size = QApplication::desktop()->size();
#endif

    QFont f = font();
    f.setPixelSize(qMin(size.width(), size.height())/14.0f);
    setFont(f);

    d->_mainWidget = new MainWidget;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    d->_contentsWidget = new ContentsWidget(d->_mainWidget);
    d->_controlsWidget = new Panel(d->_mainWidget);
	d->_headerWidget   = new Panel(d->_mainWidget);

	// controls widget
	{
        QFont f = d->_controlsWidget->font();
        f.setBold(true);
        d->_controlsWidget->setFont(f);

        Menu *exitQuery = Menu::createQuery("Exit?", QStringList() << "Yes" << "No");
		connect(exitQuery->buttonAt(0), SIGNAL(clicked()), d->_mainWidget, SLOT(close()));

		QLineEdit *e1 = new QLineEdit(d->_controlsWidget);

		Button *b1  = new Button("Goto",    QVariant(View::TYPE_SELECT_BOOK), d->_controlsWidget);
		Button *b2  = new Button("Book",    QVariant(View::TYPE_SELECT_BOOK), d->_controlsWidget);
		Button *b3  = new Button("Module",  QVariant(View::TYPE_SELECT_MODULE), d->_controlsWidget);
		Button *b4  = new Button("Search",  QVariant(View::TYPE_SEARCH), d->_controlsWidget);
		Button *b5  = new Button("Options", QVariant(View::TYPE_MENU_OPTIONS), d->_controlsWidget);
		Button *b6  = new Button("History", QVariant(View::TYPE_HISTORY), d->_controlsWidget);
		Button *b7  = new Button("Home",    QVariant(View::TYPE_HOME), d->_controlsWidget);
		Button *b8  = new Button("Back",    QVariant(View::TYPE_BACK), d->_controlsWidget);
		Button *b10 = new Button("Accept",  QVariant(View::TYPE_MODULE_INSTALLER), d->_controlsWidget);
		Button *b11 = new Button("Decline", QVariant(View::TYPE_BACK), d->_controlsWidget);
        QPushButton *b9 = new QPushButton("Exit", d->_controlsWidget);
		
		e1->setObjectName("search: input");
		b1->setObjectName("open navigation");
		b2->setObjectName("open book selection");
		b3->setObjectName("open module selection");
		b4->setObjectName("open search");
		b5->setObjectName("open options");
		b6->setObjectName("open history");
		b7->setObjectName("home");
		b8->setObjectName("back");
		b9->setObjectName("exit");
		b10->setObjectName("moduleinstaller: accept");
		b11->setObjectName("moduleinstaller: decline");

		QObject::connect(e1, SIGNAL(returnPressed()), d->_contentsWidget, SLOT(performSearch()));

		QObject::connect(b1, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b2, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b3, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b4, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b5, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b6, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b7, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b8, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b9, SIGNAL(clicked()), exitQuery, SLOT(exec()));
		QObject::connect(b10, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
		QObject::connect(b11, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));

		QVBoxLayout *verticalLayout = new QVBoxLayout();
		QHBoxLayout *controlsLayout = new QHBoxLayout();

		verticalLayout->addWidget(e1);

		controlsLayout->addWidget(b1);
		controlsLayout->addWidget(b2);
		controlsLayout->addWidget(b3);
		controlsLayout->addWidget(b4);
		controlsLayout->addWidget(b5);
		controlsLayout->addWidget(b6);
		controlsLayout->addWidget(b7);
		controlsLayout->addWidget(b8);
		controlsLayout->addWidget(b9);
		controlsLayout->addWidget(b10);
		controlsLayout->addWidget(b11);
		
		foreach(QObject *o, d->_controlsWidget->children())
		    if(QWidget *w = qobject_cast<QWidget*>(o))
		        w->hide();

		verticalLayout->addLayout(controlsLayout);

		d->_sipSpacer = new QSpacerItem(d->_mainWidget->width(), 0);
		verticalLayout->addItem(d->_sipSpacer);

		d->_controlsWidget->setLayout(verticalLayout);
	}

	// header widget
	{
		QFont f = d->_headerWidget->font();
		f.setPixelSize(f.pixelSize()*0.8);
		d->_headerWidget->setFont(f);
		
		QHBoxLayout *l = new QHBoxLayout;
        
        Button *b1 = new Button(style()->standardIcon(QStyle::SP_ArrowLeft), QString(), QVariant(View::PIN_PREV), d->_headerWidget);
        Button *b2 = new Button(style()->standardIcon(QStyle::SP_ArrowRight), QString(), QVariant(View::PIN_NEXT), d->_headerWidget);

        QObject::connect(b1, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));
        QObject::connect(b2, SIGNAL(clicked(QVariant)), Core_, SLOT(switchList(QVariant)));

        const int s = qMin(size.width(), size.height())/6;
        b1->setMaximumWidth(s);
        b1->setMaximumHeight(s);
        b2->setMaximumWidth(s);
        b2->setMaximumHeight(s);

        b1->setObjectName("arrow left");
        b2->setObjectName("arrow right");

		QLabel *l1 = new QLabel(d->_headerWidget);
		l1->setObjectName("search: label");
		l1->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
		l1->setWordWrap(true);
		l1->setAlignment(Qt::AlignCenter);
		
		QLabel *l2 = new QLabel(d->_headerWidget);
		l2->setObjectName("module: label");
		l2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
		l2->setAlignment(Qt::AlignCenter);

        l->addWidget(b1, Qt::AlignLeft);
		l->addWidget(l1, Qt::AlignCenter);
		l->addWidget(l2, Qt::AlignCenter);
        l->addWidget(b2, Qt::AlignRight);

		l->setSpacing(0);
		l->setContentsMargins(0, 0, 0, 0);

		d->_headerWidget->setLayout(l);
		
		foreach(QObject *o, d->_headerWidget->children())
		    if(QWidget *w = qobject_cast<QWidget*>(o))
		        w->hide();
	}

	layout->addWidget(d->_headerWidget);
    layout->addWidget(d->_contentsWidget);
    layout->addWidget(d->_controlsWidget);
    
    d->_mainWidget->setLayout(layout);
 
	d->_mainWidget->resize(size);

#ifdef Q_OS_WINCE
    d->_mainWidget->show();
    d->_mainWidget->showFullScreen();
#else
    d->_mainWidget->showNormal();
#endif

	QTimer *saveTimer = new QTimer(this);
	QObject::connect(saveTimer, SIGNAL(timeout()), this, SLOT(saveConfiguration()));
	saveTimer->start(5*60*1000);

    // service maintenance
    startTimer(1500);

    d->_activated = true;

	connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), Core_, SLOT(desktopResized(int)));

	qDebug("Init Core: ok.");
}

Core::~Core()
{
	qDebug("Shutdown Core");

	threadDestroy();

	saveConfiguration();

	delete d_ptr;

	qDebug("Shutdown Core: ok.");
}

bool Core::event(QEvent *e)
{
	switch(e->type())
	{
	case QEvent::ApplicationActivate:
		Core_->switchList(View::TYPE_CURRENT);
        d_ptr->_activated = true;
		return true;
	case QEvent::ApplicationDeactivate:
		Core_->switchList(View::TYPE_NONE);
        d_ptr->_activated = false;
		return true;
	}
    return QApplication::event(e);
}

void Core::saveConfiguration()
{
	Q_D(Core);

	features.save();

	if(d->options)
		d->options->Save();
	if(d->installManager)
		d->installManager->saveInstallConf();

    CBTConfig::syncConfig();
}

void Core::switchList(QVariant view)
{
	d_ptr->_contentsWidget->switchView(view);
}

class ListThread : public QThread
{
public:
	ListThread(sbList *list) : QThread(Core_), list_(list), abort_(false)
	{
		start();
		switch(atoi((*Core_->getOptions())["General"].getWithDefault("thread priority", "2")))
		{
			case 0 : setPriority(QThread::IdlePriority); break;
			case 1 : setPriority(QThread::LowestPriority); break;
			case 2 : setPriority(QThread::LowPriority); break;
			case 3 : setPriority(QThread::NormalPriority); break;
			case 4 : setPriority(QThread::HighPriority); break;
			case 5 : setPriority(QThread::HighestPriority); break;
			case 6 : setPriority(QThread::TimeCriticalPriority); break;
			case 7 : setPriority(QThread::InheritPriority); break;
		}
	}

	~ListThread()
	{
		if(!isRunning())
			return;

		int timeout = 2000;
		abort_ = true;

		setPriority(QThread::HighPriority);
		while (isRunning() && timeout > 0)
		{
			usleep(1);
			timeout--;
		}

		if(isRunning())
		{
			qDebug("thread died!");
			terminate();
		}
	}

protected:
	void run()
	{
		while(!abort_)
		{
			list_->process(&abort_);
			usleep(5);
		}
	}

private:
	bool    abort_;
	sbList *list_;
};


void Core::threadCreate (sbList * list)
{
	Q_D(Core);

	if(d->backgroundThread_)
		delete d->backgroundThread_;
	d->backgroundThread_ = new ListThread(list);
}

void Core::threadDestroy ()
{
	Q_D(Core);

	if(d->backgroundThread_)
	{
		delete d->backgroundThread_;
		d->backgroundThread_ = 0;
	}
}

const char * Core::holdString(const char *string)
{
	Q_D(Core);

	if (string == NULL || strlen(string) == 0) return NULL;

	for (unsigned int i=0; i < d->strings.size(); i++)
	{
		if (!strcmp(string, d->strings[i])) return d->strings[i];
	}
	d->strings.push_back(_strdup(string));
	return d->strings.back();
}

sword::InstallMgr * Core::getInstallMgr()
{
	Q_D(Core);

	if (d->installManager == NULL) 
    {
        installStatus = new InstallStatus;
		d->installManager = new sword::InstallMgr ("./install/", installStatus);
    }
	return d->installManager;
}

QRect Core::getRect() const
{
	return QRect(QPoint(), d_ptr->_contentsWidget->getSize());
}

sword::SWMgr * Core::getSword()
{
	Q_D(Core);
	//return d->swordMgr;
    return CSwordBackend::instance();
}

sword::SWConfig * Core::getOptions()
{
	Q_D(Core);
	return d->options;
}

QWidget * Core::getMainWidget()
{
    return d_ptr->_mainWidget;
}

QWidget * Core::getControlsWidget()
{
    return d_ptr->_controlsWidget;
}

QWidget * Core::getContentsWidget()
{
    return d_ptr->_contentsWidget;
}

QWidget * Core::getHeaderWidget()
{
    return d_ptr->_headerWidget;
}

void Core::setModulePlace(QVariant place)
{
    goVerse = true;
    currentVerse.setText(place.toString().toUtf8());
    switchList(View::TYPE_MODULE_VIEW);
}

void Core::desktopResized(int screen)
{
	Q_D(Core);
	QRect availableGeometry = QApplication::desktop()->availableGeometry();
	d->_sipSpacer->changeSize(d->_mainWidget->width(), d->_mainWidget->height()-availableGeometry.bottom()-1);
	d->_controlsWidget->layout()->invalidate();
}

void Core::timerEvent(QTimerEvent *e)
{
    if(d_ptr->_activated && activeWindow() == 0)
    {
        qDebug() << "Core: fix deactivate application";
        d_ptr->_activated = false;
        setActiveWindow(0);
    }
    else if(!d_ptr->_activated && activeWindow() != 0)
    {
        qDebug() << "Core: fix activate application";
        d_ptr->_activated = true;
        setActiveWindow(d_ptr->_mainWidget);
    }
}