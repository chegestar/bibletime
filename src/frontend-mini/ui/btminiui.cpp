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
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QtCore/qmath.h>
#include <QtDebug>

#include "backend/managers/cswordbackend.h"
#include "backend/config/btconfig.h"
#include "util/cresmgr.h"
#include "util/geticon.h"

#include "btmini.h"
#include "btminimenu.h"
#include "btminipanel.h"
#include "btminiui.h"
#include "models/btminimodulesmodel.h"
#include "models/btminimoduletextmodel.h"
#include "models/btminisettingsmodel.h"
#include "view/btminiview.h"

#if QT_VERSION >= 0x050000
#include <QAbstractNativeEventFilter>
#endif


class BtMiniUiPrivate
{
public:
    BtMiniUiPrivate(BtMiniUi *parent) : q_ptr(parent)
    {
        _mainWidget = 0;
        _worksWidget = 0;
        _searchWidget = 0;
        _installerWidget = 0;
        _settingsWidget = 0;
        _installModel = 0;
        _haveBible = false;
    }
    ~BtMiniUiPrivate()
    {
        ;
    }


    class BtMiniMainWidget : public QStackedWidget
    {
    public:
        BtMiniMainWidget() : QStackedWidget()
        {
            setAttribute(Qt::WA_DeleteOnClose);
            setFrameStyle(QFrame::NoFrame);
            _saveTimer = startTimer(5*60*1000);

            setWindowTitle("BibleTime Mini");
            setWindowIcon(util::getIcon(CResMgr::mainWindow::icon));

#if QT_VERSION >= 0x050200
            QObject::connect(QApplication::instance(), SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                    BtMiniUi::instance(), SLOT(applicationStateChanged()));
#endif
        }

        ~BtMiniMainWidget()
        {
            saveConfig();
        }

        /** Save opened session. */
        static void saveConfig()
        {
            qDebug("Save config.");

            if(BtMiniUi::instance()->d_func()->_worksWidget)
            {
                QModelIndexList list = BtMiniUi::instance()->worksView()->currentIndexes();
                QModelIndex index = BtMiniUi::instance()->worksView()->currentIndex();
                QStringList modules, places;

                for(int i = 0; i < list.size(); ++i)
                {
                    if(list[i] == index)
                        btConfig().setValue<int>("mini/openModule", i);

                    modules.append(list[i].data(BtMini::ModuleRole).toString());
                    places.append(list[i].data(BtMini::PlaceRole).toString());
                }

                btConfig().setValue<QStringList>("mini/openModules", modules);
                btConfig().setValue<QStringList>("mini/openPlaces", places);

                btConfig().sync();
            }
            qDebug("Save config: ok.");
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
            //resize(QApplication::desktop()->screenGeometry().size());
            showMaximized();
        #elif defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR) || defined(Q_WS_MAEMO_5) || defined(Q_WS_X11)
            showFullScreen();
        #else
            show();
        #endif
        }

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
            //Q_CHECK_PTR(BtMiniUi::instance()->d_func()->_mainWidget);
            //qobject_cast<QStackedWidget*>(BtMiniUi::instance()->d_func()->_mainWidget)->addWidget(this);
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


    void createMainWidget()
    {
        Q_ASSERT(_mainWidget == 0);

        // TODO qml frontend

        _mainWidget = new BtMiniMainWidget;

#if defined  Q_OS_WIN32 || (defined Q_OS_LINUX && !defined Q_OS_ANDROID)
        _mainWidget->resize(QSize(480, 640));
        _mainWidget->show();
        _mainWidget->raise();
#else
        _mainWidget->setOrientation(BtMiniMainWidget::ScreenOrientationAuto);
        _mainWidget->showExpanded();
#endif

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
        qreal sf = QApplication::desktop()->logicalDpiY() / 4.2;
#else
        qreal dpi = (QApplication::desktop()->physicalDpiX()/2.0 + QApplication::desktop()->physicalDpiY()/2.0);
        qreal screenSize = qSqrt(qPow(QApplication::desktop()->screenGeometry().width(), 2) +
                                 qPow(QApplication::desktop()->screenGeometry().height(), 2)) / dpi;

        // average of fixed physical (3mm) font size and screen resolution dependent size
        qreal sf = (dpi / (32.0 / screenSize) + (dpi / 8.4)) / 2.0;
#endif
        _sizeFactor = qMax((qreal)10.0, qMin((qreal)200.0, sf));

        qDebug() << "Surface:" << _mainWidget->size() << "  Screen:" << QApplication::desktop()->screenGeometry() <<
                    "  Dpi lx ly px py:" << _mainWidget->logicalDpiX() << _mainWidget->logicalDpiY() <<
                    _mainWidget->physicalDpiX() << _mainWidget->physicalDpiY() << "  Size factor:" << _sizeFactor;

        updateMainWidget();
    }

