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
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QTextStream>
#include <QTimer>
#include <QtDebug>

#ifdef Q_OS_WIN32
#include <windows.h>
//#include <CRTDBG.H>
#endif

#include "backend/config/cbtconfig.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/btstringmgr.h"
#include "util/directory.h"

#include <SWLog.h>

#include "models/btminimoduletextmodel.h"
#include "ui/btminimenu.h"
#include "ui/btminipanel.h"
#include "view/btminiview.h"


// Main stacked widget
class BtMiniMainWidget : public QStackedWidget
{
public:
	BtMiniMainWidget(QWidget *parent=0) : QStackedWidget(parent)
	{
	    setAttribute(Qt::WA_DeleteOnClose);
	    setFrameStyle(QFrame::NoFrame);
        _saveTimer = startTimer(5*60*1000);
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
        w->showNormal();
#endif
    }
    
    return w;
}

QWidget * BtMini::worksWidget()
{
    static QWidget *w = 0;
    
    if(!w)
    {
        w = new QWidget(mainWidget());

        //w->resize(QSize(480, 640));
        //w->setMaximumSize(QSize(480, 640));
        //w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		qobject_cast<QStackedWidget*>(mainWidget())->addWidget(w);

        BtMiniView *v = new BtMiniView(w);

        v->setTopShadowEnabled(true);

        // Setup controls
        QPushButton *b1 = new QPushButton(v->style()->standardIcon(QStyle::SP_ArrowLeft), QString(), w);
        QPushButton *b2 = new QPushButton(v->style()->standardIcon(QStyle::SP_ArrowRight), QString(), w);
        QPushButton *b3 = new QPushButton(QString("Module"), w);
        QPushButton *b4 = new QPushButton(QString("Place"), w);

        QObject::connect(b1, SIGNAL(clicked()), v, SLOT(slideLeft()));
        QObject::connect(b2, SIGNAL(clicked()), v, SLOT(slideRight()));

        const int maxSize = w->font().pixelSize()*2.0;
        const QSize iconSize(w->font().pixelSize()*1.3, w->font().pixelSize()*1.3);
        b1->setMaximumSize(maxSize, maxSize);
        b1->setIconSize(iconSize);
        b2->setMaximumSize(maxSize, maxSize);
        b2->setIconSize(iconSize);

        QFont f = w->font();
        f.setPixelSize(f.pixelSize() * 0.8);
        f.setWeight(QFont::DemiBold);
        b3->setFont(f);
        b4->setFont(f);

        b3->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        b4->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << 
            BtMiniPanel::Search << BtMiniPanel::Exit, w);

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
					places = QStringList() << CBTConfig::getDefault(CBTConfig::openPlaces)[1];
					break;
				}
		}	

		Q_ASSERT(modules.size() > 0);

		// Setup model
		BtMiniModuleTextModel *m = new BtMiniModuleTextModel(modules, v);

		v->setModel(m);

		QObject::connect(v, SIGNAL(currentChanged(const QModelIndex &)), m, SLOT(updateIndicators(const QModelIndex &)));
		QObject::connect(v, SIGNAL(longPressed(const QModelIndex &)), m, SLOT(openContext(const QModelIndex &)));
		QObject::connect(v, SIGNAL(servicePressed(const QModelIndex &)), m, SLOT(openMenu(const QModelIndex &)));

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
    }
    
    return w;
}

QWidget * BtMini::searchWidget()
{
	static QWidget *w = 0;

	if(!w)
	{
		w = new QWidget(mainWidget());
		qobject_cast<QStackedWidget*>(mainWidget())->addWidget(w);

		BtMiniView *v = new BtMiniView(w);
		v->setTopShadowEnabled(true);

		// Setup controls
		QLineEdit *le = new QLineEdit(w);

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

		QObject::connect(le, SIGNAL(textChanged(const QString &)), m, SLOT(setSearchText(const QString &)));
		QObject::connect(le, SIGNAL(returnPressed()), m, SLOT(startSearch()));
	}

	return w;
}

void BtMini::setActiveWidget(QWidget *widget)
{
    qobject_cast<QStackedWidget*>(mainWidget())->setCurrentWidget(widget);
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

/** Application entry. */
int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN32
    //_CrtSetBreakAlloc(101307);
    //_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

	qInstallMsgHandler(BtMiniMessageHandler);
    //QApplication::setGraphicsSystem("opengl");
    
    // TODO set style for main widget
    QApplication::setStyle("mini");
    
	// Init application
	QApplication app(argc, argv);
    
	if(!util::directory::initDirectoryCache())
	{
		qFatal("Init Application: Error initializing directory cache!");
		return -1;
	}
	
    app.setApplicationName("BibleTime Mini");
    app.setOrganizationName("Crosswire");
	app.setApplicationVersion("2.9.0");
    app.setAutoSipEnabled(true);

	CBTConfig::set(CBTConfig::bibletimeVersion, app.applicationVersion());

	// TODO install translators

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
