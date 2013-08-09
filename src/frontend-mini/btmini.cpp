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

#include <QtCore/qmath.h>
#include <QtGlobal>
#include <QtPlugin>
#include <QtDebug>

#include <QApplication>
#include <QBitmap>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QFile>
#include <QFontDatabase>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QStringListModel>
#include <QStackedWidget>
#include <QStyle>
#include <QStyleFactory>
#include <QTextCodec>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QTranslator>

#include <swlog.h>

#include "backend/config/btconfig.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/btinstallbackend.h"
#include "backend/btinstallmgr.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/btstringmgr.h"
#include "util/directory.h"
#include "util/cresmgr.h"
#include "util/geticon.h"
#include "bibletimeapp.h"

#include "btmini.h"
#include "models/btminimoduletextmodel.h"
#include "models/btminimodelsmodel.h"
#include "models/btminimodulenavigationmodel.h"
#include "models/btminisettingsmodel.h"
#include "ui/btminimenu.h"
#include "ui/btminipanel.h"
#include "view/btminiview.h"
#include "view/btminilayoutdelegate.h"

#ifdef BT_MINI_QML
#include <QtQml>
#include <QtQuick>
#endif
#if QT_VERSION >= 0x050000
#include <QAbstractNativeEventFilter>
#endif
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#ifdef ANDROID
#include <android/log.h>
#endif
#ifdef Q_OS_SYMBIAN
#include <hwrmvibra.h>
#endif
#ifdef Q_OS_IOS
#import <AudioToolbox/AudioToolbox.h>
#endif


// Vibration
#ifdef ANDROID
#include <jni.h>

jmethodID   jniBtMiniVibrateMethodId = 0;
JavaVM     *jniBtMiniVm;
jclass      jniBtMiniClass = 0;

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv  *p_env;
    jniBtMiniVm = vm;

    if(jniBtMiniVm->GetEnv((void **)&p_env, JNI_VERSION_1_6) != JNI_OK)
    {
        qWarning("Jni can't get environment");
        return JNI_VERSION_1_6;
    }

#if QT_VERSION < 0x050000
    if(!(jniBtMiniClass = (*p_env).FindClass("org/kde/necessitas/origo/QtActivity")))
#else
    if(!(jniBtMiniClass = (*p_env).FindClass("org/qtproject/qt5/android/bindings/QtActivity")))
#endif
    {
        qWarning("Jni can't get class");
        return JNI_VERSION_1_6;
    }

    if(!(jniBtMiniVibrateMethodId = p_env->GetStaticMethodID(jniBtMiniClass,"vibrate", "(J)V")))
        qWarning("Jni can't get method");

    return JNI_VERSION_1_6;
}
#endif

#ifdef Q_OS_WINCE
#include  <windows.h>
#include  <nled.h>

extern "C"
{
    BOOL WINAPI NLedSetDevice(UINT nDeviceId, void *pInput);
};
#endif

void BtMini::vibrate(int milliseconds)
{
    if(btConfig().value<bool>("mini/disableVibration", false))
        return;

#ifdef Q_OS_WINCE
    NLED_SETTINGS_INFO settings;
    settings.LedNum = 1;
    settings.OffOnBlink = true;
    NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);

    Sleep(milliseconds);

    settings.OffOnBlink = false;
    NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);
#endif

#ifdef ANDROID
    JNIEnv  *p_env;

    if(jniBtMiniVm->AttachCurrentThread(&p_env, 0) < 0)
    {
        qWarning("Jni can't attach to thread");
        return;
    }

    if(!jniBtMiniClass || !jniBtMiniVibrateMethodId)
    {
        qWarning("Have no Jni classes");
        return;
    }

    jlong ms = milliseconds;
    p_env->CallStaticVoidMethod(jniBtMiniClass, jniBtMiniVibrateMethodId, ms);

    jniBtMiniVm->DetachCurrentThread();
#endif

#ifdef Q_OS_SYMBIAN
    static bool initiated = false;
    static QScopedPointer<CHWRMVibra> v;
    if(initiated == false)
    {
        TRAPD(err, v.reset(CHWRMVibra::NewL()));
        if(err != KErrNone)
            qDebug() << "Can not create Vibra, leave:" << err;

        initiated = true;
    }

    TRAPD(err, v.data()->StartVibraL(milliseconds, 50));
    if(err != KErrNone)
        qDebug() << "Can not vibrate, leave:" << err;
#endif

#ifdef Q_OS_IOS
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
#endif
}