    void createWorksWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_worksWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _worksWidget = new BtMiniWidget(_mainWidget);
        _mainWidget->addWidget(_worksWidget);

        BtMiniView *v = new BtMiniView(_worksWidget);
        v->setTopShadow(true);
        v->setContinuousScrolling(btConfig().value<bool>("mini/miniContinuousScrolling", false));
        v->setWebKitEnabled(true);

        QFont f(v->font());
        f.setPixelSize(f.pixelSize() * btConfig().value<int>("mini/fontTextScale", 100) / 100);
        f.setWeight(QFont::Normal);
        v->setFont(f);

        v->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));

        // Setup controls
        //QPushButton *b1 = new QPushButton(v->style()->standardIcon(QStyle::SP_ArrowLeft), QString(), _worksWidget);
        //QPushButton *b2 = new QPushButton(v->style()->standardIcon(QStyle::SP_ArrowRight), QString(), _worksWidget);
        //QPushButton *b3 = new QPushButton(QString("Work"), _worksWidget);
        //QPushButton *b4 = new QPushButton(QString("Place"), _worksWidget);

        QPushButton *b1 = q->makeButton("", v->style()->standardIcon(QStyle::SP_ArrowLeft));
        QPushButton *b2 = q->makeButton("", v->style()->standardIcon(QStyle::SP_ArrowRight));
        QPushButton *b3 = q->makeButton(BtMiniUi::tr("Work"));
        QPushButton *b4 = q->makeButton(BtMiniUi::tr("Place"));

        QObject::connect(b1, SIGNAL(clicked()), v, SLOT(slideLeft()));
        QObject::connect(b2, SIGNAL(clicked()), v, SLOT(slideRight()));

//        const int maxSize = _worksWidget->font().pixelSize()*2.0;
//        //const QSize iconSize(_worksWidget->font().pixelSize()*1.3, _worksWidget->font().pixelSize()*1.3);
//        b1->setMaximumSize(maxSize, maxSize);
//        b1->setIconSize(getIconSize(b1->icon()));
//        b2->setMaximumSize(maxSize, maxSize);
//        b2->setIconSize(getIconSize(b2->icon()));

//        f = _worksWidget->font();
//        f.setPixelSize(f.pixelSize() * 0.75);
//        f.setWeight(QFont::DemiBold);
//        b3->setFont(f);
//        b4->setFont(f);

//        b3->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
//        b4->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        BtMiniPanel *p = new BtMiniPanel(BtMiniPanel::Activities() << BtMiniPanel::Search <<
            BtMiniPanel::Installer << BtMiniPanel::Settings << BtMiniPanel::Exit, _worksWidget);
        p->layout()->setContentsMargins(0, 0, 0, 0);

        // Put into layout
        //QHBoxLayout *hl = new QHBoxLayout;
        QVBoxLayout *vl = new QVBoxLayout;

