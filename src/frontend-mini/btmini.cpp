/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime Mini project. Visit 
* http://sourceforge.net/projects/bibletimemini for more information.
*
* This code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QApplication>
#include <QBitmap>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStackedWidget>
#include <QStyle>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>
#include <QtDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef Q_OS_WIN32
//#include <crtdbg.h>
#endif
#endif

#include <SWLog.h>

#include "backend/config/cbtconfig.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/btinstallbackend.h"
#include "frontend/bookshelfmanager/btinstallmgr.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/btstringmgr.h"
#include "util/directory.h"
#include "util/cresmgr.h"

#include "btmini.h"
#include "models/btminimoduletextmodel.h"
#include "models/btminimodelsmodel.h"
#include "ui/btminimenu.h"
#include "ui/btminipanel.h"
#include "view/btminiview.h"
#include "view/btminilayoutdelegate.h"


// Main stacked widget
class BtMiniMainWidget : public QStackedWidget
{
public:
	BtMiniMainWidget(QWidget *parent=0) : QStackedWidget(parent)
	{
	    setAttribute(Qt::WA_DeleteOnClose);
	    setFrameStyle(QFrame::NoFrame);
        _saveTimer = startTimer(5*60*1000);
        
        setWindowTitle("BibleTime Mini");
        setWindowIcon(util::directory::getIcon(CResMgr::mainWindow::icon));
	}
	
    ~BtMiniMainWidget()
    {
        saveConfig();
    }
    
    /** Save opened session. */
    void saveConfig()
    {
        BtMiniView *v = BtMini::findView(BtMini::worksWidget());

        QModelIndexList list = v->currentIndexes();
        QStringList modules, places;
        
        for(int i = 0; i < list.size(); ++i)
        {
            if(list[i] == v->currentIndex())
                CBTConfig::set(CBTConfig::openModule, i);
            
            modules.append(list[i].data(BtMini::ModuleRole).toString());
            places.append(list[i].data(BtMini::PlaceRole).toString());
        }
        
        CBTConfig::set(CBTConfig::openModules, modules);
        CBTConfig::set(CBTConfig::openPlaces, places);
        
        CBTConfig::syncConfig();
    }

protected:
    QSize sizeHint() const
    {
        return QWidget::sizeHint().boundedTo(QApplication::desktop()->size());
    }

    QSize minimumSizeHint() const
    {
        return QWidget::minimumSizeHint().boundedTo(QApplication::desktop()->size());
    }

    void timerEvent(QTimerEvent *e)
    {
        if(e->timerId() == _saveTimer)
            saveConfig();
        else
            QStackedWidget::timerEvent(e);
    }
    
private:
    int _saveTimer;

};

class BtMiniWidget : public QWidget
{
public:
    BtMiniWidget(QWidget *parent=0) : QWidget(parent)
    {
        qobject_cast<QStackedWidget*>(BtMini::mainWidget())->addWidget(this);
    }

    ~BtMiniWidget()
    {
        ;
    }

protected:
    QSize sizeHint() const
    {
        return QWidget::sizeHint().boundedTo(BtMini::mainWidget()->size());
    }

    QSize minimumSizeHint() const
    {
        return QWidget::minimumSizeHint().boundedTo(BtMini::mainWidget()->size());
    }

};

BtMini::BtMini()
{
	;
}

BtMini::~BtMini()
{
	;
}

BtMini & BtMini::instance()
{
	static BtMini bm;
	return bm;
}

QWidget * BtMini::mainWidget()
{
    static BtMiniMainWidget *w = 0;

    if(!w)
    {
        //QSize size(240, 320);
		QSize size(480, 640);
    
#ifdef Q_WS_WINCE
        size = QApplication::desktop()->size();
#endif
    
        w = new BtMiniMainWidget;
        
        QFont f = w->font();
        f.setPixelSize(qMin(size.width(), size.height())/14.0f);
        w->setFont(f);
        
     	w->resize(size);

        //w->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    
#ifdef Q_OS_WINCE
        w->show();
        w->showFullScreen();
#else
		w->show();
        w->raise();
#endif
    }
    
    return w;
}