bool haveBible()
{
    foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
        if(m->type() == CSwordModuleInfo::Bible)
            return true;
    return false;
}

void changeFontSize(QWidget *w, double factor)
{
    QFont f(w->font());
    f.setPixelSize(f.pixelSize() * factor);
    w->setFont(f);
}


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
        setWindowIcon(util::getIcon(CResMgr::mainWindow::icon));
    }

    ~BtMiniMainWidget()
    {
        saveConfig();
    }

    /** Save opened session. */
    static void saveConfig()
    {
        BtMiniView *v = BtMini::findView(BtMini::worksWidget());

        QModelIndexList list = v->currentIndexes();
        QStringList modules, places;

        for(int i = 0; i < list.size(); ++i)
        {
            if(list[i] == v->currentIndex())
                btConfig().setValue<int>("mini/openModule", i);

            modules.append(list[i].data(BtMini::ModuleRole).toString());
            places.append(list[i].data(BtMini::PlaceRole).toString());
        }

        btConfig().setValue<QStringList>("mini/openModules", modules);
        btConfig().setValue<QStringList>("mini/openPlaces", places);

        btConfig().sync();
    }

    /** */
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };
    void setOrientation(ScreenOrientation orientation)
    {
#if QT_VERSION < 0x050000
        #if defined(Q_OS_SYMBIAN)
            // If the version of Qt on the device is < 4.7.2, that attribute won't work
            if (orientation != ScreenOrientationAuto) {
                const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
                if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
                    qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
                    return;
                }
            }
        #endif // Q_OS_SYMBIAN

        Qt::WidgetAttribute attribute;
        switch (orientation) {
    #if QT_VERSION < 0x040702
        // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
        case ScreenOrientationLockPortrait:
            attribute = static_cast<Qt::WidgetAttribute>(128);
            break;
        case ScreenOrientationLockLandscape:
            attribute = static_cast<Qt::WidgetAttribute>(129);
            break;
        default:
        case ScreenOrientationAuto:
            attribute = static_cast<Qt::WidgetAttribute>(130);
            break;
    #else
        case ScreenOrientationLockPortrait:
            attribute = Qt::WA_LockPortraitOrientation;
            break;
        case ScreenOrientationLockLandscape:
            attribute = Qt::WA_LockLandscapeOrientation;
            break;
        default:
        case ScreenOrientationAuto:
            attribute = Qt::WA_AutoOrientation;
            break;
    #endif
        };
        setAttribute(attribute, true);
#endif
    }

    void showExpanded()
    {
    #if defined(Q_OS_WINCE)
        resize(QApplication::desktop()->size());
        show();
        showFullScreen();
    #elif defined (Q_OS_ANDROID)
        resize(QApplication::desktop()->screenGeometry().size());
        showMaximized();
    #elif defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR) || defined(Q_WS_MAEMO_5) || defined(Q_WS_X11)
        showFullScreen();
    #else
        show();
    #endif
    }


    static bool eventFilterFunction(void *message, long *result)
	{
#ifdef Q_OS_WINCE
		static bool enteredMainLoop = false;
		MSG *msg = reinterpret_cast<MSG *>(message);

		Q_CHECK_PTR(msg);

	    switch(msg->message)
		{
		case WM_PAINT:
			if(!enteredMainLoop)
				qDebug("Start");
			enteredMainLoop = true;
			break;
		case WM_ACTIVATE:
			if(enteredMainLoop)
			{
				int active = LOWORD(msg->wParam);
				bool minimized = (BOOL) HIWORD(msg->wParam);

				if(BtMini::mainWidget()->winId() == msg->hwnd)
					BtMini::findView(BtMini::worksWidget())->setSleep(active == WA_INACTIVE);
			}
			break;
		case WM_CLOSE: qDebug("Close"); break;
		case WM_HIBERNATE: qDebug("Low memory");break;
		}
#endif
		return false;
	 }

#if QT_VERSION >= 0x050000
    class EventFilterProcessor : public QAbstractNativeEventFilter
    {
    public:
        EventFilterProcessor() {;}

        bool nativeEventFilter(const QByteArray & eventType, void * message, long * result)
        {
            return eventFilterFunction(message, result);
        }
    };

    static EventFilterProcessor* eventFilterProcessor()
    {
        static EventFilterProcessor s;
        return &s;
    }
#endif