//        hl->addWidget(b1, Qt::AlignLeft);
//        hl->addWidget(b3, Qt::AlignCenter);
//        hl->addWidget(b4, Qt::AlignCenter);
//        hl->addWidget(b2, Qt::AlignRight);
        BtMiniPanel *pn = new BtMiniPanel;
        pn->addWidget(b1, Qt::AlignLeft);
        pn->addWidget(b3, Qt::AlignCenter);
        pn->addWidget(b4, Qt::AlignCenter);
        pn->addWidget(b2, Qt::AlignRight);

        vl->addWidget(pn);
        vl->addWidget(v);
        vl->addWidget(p);

        _worksWidget->setLayout(vl);

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

        QObject::connect(v, SIGNAL(currentChanged(const QModelIndex &)), m, SLOT(updateIndicators(const QModelIndex &)));
        QObject::connect(v, SIGNAL(shortPressed(const QModelIndex &)), m, SLOT(openContext(const QModelIndex &)));
        QObject::connect(v, SIGNAL(longPressed(const QModelIndex &)), m, SLOT(openMenu(const QModelIndex &)));

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

        QObject::connect(CSwordBackend::instance(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
            m, SLOT(modulesReloaded()));
    }

    void createSearchWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_searchWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _searchWidget = new BtMiniWidget(_mainWidget);
        _mainWidget->addWidget(_searchWidget);

        BtMiniView *v = new BtMiniView(_searchWidget);
        v->setTopShadow(true);
        v->setWebKitEnabled(btConfig().value<bool>("mini/useWebKit", false));
        changeFontSize(v, btConfig().value<int>("mini/fontTextScale", 100) / 100.0);

        QLineEdit *le = new QLineEdit(_searchWidget);
        le->setAlignment(Qt::AlignCenter);
        le->setPlaceholderText(BtMiniUi::tr("search string"));
        changeFontSize(le, 0.95);
        le->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

        QPushButton *pb = q->makeButton(QObject::tr("Back"), "mini-back.svg", "mini-back-night.svg");
        QPushButton *eb = q->makeButton("", "find.svg");

        BtMiniPanel *p = new BtMiniPanel;
        p->addWidget(pb, Qt::AlignLeft);
        p->addWidget(le, Qt::AlignCenter);
        p->addWidget(eb, Qt::AlignRight);

        // Put into layout
        QVBoxLayout *vl = new QVBoxLayout;

        //vl->addWidget(le);
        vl->addWidget(p);
        vl->addWidget(v);

        _searchWidget->setLayout(vl);

        // Setup model
        BtMiniModuleTextModel *m = new BtMiniModuleTextModel(QStringList() << "[Search]", v);

        v->setModel(m);

        QObject::connect(le, SIGNAL(textChanged(const QString &)), m, SLOT(setSearchText(const QString &)));
        QObject::connect(le, SIGNAL(returnPressed()), m, SLOT(startSearch()));
        QObject::connect(eb, SIGNAL(clicked()), m, SLOT(startSearch()));
        QObject::connect(v, SIGNAL(shortPressed(const QModelIndex &)), m, SLOT(openContext(const QModelIndex &)));
        QObject::connect(v, SIGNAL(longPressed(const QModelIndex &)), _worksWidget->findChild<BtMiniView*>(), SLOT(scrollTo(const QModelIndex &)));
        //QObject::connect(v, SIGNAL(longPressed(const QModelIndex &)), p->findChild<QPushButton*>("Close"), SIGNAL(clicked()));
        QObject::connect(pb, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(activatePreviousWidget()));
    }

    void createInstallerWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_installerWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _installerWidget = new BtMiniWidget(_mainWidget);
        _mainWidget->addWidget(_installerWidget);

        BtMiniView *v = new BtMiniView(_installerWidget);
        v->setTopShadow(true);

        BtMiniPanel *p = new BtMiniPanel;

        QPushButton *bb = 0;
        if(_worksWidget)
            bb = q->makeButton(QObject::tr("Back"), "mini-back.svg", "mini-back-night.svg");
#if BT_MINI_EXIT_BUTTON
        else
            bb = q->makeButton(QObject::tr("Exit"));