QWidget * BtMini::worksWidget()
{
    static BtMiniWidget *w = 0;
    
    if(!w)
    {
        w = new BtMiniWidget(mainWidget());

        BtMiniView *v = new BtMiniView(w);

        v->setTopShadowEnabled(true);

		if(CBTConfig::get(CBTConfig::useRenderCaching))
			v->setRenderCaching(true);

        // Setup controls
        QPushButton *b1 = new QPushButton(v->style()->standardIcon(QStyle::SP_ArrowLeft), QString(), w);
        QPushButton *b2 = new QPushButton(v->style()->standardIcon(QStyle::SP_ArrowRight), QString(), w);
        QPushButton *b3 = new QPushButton(QString("Work"), w);
        QPushButton *b4 = new QPushButton(QString("Place"), w);

        connect(b1, SIGNAL(clicked()), v, SLOT(slideLeft()));
        connect(b2, SIGNAL(clicked()), v, SLOT(slideRight()));

        const int maxSize = w->font().pixelSize()*2.0;
        const QSize iconSize(w->font().pixelSize()*1.3, w->font().pixelSize()*1.3);
        b1->setMaximumSize(maxSize, maxSize);
        b1->setIconSize(iconSize);
        b2->setMaximumSize(maxSize, maxSize);
        b2->setIconSize(iconSize);

        QFont f = w->font();
        f.setPixelSize(f.pixelSize() * 0.75);
        f.setWeight(QFont::DemiBold);
        b3->setFont(f);
        b4->setFont(f);

        b3->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        b4->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << 
            BtMiniPanel::Search << BtMiniPanel::Installer << BtMiniPanel::Exit, w);

        // Put into layout
        QHBoxLayout *hl = new QHBoxLayout;
        QVBoxLayout *vl = new QVBoxLayout;

        hl->addWidget(b1, Qt::AlignLeft);
        hl->addWidget(b3, Qt::AlignCenter);
        hl->addWidget(b4, Qt::AlignCenter);
        hl->addWidget(b2, Qt::AlignRight);
        
        vl->addLayout(hl);
        vl->addWidget(v);
        vl->addWidget(p);

        w->setLayout(vl);

		// Retrieve last session
		QStringList modules = CBTConfig::get(CBTConfig::openModules);
		QStringList places = CBTConfig::get(CBTConfig::openPlaces);

		if(modules.size() == 0)
		{
			modules = CBTConfig::getDefault(CBTConfig::openModules);
			places = CBTConfig::getDefault(CBTConfig::openPlaces);
		}

		QStringList moduleNames;

		foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
			moduleNames << m->name();

		for(int i = 0; i < modules.size(); ++i)
		{
			if(!moduleNames.contains(modules[i]))
			{
				qDebug() << "Remove reference to inexistent module" << modules[i];
				modules.erase(modules.begin() + i);
				places.erase(places.begin() + i);
				--i;
			}
		}

		if(modules.size() == 0)
		{
			foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
				if(m->type() == CSwordModuleInfo::Bible)
				{
					modules = QStringList() << m->name();
					places = QStringList() << CBTConfig::getDefault(CBTConfig::openPlaces)[0];
					break;
				}
		}	

		Q_ASSERT(modules.size() > 0);

		// Setup model
		BtMiniModuleTextModel *m = new BtMiniModuleTextModel(modules, v);

		v->setModel(m);

		connect(v, SIGNAL(currentChanged(const QModelIndex &)), m, SLOT(updateIndicators(const QModelIndex &)));
		connect(v, SIGNAL(shortPressed(const QModelIndex &)), m, SLOT(openContext(const QModelIndex &)));
		connect(v, SIGNAL(longPressed(const QModelIndex &)), m, SLOT(openMenu(const QModelIndex &)));

		m->setIndicators(b1, b3, b4, b2);

		// Restore last session
		for(int i = 0, c = CBTConfig::get(CBTConfig::openModule); i < modules.size(); ++i)
        {
            QModelIndex index(m->keyIndex(i, places[i]));
    		if(i == c)
            {
				v->scrollTo(m->keyIndex(i, places[i]));
            }
		    else if(index.isValid())
            {
		        v->setCurrentIndex(index);
            }
		}

		connect(CSwordBackend::instance(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)), 
			m, SLOT(modulesReloaded()));
    }
    
    return w;
}

QWidget * BtMini::searchWidget()
{
	static BtMiniWidget *w = 0;

	if(!w)
	{
		w = new BtMiniWidget(mainWidget());

		BtMiniView *v = new BtMiniView(w);
		v->setTopShadowEnabled(true);

		// Setup controls
		QLineEdit *le = new QLineEdit(w);

        le->setAlignment(Qt::AlignCenter);

		QFont f(le->font());
		f.setPixelSize(f.pixelSize() * 1.3);
		le->setFont(f);

		BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << BtMiniPanel::Close, w);

		// Put into layout
		QVBoxLayout *vl = new QVBoxLayout;

		vl->addWidget(le);
		vl->addWidget(v);
		vl->addWidget(p);

		w->setLayout(vl);

		// Setup model
		BtMiniModuleTextModel *m = new BtMiniModuleTextModel(QStringList() << "[Search]", v);

		v->setModel(m);

		connect(le, SIGNAL(textChanged(const QString &)), m, SLOT(setSearchText(const QString &)));
		connect(le, SIGNAL(returnPressed()), m, SLOT(startSearch()));
        connect(v, SIGNAL(shortPressed(const QModelIndex &)), m, SLOT(openContext(const QModelIndex &)));
	}

	return w;
}