protected:
    QSize sizeHint() const
    {
        return QWidget::sizeHint().boundedTo(QApplication::desktop()->screenGeometry().size());
    }

    QSize minimumSizeHint() const
    {
        return QWidget::minimumSizeHint().boundedTo(QApplication::desktop()->screenGeometry().size());
    }

    void timerEvent(QTimerEvent *e)
    {
        if(e->timerId() == _saveTimer)
            saveConfig();
        else
            QStackedWidget::timerEvent(e);
    }

    void resizeEvent(QResizeEvent *e)
    {
        //qDebug() << "Resize main widget" << e->oldSize() << "to" << e->size() << "desktop" << QApplication::desktop()->screenGeometry();
        QStackedWidget::resizeEvent(e);
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
        return QWidget::sizeHint().boundedTo(parentWidget()->size());
    }

    QSize minimumSizeHint() const
    {
        return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
    }

    void resizeEvent(QResizeEvent *e)
    {
        //qDebug() << "Resize widget" << e->oldSize() << "to" << e->size() << "desktop";
        QWidget::resizeEvent(e);
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

QWidget *BtMini::widget(BtMiniState type)
{
    switch(type)
    {
    default:
        Q_ASSERT(false);
    }

    return 0;
}

BtMiniView *BtMini::view(BtMiniState type)
{
    return findView(widget(type));
}

void BtMini::reset(BtMiniState type)
{
    ;
}



QWidget * BtMini::mainWidget(bool fontSizeChanged, QString newStyle)
{
    static BtMiniMainWidget *w = 0;
    static QString style;
    static qreal sizeFactor = 10.0; // default font size


    if(!newStyle.isEmpty())
    {
        style = newStyle;
        return 0;
    }

    if(!style.isEmpty())
    {
        QApplication::setStyle(style);
        style.clear();
    }

    if(!w)
    {
        w = new BtMiniMainWidget;

        /*  device          resolution          logical dpi         physical dpi
            htc hd2         800x480             138                 254
                factor 30 is ok
            htc diamond                         192
            desktop         1920x1200           96                  72
                factor 26 is ok
            nexus 10        1454                200                 321
                factor 72 is ok
            nexus 7         800                 133                 214
                factor 45 is ok
        */

#if defined  Q_OS_WIN32 || (defined Q_OS_LINUX && !defined Q_OS_ANDROID)
        w->resize(QSize(480, 640));
        w->show();
        w->raise();

        sizeFactor = w->logicalDpiY() / 4.2;
#else
        w->setOrientation(BtMiniMainWidget::ScreenOrientationAuto);
        w->showExpanded();

        qreal dpi = (w->physicalDpiX()/2.0 + w->physicalDpiY()/2.0);
        qreal screenSize = qSqrt(qPow(QApplication::desktop()->screenGeometry().width(), 2) +
                                 qPow(QApplication::desktop()->screenGeometry().height(), 2)) / dpi;

        // average of fixed physical (3mm) font size and screen resolution dependent size
        sizeFactor = (dpi / (32.0 / screenSize) + (dpi / 8.4)) / 2.0;
#endif

        sizeFactor = qMax((qreal)10.0, qMin((qreal)200.0, sizeFactor));

        qDebug() << "Surface:" << w->size() << "  Screen:" << QApplication::desktop()->screenGeometry() <<
                    "  Dpi lx ly px py:" << w->logicalDpiX() << w->logicalDpiY() <<
                    w->physicalDpiX() << w->physicalDpiY() << "  Size factor:" << sizeFactor;

        fontSizeChanged = true;
    }

    if(fontSizeChanged)
    {
        QFont f = w->font();
        f.setPixelSize(sizeFactor * btConfig().value<int>("mini/fontScale", 100) / 100);
        w->setFont(f);
    }

    return w;
}


QWidget * BtMini::worksWidget(bool showTip, bool reset)
{
    static BtMiniWidget *w = 0;
	static bool recreate = false;
    static bool toolTipShown = false;

    if(reset)
    {
		if(w)
			recreate = true;
        return 0;
    }

	if(recreate)
	{
		recreate = false;

		BtMiniMainWidget::saveConfig();

		delete w;
		w = 0;
	}

    if(!w)
    {	
        if(!haveBible())
            return installerWidget(true);

        w = new BtMiniWidget(mainWidget());

        BtMiniView *v = new BtMiniView(w);
        v->setTopShadow(true);
        v->setContinuousScrolling(btConfig().value<int>("mini/miniContinuousScrolling", false));
        v->setWebKitEnabled(true);

        QFont f(v->font());
        f.setPixelSize(f.pixelSize() * btConfig().value<int>("mini/fontTextScale", 100) / 100);
        f.setWeight(QFont::Normal);
        v->setFont(f);

        v->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));

        if(btConfig().value<int>("mini/useRenderCaching", false))
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

        f = w->font();
        f.setPixelSize(f.pixelSize() * 0.75);
        f.setWeight(QFont::DemiBold);
        b3->setFont(f);
        b4->setFont(f);

        b3->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        b4->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << BtMiniPanel::Search <<
			BtMiniPanel::Installer << BtMiniPanel::Settings << BtMiniPanel::Exit, w);

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
        QStringList modules = btConfig().value<QStringList>("mini/openModules", QStringList() << "KJV");
        QStringList places = btConfig().value<QStringList>("mini/openPlaces", QStringList() << "John 1:1");

        QStringList moduleNames;

        foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
            moduleNames << m->name();

        for(int i = 0; i < modules.size(); ++i)
        {
            // parallel module
            if(modules[i].contains(','))
            {
                QStringList sl(modules[i].split(','));
                QString rs;
                foreach(QString s, sl)
                {
                    if(!moduleNames.contains(s))
                        qDebug() << "Remove reference to inexistent module in parallel module" << s;
                    else
                        rs.append(rs.isEmpty() ? s : ',' + s);
                }

                modules[i] = rs;

                if(!rs.isEmpty())
                    continue;
            }

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
                    places = QStringList() << btConfig().value<QStringList>("mini/openPlaces", QStringList() << "John 1:1")[0];
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
        for(int i = 0, c = btConfig().value<int>("mini/openModule", 0); i < modules.size(); ++i)
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

        // tips
        if(showTip && btConfig().value<int>("mini/showTipAtStartup", true) && !toolTipShown)
        {
            if(BtMiniMenu::execTip(BtMiniSettingsModel::standardData(BtMiniSettingsModel::tipWorksAddon).toString() +
                                BtMiniSettingsModel::standardData(BtMiniSettingsModel::tipWorks).toString()) == 1)
                btConfig().setValue<int>("mini/showTipAtStartup", false);
            toolTipShown = true;
        }
    }

    return w;
}