#endif
        if(bb) bb->setObjectName("back");

        //QPushButton *rb = new QPushButton(BtMiniUi::tr("Refresh"));
        QPushButton *rb = q->makeButton("", "refresh.svg");

        QLabel *lb = new QLabel(BtMiniModulesModel::tr("Updating") + "...", _installerWidget);
        lb->setAlignment(Qt::AlignCenter);
        lb->setObjectName("label");

        p->addWidget(lb, Qt::AlignCenter);
        if(bb) p->addWidget(bb, Qt::AlignLeft);
        p->addWidget(rb, Qt::AlignRight);

        QVBoxLayout *vl = new QVBoxLayout;
        vl->addWidget(p);
        vl->addWidget(v);

        _installerWidget->setLayout(vl);

        // setup model
        if(!_installModel)
            _installModel = new BtMiniModulesModel(_mainWidget);

        _installModel->setIndicator(lb);

        QObject::connect(v, SIGNAL(currentChanged(QModelIndex)), _installModel, SLOT(updateIndicators(QModelIndex)));
        v->disconnect(SIGNAL(clicked(const QModelIndex &)));
        QObject::connect(v, SIGNAL(clicked(const QModelIndex &)), _installModel, SLOT(installerQuery(const QModelIndex &)));

        v->setModel(_installModel);
        _installModel->updateIndicators();

        if(_worksWidget)
            QObject::connect(bb, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(activatePreviousWidget()));
        else if(bb)
            QObject::connect(bb, SIGNAL(clicked()), BtMiniUi::instance()->mainWidget(), SLOT(close()));
        QObject::connect(rb, SIGNAL(clicked()), _installModel, SLOT(backgroundDownload()));
    }

    void createSettingsWidget()
    {
        Q_Q(BtMiniUi);
        Q_ASSERT(_settingsWidget == 0);
        Q_CHECK_PTR(_mainWidget);

        _settingsWidget = new BtMiniWidget(_mainWidget);
        _mainWidget->addWidget(_settingsWidget);

        BtMiniView *v = new BtMiniView(_settingsWidget);
        v->setTopShadow(true);

        BtMiniSettingsModel *m = new BtMiniSettingsModel(v);
        v->setModel(m);

        QPushButton *bb = q->makeButton(QObject::tr("Back"), "mini-back.svg", "mini-back-night.svg");

        QLabel *lb = new QLabel(BtMiniUi::tr("Settings"));
        lb->setAlignment(Qt::AlignCenter);

        BtMiniPanel *p = new BtMiniPanel;
        p->addWidget(lb, Qt::AlignCenter);
        p->addWidget(bb, Qt::AlignLeft);

        QVBoxLayout *vl = new QVBoxLayout;
        vl->addWidget(p);
        vl->addWidget(v);

        _settingsWidget->setLayout(vl);

        QObject::connect(v, SIGNAL(clicked(const QModelIndex &)), m, SLOT(clicked(const QModelIndex &)));
        QObject::connect(bb, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(activatePreviousWidget()));
    }


    void changeFontSize(QWidget *w, double factor)
    {
        QFont f(w->font());
        f.setPixelSize(f.pixelSize() * factor);
        w->setFont(f);
    }

    void updateMainWidget()
    {
        Q_CHECK_PTR(_mainWidget);

        QFont f = _mainWidget->font();
        f.setPixelSize(_sizeFactor * btConfig().value<int>("mini/fontScale", 100) / 100.0);
        _mainWidget->setFont(f);

        QString s = btConfig().value<QString>("mini/miniStyle", "mini");
        if(s != _mainWidget->style()->objectName())
            QApplication::setStyle(s);
    }

    /** Return basic icon size for BtMini interface. */
    QSize getIconSize(QIcon i)
    {
        int h = _mainWidget->font().pixelSize();
        if(h < 0)
        {
            QFontInfo i(_mainWidget->font());
            h = i.pixelSize();
        }
        h *= 1.6;
        QSize s(i.actualSize(QSize(h, h)));
        return QSize(h / s.height() * s.width(), h);
    }

    /** Do work to switch from current widget type. */
    void switchFrom()
    {
        // sleep works
        if(_worksWidget && _mainWidget->currentWidget() == _worksWidget)
            BtMiniUi::instance()->worksView()->setSleep(true);

        if(_resetFlag & Main)
            updateMainWidget();

        if(_resetFlag & Works && _worksWidget)
        {
            _mainWidget->saveConfig();

            //delete _worksWidget;
            _worksWidget->deleteLater();
            _worksWidget = 0;
        }
        if(_resetFlag & Rest && _installerWidget)
        {
            //delete _installerWidget;
            _installerWidget->deleteLater();
            _installerWidget = 0;
        }
        if(_resetFlag & Rest && _settingsWidget)
        {
            //delete _settingsWidget;
            _settingsWidget->deleteLater();
            _settingsWidget = 0;
        }
        if(_resetFlag & Rest && _searchWidget)
        {
            //delete _searchWidget;
            _searchWidget->deleteLater();
            _searchWidget = 0;
        }

        _resetFlag = 0;
    }


    BtMiniMainWidget            *_mainWidget;
    QWidget                     *_worksWidget;
    QWidget                     *_searchWidget;
    QWidget                     *_installerWidget;
    QWidget                     *_settingsWidget;
    QList<QWidget*>              _contextWidgets;

    BtMiniModulesModel          *_installModel;

    bool                         _haveBible;

    qreal                        _sizeFactor;

    enum WidgetFlag
    {
        Main = 1 << 0,
        Works = 1 << 1,
        Rest = 1 << 2,

        Search = 1 << 3,
        Installer = 1 << 4,
        Settings = 1 << 5,
        Context = 1 << 6
    };
    QFlags<WidgetFlag>           _resetFlag;
    QList<WidgetFlag>            _widgetStack;


    Q_DECLARE_PUBLIC(BtMiniUi);
    BtMiniUi * const             q_ptr;
};