/** */
QWidget * BtMini::installerWidget()
{
    static BtMiniWidget *w = 0;

    if(!w)
    {
		bool refresh = true;

		switch(BtMiniMenu::execQuery(tr("Update all sources?"), 
			QStringList() << tr("Yes") << tr("No") << tr("Cancel")))
		{
		case 2:
		case -1:
			return 0;
		case 1:
			refresh = false;
		}

        w = new BtMiniWidget(mainWidget());

        BtMiniView *v = new BtMiniView(w);
        v->setTopShadowEnabled(true);

        BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << BtMiniPanel::Close, w);

        // Put into layout
        QVBoxLayout *vl = new QVBoxLayout;

        vl->addWidget(v);
        vl->addWidget(p);

		w->setLayout(vl);

        // Setup model
        BtMiniModelsModel *m = new BtMiniModelsModel(v);
        QStringList        ss(BtInstallBackend::sourceNameList(refresh));
		BtInstallMgr      *im = new BtInstallMgr(m);

		BtMiniLayoutOption o(v->layoutDelegate()->levelOption());
		o.limitItems = true;
		o.perCycle = 5;
		o.scrollPerItem = true;
		v->layoutDelegate()->setLevelOption(1, o);

        foreach(QString s, ss)
        {
            sword::InstallSource is = BtInstallBackend::source(s);
			if(refresh)
				im->refreshRemoteSource(&is);
	
			CSwordBackend *be = BtInstallBackend::backend(is);

			BtBookshelfTreeModel *mm = new BtBookshelfTreeModel(BtBookshelfTreeModel::Grouping(true), m);
			mm->setDisplayFormat(QList<QVariant>() << BtBookshelfModel::ModuleNameRole << "<br/>"
				"<word-breaks/><font size=\"60%\" color=\"#555555\">" << BtBookshelfModel::ModuleDescriptionRole << "</font>");
            mm->setSourceModel(be->model());

            m->addModel(mm, "<center><b>" + s + "</b></center>");
        }

        v->setModel(m);

		connect(v, SIGNAL(clicked(const QModelIndex &)), &BtMini::instance(), SLOT(installerQuery(const QModelIndex &)));
    }

    return w;
}

void BtMini::setActiveWidget(QWidget *widget)
{
	if(widget)
	{
	    qobject_cast<QStackedWidget*>(mainWidget())->setCurrentWidget(widget);

		bool b = widget != worksWidget();
		findView(worksWidget())->setSleep(b);
	}
}

BtMiniView * BtMini::findView(QWidget *widget)
{
    if(BtMiniView *v = qobject_cast<BtMiniView*>(widget))
        return v;

    QList<BtMiniView*> l = widget->findChildren<BtMiniView*>();

    if(l.size() == 1)
        return l[0];

    Q_ASSERT(false);
    return 0;
}

void BtMini::installerQuery(const QModelIndex &index)
{
	CSwordModuleInfo *m = reinterpret_cast<CSwordModuleInfo*>(
		index.data(BtBookshelfModel::ModulePointerRole).value<void *>());

	if(m)
	{
		// TODO detect module status: installed, obsolete, not installed

		if (BtMiniMenu::execQuery(QString(tr("Do you want to install %1 ?")).arg(m->name()), 
			QStringList() << tr("Install") << tr("Cancel")) == 0)
		{
			BtInstallMgr *im = index.model()->findChild<BtInstallMgr*>();
			
			QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress(tr("Installing ...")));
			dialog->show();

			connect(im, SIGNAL(percentCompleted(int, int)), dialog.data(), SLOT(setValue(int)));

            sword::InstallSource is = BtInstallBackend::source(BtInstallBackend::sourceNameList(true)[index.parent().row()]);
            int status = im->installModule(CSwordBackend::instance(), 0, m->name().toLatin1(), &is);

            if (status != 0 || dialog->wasCanceled())
                BtMiniMenu::execQuery(QString(tr("Module was not installed")));
            else
                CSwordBackend::instance()->reloadModules(CSwordBackend::AddedModules);
                //BtMiniMenu::execQuery(QString(tr("Completed. Restart application to see module")));
		}
	}
}