QWidget * BtMini::searchWidget(bool reset)
{
    static BtMiniWidget *w = 0;

    if(reset)
    {
		if(w)
			delete w;
        return w = 0;
    }

    if(!w)
    {
        w = new BtMiniWidget(mainWidget());

        BtMiniView *v = new BtMiniView(w);
        v->setTopShadow(true);
        v->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));
        changeFontSize(v, btConfig().value<int>("mini/fontTextScale", 100) / 100);

        // Setup controls
        QLineEdit *le = new QLineEdit(w);
        le->setAlignment(Qt::AlignCenter);
        le->setPlaceholderText(tr("type and press Enter"));
        changeFontSize(le, 1.2);

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
        connect(v, SIGNAL(longPressed(const QModelIndex &)), findView(worksWidget()), SLOT(scrollTo(const QModelIndex &)));
        connect(v, SIGNAL(longPressed(const QModelIndex &)), p->findChild<QPushButton*>("Close"), SIGNAL(clicked()));
    }

    return w;
}

/** */
QWidget * BtMini::installerWidget(bool firstTime, bool reset)
{
    static BtMiniWidget *w = 0;

    if(reset)
    {
		if(w)
			delete w;
        return w = 0;
    }

    if(!w)
    {
        w = new BtMiniWidget(mainWidget());

        BtMiniView *v = new BtMiniView(w);
        v->setTopShadow(true);
        changeFontSize(v, btConfig().value<int>("mini/fontTextScale", 100) / 100);

        BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() <<
            BtMiniPanel::Refresh << (firstTime ? BtMiniPanel::Exit : BtMiniPanel::Close), w);

        QLabel *lb = new QLabel(tr("No Remote Sources"), w);
        changeFontSize(lb, 0.9);
        lb->setAlignment(Qt::AlignCenter);
        lb->setMargin(lb->font().pixelSize() / 2);
        lb->setObjectName("label");

        // Put into layout
        QVBoxLayout *vl = new QVBoxLayout;

        vl->addWidget(lb, Qt::AlignCenter);
        vl->addWidget(v);
        vl->addWidget(p);

        w->setLayout(vl);

        updateRemoteSources(false);

		// scroll to proper language
		const CLanguageMgr::Language *sl = 0;
		QList<const CLanguageMgr::Language *> languges(CLanguageMgr::instance()->languages()->values());
		QString languageEnglish(QLocale::system().languageToString(QLocale::system().language()));

		foreach(const CLanguageMgr::Language *ll, languges)
		{
			if(ll->englishName() == languageEnglish)
			{
				sl = ll;
				break;
			}
		}

		if(sl && sl->isValid())
		{
			QModelIndexList il = v->model()->match(v->model()->index(0, 0), Qt::DisplayRole, sl->translatedName(), 2);
			if(il.size() == 1)
				v->scrollTo(il[0].child(0, 0));
		}
    }

    return w;
}