BtMiniUi::BtMiniUi(QObject *parent)
    : QObject(parent)
    , d_ptr(new BtMiniUiPrivate(this))
{
    ;
}

BtMiniUi::~BtMiniUi()
{
    delete d_ptr;
}

void BtMiniUi::show()
{
    Q_D(BtMiniUi);

	// need to do queued connection otherwise install dialogs wontn't close
	qRegisterMetaType<CSwordBackend::SetupChangedReason>("CSwordBackend::SetupChangedReason");
    connect(CSwordBackend::instance(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
		this, SLOT(modulesReloaded()), Qt::QueuedConnection);

    foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
        if(m->type() == CSwordModuleInfo::Bible)
        {
            d->_haveBible = true;
            break;
        }

    d->createMainWidget();

    if(!d->_haveBible)
        activateInstaller();
    else
        activateWorks();
}

QWidget *BtMiniUi::mainWidget()
{
    Q_D(BtMiniUi);
    return d->_mainWidget;
}

QWidget *BtMiniUi::worksWidget()
{
    Q_D(BtMiniUi);
    return d->_worksWidget;
}

BtMiniView *BtMiniUi::worksView()
{
    Q_D(BtMiniUi);

    if(!d->_worksWidget)
        return 0;
    return d->_worksWidget->findChild<BtMiniView*>();
}

BtMiniView *BtMiniUi::searchView()
{
    Q_D(BtMiniUi);

    if(!d->_searchWidget)
        return 0;
    return d->_searchWidget->findChild<BtMiniView*>();
}

void BtMiniUi::resetWidgets(bool main, bool works, bool rest)
{
    Q_D(BtMiniUi);

    d->_resetFlag = 0;
    if(main)
        d->_resetFlag |= BtMiniUiPrivate::Main;
    if(works)
        d->_resetFlag |= BtMiniUiPrivate::Works;
    if(rest)
        d->_resetFlag |= BtMiniUiPrivate::Rest;
}

QWidget* BtMiniUi::activateNewContextWidget()
{
    Q_D(BtMiniUi);

    d->switchFrom();

    d->_widgetStack.append(BtMiniUiPrivate::Context);
    d->_contextWidgets.append(new QWidget(d->_mainWidget));
    d->_mainWidget->addWidget(d->_contextWidgets.last());
    d->_mainWidget->setCurrentWidget(d->_contextWidgets.last());

    return d->_contextWidgets.last();
}

QWidget *BtMiniUi::currentContextWidget()
{
    Q_D(BtMiniUi);

    Q_ASSERT(d->_widgetStack.last() == BtMiniUiPrivate::Context);
    return d->_contextWidgets.last();
}

QPushButton* BtMiniUi::makeButton(QString text, QString icon, QString invertedIcon)
{
    QPushButton *b = new QPushButton(text);
    b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    if(!icon.isEmpty())
    {
        Q_D(BtMiniUi);

        if(!invertedIcon.isEmpty() && d->_mainWidget->style()->standardPalette().background().color().lightnessF() < 0.2)
            b->setIcon(util::getIcon(invertedIcon));
        else
            b->setIcon(util::getIcon(icon));

        b->setIconSize(d->getIconSize(b->icon()));
    }
    return b;
}

QPushButton *BtMiniUi::makeButton(QString text, QIcon icon)
{
    QPushButton *b = new QPushButton(text);
    b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    if(!icon.isNull())
    {
        Q_D(BtMiniUi);

        b->setIcon(icon);
        b->setIconSize(d->getIconSize(b->icon()));
    }
    return b;
}

bool BtMiniUi::activatePreviousWidget()
{
    Q_D(BtMiniUi);

    if(d->_widgetStack.size() <= 1)
        return false;

    if(d->_widgetStack.last() == BtMiniUiPrivate::Context)
        d->_contextWidgets.takeLast()->deleteLater();

    d->_widgetStack.removeLast();

    switch(d->_widgetStack.last())
    {
        case BtMiniUiPrivate::Works: activateWorks(); break;
        case BtMiniUiPrivate::Installer: activateInstaller(); break;
        case BtMiniUiPrivate::Settings: activateSettings(); break;
        case BtMiniUiPrivate::Search: activateSearch(); break;
        case BtMiniUiPrivate::Context:
            d->switchFrom();
            d->_mainWidget->setCurrentWidget(d->_contextWidgets.last());
            break;
        default:
            Q_ASSERT(false);
    }
    return true;
}

void BtMiniUi::applicationStateChanged()
{
    Q_D(BtMiniUi);

#if QT_VERSION >= 0x050200
    if(QApplication::applicationState() == Qt::ApplicationSuspended)
        d->_mainWidget->saveConfig();
#endif
}

void BtMiniUi::activateWorks()
{
    Q_D(BtMiniUi);

    d->switchFrom();

    for(int i = d->_widgetStack.size() - 1; i >= 0; --i)
        if(d->_widgetStack[i] == BtMiniUiPrivate::Works)
        d->_widgetStack.removeAt(i);
    d->_widgetStack.append(BtMiniUiPrivate::Works);

    if(!d->_worksWidget)
        d->createWorksWidget();

    d->_mainWidget->setCurrentWidget(d->_worksWidget);
    worksView()->setSleep(false);

    // tips
    static bool shown = false;
    if(!shown && btConfig().value<bool>("mini/showTipAtStartup", true))
    {
        if(BtMiniMenu::execTip(BtMiniSettingsModel::standardData(BtMiniSettingsModel::tipWorksAddon).toString() +
                            BtMiniSettingsModel::standardData(BtMiniSettingsModel::tipWorks).toString()) == 1)
            btConfig().setValue<bool>("mini/showTipAtStartup", false);
        shown = true;
    }
}

void BtMiniUi::activateInstaller()
{
    Q_D(BtMiniUi);

    d->switchFrom();

    for(int i = d->_widgetStack.size() - 1; i >= 0; --i)
        if(d->_widgetStack[i] == BtMiniUiPrivate::Installer)
        d->_widgetStack.removeAt(i);
    d->_widgetStack.append(BtMiniUiPrivate::Installer);

    if(!d->_installerWidget)
        d->createInstallerWidget();

    d->_mainWidget->setCurrentWidget(d->_installerWidget);
}

void BtMiniUi::activateSearch()
{
    Q_D(BtMiniUi);

    d->switchFrom();

    for(int i = d->_widgetStack.size() - 1; i >= 0; --i)
        if(d->_widgetStack[i] == BtMiniUiPrivate::Search)
        d->_widgetStack.removeAt(i);
    d->_widgetStack.append(BtMiniUiPrivate::Search);

    if(!d->_searchWidget)
        d->createSearchWidget();

    d->_mainWidget->setCurrentWidget(d->_searchWidget);
}

void BtMiniUi::activateSettings()
{
    Q_D(BtMiniUi);

    d->switchFrom();

    for(int i = d->_widgetStack.size() - 1; i >= 0; --i)
        if(d->_widgetStack[i] == BtMiniUiPrivate::Settings)
        d->_widgetStack.removeAt(i);
    d->_widgetStack.append(BtMiniUiPrivate::Settings);

    if(!d->_settingsWidget)
        d->createSettingsWidget();

    d->_mainWidget->setCurrentWidget(d->_settingsWidget);
}

void BtMiniUi::modulesReloaded()
{
    Q_D(BtMiniUi);

    if(!d->_haveBible)
    {
        bool haveBible = false;
        foreach(CSwordModuleInfo *m, CSwordBackend::instance()->moduleList())
            if(m->type() == CSwordModuleInfo::Bible)
            {
                haveBible = true;
                break;
            }

        if(haveBible)
        {
            d->_haveBible = true;

            // refresh panel
            //delete d->_installerWidget->findChild<BtMiniPanel*>();
            //d->_installerWidget->layout()->addWidget(new BtMiniPanel(BtMiniPanel::Activities() <<
            //    BtMiniPanel::Refresh << BtMiniPanel::Close, d->_installerWidget));

//            qDebug("modulesReloaded & QPushButton");

//            QPushButton *b = d->_installerWidget->findChild<BtMiniPanel*>()->findChild<QPushButton*>("back");
//            if(b)
//            {
//                connect(b, SIGNAL(clicked()), BtMiniUi::instance(), SLOT(activatePreviousWidget()), Qt::UniqueConnection);
//                b->setText(tr("Back"));
//            }

            resetWidgets(false, false, true);
            activateWorks();
        }
    }
}