/** Sword debug messages. */
class BtMiniSwordLog : public sword::SWLog
{
public:
    BtMiniSwordLog(){;}
    ~BtMiniSwordLog(){;}

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

/** Debug messages. */
void BtMiniMessageHandler(QtMsgType type, const char *msg)
{
	static QFile f("log.txt");
	static bool r = f.open(QIODevice::WriteOnly);

	QString s(QString::fromLocal8Bit(msg) + QLatin1Char('\n'));
	
	QTextStream t(&f);
	t << s;

#if defined Q_OS_WINCE || !defined Q_OS_WIN
	printf(s.toLocal8Bit());
#elif defined Q_OS_WIN
	OutputDebugString((wchar_t*)s.utf16());
#endif
}

// presentation mode
#ifdef Q_OS_WIN32
//#define PRESENTATION
#endif

class BtMiniApplication : public QApplication
{
public:
	BtMiniApplication(int argc, char **argv) : QApplication(argc, argv)
	{
		mainLoop = false;

#ifdef PRESENTATION
		QPixmap bmu("hand01.png");
		QPixmap bmd("hand02.png");

		cu = QCursor(bmu);
		cd = QCursor(bmd);

		setOverrideCursor(cu);
#endif
	}

	~BtMiniApplication() {;}

#ifdef Q_OS_WINCE
	bool winEventFilter (MSG *msg, long *result)
	{
		switch(msg->message)
		{
		case WM_PAINT:
			mainLoop = true;
			break;
		case WM_ACTIVATE:
			{
				if(mainLoop)
				{
					int active = LOWORD(msg->wParam);
					bool minimized = (BOOL) HIWORD(msg->wParam);

					if(BtMini::mainWidget()->winId() == msg->hwnd)
						BtMini::findView(BtMini::worksWidget())->setSleep(active == WA_INACTIVE);
				}
			}
			break;
		case WM_CLOSE:
			qDebug("Close");
			break;
		case WM_HIBERNATE:
			qDebug("Low memory");
			break;
		//default:
		//	qDebug() << "msg:" << msg->message;
		}
		return false;
	}
#elif defined Q_OS_WIN && defined PRESENTATION
	bool winEventFilter (MSG *msg, long *result)
	{
		switch(msg->message)
		{
		case WM_LBUTTONDOWN:
			setOverrideCursor(cd);
			break;
		case WM_LBUTTONUP:
			setOverrideCursor(cu);
			break;
		}
		return false;
	}
#endif

public:
	bool mainLoop;

#ifdef PRESENTATION
	QCursor cu;
	QCursor cd;
#endif
};

/** Application entry. */
int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN32
    //_CrtSetBreakAlloc(1324);
    //_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

	qInstallMsgHandler(BtMiniMessageHandler);
    //QApplication::setGraphicsSystem("opengl");
    
    // TODO set style for main widget
    QApplication::setStyle("mini");
    
	// Init application
	BtMiniApplication app(argc, argv);
    
	if(!util::directory::initDirectoryCache())
	{
		qFatal("Init Application: Error initializing directory cache!");
		return -1;
	}
	
    app.setApplicationName("BibleTime Mini");
    app.setOrganizationName("Crosswire");
	app.setApplicationVersion("2.9.1");
    app.setAutoSipEnabled(true);

	CBTConfig::set(CBTConfig::bibletimeVersion, app.applicationVersion());

	// install translators
	QTranslator qtTranslator;qtTranslator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);

	QTranslator BibleTimeTranslator;
	BibleTimeTranslator.load( QString("bibletime_ui_").append(QLocale::system().name()), 
		util::directory::getLocaleDir().canonicalPath());
	app.installTranslator(&BibleTimeTranslator);
	
	QTranslator BtMiniTranslator;
	BtMiniTranslator.load( QString("bibletimemini_").append(QLocale::system().name()), 
		util::directory::getLocaleDir().canonicalPath());
	app.installTranslator(&BtMiniTranslator);


	QString errorMessage;
	new CDisplayTemplateMgr(errorMessage);
	if(!errorMessage.isNull())
	{
		qFatal(("Init Application: " + errorMessage).toLatin1());
		return -1;
	}
	
    // Init Sword
    sword::SWLog::setSystemLog(new BtMiniSwordLog);
    sword::SWLog::getSystemLog()->setLogLevel(CBTConfig::get(CBTConfig::swordDebugLevel));

	sword::StringMgr::setSystemStringMgr(new BTStringMgr);
	
	CSwordBackend *backend = CSwordBackend::createInstance();
	
	backend->booknameLanguage(CBTConfig::get(CBTConfig::language));

	CSwordBackend::instance()->initModules(CSwordBackend::OtherChange);
	backend->deleteOrphanedIndices();
	
	// Let's run...
    BtMini::setActiveWidget(BtMini::worksWidget());
    
    return app.exec();
}