QWidget *BtMini::settingsWidget(bool reset)
{
    static QWidget *w = 0;
	static bool recreate = false;

    if(reset)
	{
		recreate = true;
		return 0;
	}

	if(recreate)
		delete w, w = 0, recreate = false;

    if(!w)
    {
        w = new BtMiniWidget(mainWidget());

        BtMiniView *v = new BtMiniView(w);
        //v->setTopShadow(true);

        QFont f(v->font());
        f.setPixelSize(f.pixelSize() * btConfig().value<int>("mini/fontTextScale", 100) / 100);
        v->setFont(f);

        BtMiniSettingsModel *m = new BtMiniSettingsModel(v);

        v->setModel(m);

        BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << BtMiniPanel::Close, w);

        QVBoxLayout *vl = new QVBoxLayout;

        vl->addWidget(v);
        vl->addWidget(p);

        w->setLayout(vl);

        QObject::connect(v, SIGNAL(clicked(const QModelIndex &)), m, SLOT(clicked(const QModelIndex &)));
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
            bool hb = haveBible();

            BtInstallMgr *im = index.model()->findChild<BtInstallMgr*>();

            QScopedPointer<BtMiniMenu> dialog(BtMiniMenu::createProgress(tr("Installing ...")));
			connect(im, SIGNAL(percentCompleted(int, int)), dialog.data(), SLOT(setValue(int)));
			dialog->show();

            //// find most parent index corresponding to remote source
            //QModelIndex si = index;
            //while(si.parent().isValid())
            //    si = si.parent();
            //sword::InstallSource is = BtInstallBackend::source(BtInstallBackend::sourceNameList(true)[si.row()]);

			sword::InstallSource is = BtInstallBackend::source(index.data(BtMini::RepositoryRole).toString());

            int status = im->installModule(CSwordBackend::instance(), 0, m->name().toLatin1(), &is);

            if (status != 0)
            {
                BtMiniMenu::execQuery(QString(tr("Module was not installed")));
                qDebug() << "Failed to install" << m->name() << status;
            }
            else if(!dialog->wasCanceled())
                CSwordBackend::instance()->reloadModules(CSwordBackend::AddedModules);

            // Bible installed, can switch to reader
            if(!hb && haveBible())
            {
                // refresh panel
                delete installerWidget()->findChild<BtMiniPanel*>();
                installerWidget()->layout()->addWidget(new BtMiniPanel(BtMiniPanel::Activities() <<
                    BtMiniPanel::Refresh << BtMiniPanel::Close, installerWidget()));

                setActiveWidget(worksWidget(false));
            }
        }
    }
}

void BtMini::updateRemoteSources(bool download)
{
    if(download)
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Setup model
    BtMiniView *v = findView(installerWidget());
    BtMiniModelsModel *m = new BtMiniModelsModel(v);
    QLabel *l = installerWidget()->findChild<QLabel*>("label");
    Q_CHECK_PTR(l);
    m->setIndicator(l);

    QObject::connect(v, SIGNAL(currentChanged(QModelIndex)), m, SLOT(updateIndicators(QModelIndex)));
    v->disconnect(SIGNAL(clicked(const QModelIndex &)));
    QObject::connect(v, SIGNAL(clicked(const QModelIndex &)), &BtMini::instance(), SLOT(installerQuery(const QModelIndex &)));

    QStringList ss(BtInstallBackend::sourceNameList(download));
    BtInstallMgr *im = new BtInstallMgr(m);

    foreach(QString s, ss)
    {
        sword::InstallSource is = BtInstallBackend::source(s);

        if(download)
            im->refreshRemoteSource(&is);

        CSwordBackend *be = BtInstallBackend::backend(is);

        if(be->moduleList().size() == 0)
            continue;

        BtBookshelfTreeModel::Grouping g(BtBookshelfTreeModel::GROUP_LANGUAGE);
        g.push_back(BtBookshelfTreeModel::GROUP_CATEGORY);

        BtBookshelfTreeModel *mm = new BtBookshelfTreeModel(g, m);
        mm->setDisplayFormat(QList<QVariant>() << BtBookshelfModel::ModuleNameRole << "<br/>"
            "<word-breaks/><font size=\"60%\" color=\"#555555\">" << BtBookshelfModel::ModuleDescriptionRole << "</font>");
        mm->setSourceModel(be->model());

        m->addModel(mm, s);
    }

    v->setModel(m);

    if(ss.size() > 0)
        m->updateIndicators(QModelIndex());

    if(download)
        QApplication::restoreOverrideCursor();
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
#if QT_VERSION < 0x050000
void BtMiniMessageHandler(QtMsgType type, const char *msg)
#else
void BtMiniMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#endif
{
#ifdef Q_OS_SYMBIAN
    static QFile f("c:\\btlog.txt");
#else
    static QFile f("log.txt");
#endif
    static bool r = f.open(QIODevice::WriteOnly);

#if QT_VERSION < 0x050000
    QString s(QString::fromLocal8Bit(msg) + QLatin1Char('\n'));
#else
    QString s(msg + '\n');
#endif
    QTextStream (&f) << s;

#ifdef ANDROID
    const int a[] = {ANDROID_LOG_DEBUG, ANDROID_LOG_WARN, ANDROID_LOG_ERROR, ANDROID_LOG_FATAL};
    __android_log_print(a[qMin((const int)type, 3)], "BtMini", s.toLocal8Bit());
#elif defined Q_OS_WIN && !(defined Q_OS_WINCE)
    OutputDebugString((wchar_t*)s.utf16());
#else
    printf(s.toLocal8Bit());
#endif
}


int main(int argc, char *argv[])
{
#ifdef Q_OS_IOS
    Q_IMPORT_PLUGIN(QSvgPlugin);
#endif

#if QT_VERSION < 0x050000
    qInstallMsgHandler(BtMiniMessageHandler);
    BibleTimeApp app(argc, argv);
    app.setEventFilter(BtMiniMainWidget::eventFilterFunction);
#else
    qInstallMessageHandler(BtMiniMessageHandler);
    BibleTimeApp app(argc, argv);
    app.installNativeEventFilter(BtMiniMainWidget::eventFilterProcessor());
#endif


    //registerMetaTypes();

    if(!util::directory::initDirectoryCache())
    {
        qFatal("Init Application: Error initializing directory cache!");
        return EXIT_FAILURE;
    }

    app.startInit();
    if (!app.initBtConfig()) {
        return EXIT_FAILURE;
    }

    app.setStyle(btConfig().value<QString>("mini/miniStyle", "mini"));
    btConfig().setValue("bibletimeVersion", app.applicationVersion());


    // install translators
    QString systemName(QLocale::system().name());

    qDebug() << "Select interface locale:" << systemName;

    foreach(QString s, QStringList() << "bibletime_ui_" << "bibletimemini_")
    {
        QTranslator *t = new QTranslator(&app);
        t->load(s + systemName, util::directory::getLocaleDir().canonicalPath());
        app.installTranslator(t);
    }

    QString errorMessage;
    new CDisplayTemplateMgr(errorMessage);
    if(!errorMessage.isNull())
    {
        qFatal(("Init Application: " + errorMessage).toLatin1());
        return -1;
    }

    // Init Sword
    sword::SWLog::setSystemLog(new BtMiniSwordLog);
    sword::SWLog::getSystemLog()->setLogLevel(btConfig().value<int>("mini/swordDebugLevel", sword::SWLog::LOG_ERROR));

    sword::StringMgr::setSystemStringMgr(new BtStringMgr);

    CSwordBackend *backend = CSwordBackend::createInstance();
    backend->booknameLanguage(btConfig().value<QString>("language", systemName));
    backend->initModules(CSwordBackend::OtherChange);
    backend->deleteOrphanedIndices();

    // Let's run...
#ifdef BT_MINI_QML
    QQuickView view;
    view.setSource(QUrl("qrc:/bibletime/qml/main.qml"));
    view.connect(view.engine(), SIGNAL(quit()), SLOT(close()));
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    BtMiniModuleNavigationModel mn(btConfig().getDefaultSwordModuleByType("standardBible")->name());

    QQmlContext *ctxt = view.rootContext();
    ctxt->setContextProperty("navigationModel", &mn);
    view.show();
#else
    BtMini::setActiveWidget(BtMini::worksWidget());
#endif

    return app.exec();
}
