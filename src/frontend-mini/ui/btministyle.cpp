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

#include <QAbstractItemView>
#include <QApplication>
#include <QCommonStyle>
#include <QLayout>
#include <QStyleOption>
#include <QStylePlugin>
#include <QPainter>
#include <QPushButton>
#include <QtDebug>
#include <qdrawutil.h>

class BtMiniStyle : public QCommonStyle
{
    Q_OBJECT

public:
    BtMiniStyle(bool night = false)
    {
        Q_INIT_RESOURCE(btministyle);

        _night = night;
        _menuFrame = 0;

        if(_night)
        {
            _palette.setColor(QPalette::Text, QColor(255, 255, 255));
            _palette.setColor(QPalette::Base, QColor(22, 22, 22));
            _palette.setColor(QPalette::Link, QColor("#62a4db"));

            _palette.setColor(QPalette::Highlight, QColor("#9cd2ff"));
            _palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        }
        else
        {
            _palette.setColor(QPalette::Text, QColor(0, 0, 0));
            _palette.setColor(QPalette::Base, QColor(255, 255, 255));
            _palette.setColor(QPalette::Link, QColor(127, 196, 255));

            _palette.setColor(QPalette::Highlight, QColor("#9cd2ff"));
            _palette.setColor(QPalette::HighlightedText, QColor("#000000"));
        }

        _palette.setColor(QPalette::Window, _palette.color(QPalette::Base));
        _palette.setColor(QPalette::Button, _palette.color(QPalette::Base));
        _palette.setColor(QPalette::WindowText, _palette.color(QPalette::Text));
        _palette.setColor(QPalette::ButtonText, _palette.color(QPalette::Text));

        QApplication::setPalette(_palette);
    }

    ~BtMiniStyle()
    {
        if(_menuFrame)
            delete _menuFrame;
    }

    void drawPrimitive(PrimitiveElement element, const QStyleOption *opt, QPainter *p, const QWidget *widget = 0) const
    {
        switch(element)
        {
        case PE_FrameMenu:
            if(const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame*>(opt))
            {
                const int m = _menuFrameWidth;
                qDrawBorderPixmap(p, frame->rect, QMargins(m, m, m, m), *_menuFrame);
            }
            return;
        case PE_PanelButtonCommand:
        case PE_PanelMenu:
            return;
        case PE_CustomBase + 1:
            {
                const int h = opt->fontMetrics.height();
                const int m = _night ? 200 : 100;

                QRect r(opt->rect.adjusted(0, 0, 0, h - opt->rect.height()));

                QLinearGradient g(r.topLeft(), r.bottomLeft());

                g.setColorAt(0.0, QColor(0, 0, 0, m));
                g.setColorAt(1.0, QColor(0, 0, 0, 0));

                p->setPen(Qt::NoPen);
                p->setBrush(g);
                p->drawRect(r);
            }
            return;
        }
        QCommonStyle::drawPrimitive(element, opt, p, widget);
    }

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
        const QWidget *widget = 0) const
    {
        switch(element)
        {
        case CE_ScrollBarSlider:
            {
                const int s = opt->rect.width() / (_night ? 3.6 : 3);
                p->setBrush(_night ? QColor(0, 0, 0) : QColor(255, 255, 255));
                p->drawRect(opt->rect.adjusted(s, (opt->rect.width() - s - s) / 2, -s, (opt->rect.width() - s - s) / -2));
                p->drawEllipse(opt->rect.adjusted(s, 0, -s, (opt->rect.width() - s * 2) - opt->rect.height()));
                p->drawEllipse(opt->rect.adjusted(s, opt->rect.height() - (opt->rect.width() - s * 2), -s, 0));
                return;
            }
        case CE_PushButtonBevel:
            break;
        case CE_PushButton:
            break;
        default:
            ;
        }
        QCommonStyle::drawControl(element, opt, p, widget);
    }

    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *option,
        QPainter *p, const QWidget *widget = 0 ) const
    {
        switch(cc)
        {
        case CC_ScrollBar:
            {
                if(const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option))
                {
                    p->setPen(Qt::NoPen);
                    p->setBrush(_palette.color(QPalette::Link)); // "#2c6799"
                    p->drawRect(scrollbar->rect);

                    if(scrollbar->subControls & SC_ScrollBarSlider)
                    {
                        QStyleOptionSlider to = *scrollbar;

                        to.rect = subControlRect(cc, &to, SC_ScrollBarSlider, widget);
                        if(to.rect.isValid())
                        {
                            if(!(scrollbar->activeSubControls& SC_ScrollBarSlider))
                                to.state &= ~(State_Sunken | State_MouseOver);

                            drawControl(CE_ScrollBarSlider, &to, p, widget);
                        }
                    }

                    QCommonStyle::drawComplexControl(cc, option, p, widget);

                    // top shadow
                    if(widget && widget->parentWidget() && widget->parentWidget()->parentWidget() &&
                       widget->parentWidget()->parentWidget()->property("topShadow").toBool())
                        drawPrimitive((PrimitiveElement)(PE_CustomBase + 1), option, p, widget);

                    //drawPrimitive((QStyle::PrimitiveElement)(QStyle::PE_CustomBase + 1), option, p);
                }
            }
            return;
        }

        QCommonStyle::drawComplexControl(cc, option, p, widget);
    }

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
    {
        switch(metric)
        {
        case PM_LayoutLeftMargin:
        case PM_LayoutTopMargin:
        case PM_LayoutRightMargin:
        case PM_LayoutBottomMargin:
        case PM_LayoutHorizontalSpacing:
        case PM_LayoutVerticalSpacing:
            return 0;
        case PM_MenuPanelWidth:
            return _menuFrameWidth;
        case PM_MaximumDragDistance:
            return -1;
        case PM_SliderThickness:
        case PM_ScrollBarExtent:
            {
                static int v = -2;

                if(v == -2)
                {
                    const QWidget *w = widget;
                    while(w->parentWidget())
                        w = w->parentWidget();
                    v = w->font().pixelSize() * 1.65;
                }

                return v;
            }
        }
        return QCommonStyle::pixelMetric(metric, option, widget);
    }

	QPalette standardPalette() const
	{
        return _palette;
	}


	void polish(QWidget *widget)
    {
        // FIX initialization in constructor cause crash
        static bool initialized = false;
        if(!initialized)
        {
            _menuFrame = new QPixmap(_night ? ":/style-mini/menu-night-frame.png"
                                            : ":/style-mini/menu-frame.png");
            _menuFrameWidth = _menuFrame->width() / 3;
            initialized = true;
        }

        // FIX on Symbian palettes are not applied
#ifdef Q_OS_SYMBIAN
        if(!widget->parentWidget())
            widget->setPalette(_palette);
        else
            widget->setPalette(widget->palette());
#endif

        if(QString(widget->metaObject()->className()) == "BtMiniMenu")
        {
            QMargins m = widget->contentsMargins();
            widget->setContentsMargins(m.left()+_menuFrameWidth, m.top()+_menuFrameWidth,
                m.right()+_menuFrameWidth, m.bottom()+_menuFrameWidth);
            widget->setAttribute(Qt::WA_TranslucentBackground);
        }

        if(QString(widget->metaObject()->className()) == "BtMiniView")
        {
            QAbstractItemView *l = qobject_cast<QAbstractItemView*>(widget);
            if(l)
            {
                l->viewport()->setAutoFillBackground(false);
                l->setAutoFillBackground(false);
            }
        }

        // bottom panel widget
        if(QString(widget->metaObject()->className()) == "BtMiniPanel")
		{
			widget->setAutoFillBackground(true);

            QPalette p = widget->palette();
            if(_night)
            {
                p.setColor(QPalette::Window, QColor(0, 0, 0));
                p.setColor(QPalette::Button, QColor(0, 0, 0));
                p.setColor(QPalette::WindowText, QColor(255, 255, 255));
                p.setColor(QPalette::ButtonText, QColor(255, 255, 255));
            }
            else
            {
                p.setColor(QPalette::Window, QColor(102, 102, 102));
                p.setColor(QPalette::Button, QColor(102, 102, 102));
                p.setColor(QPalette::WindowText, QColor(255, 255, 255));
                p.setColor(QPalette::ButtonText, QColor(255, 255, 255));
            }

			foreach(QWidget *w, widget->findChildren<QWidget*>() << widget)
				w->setPalette(p);
        }

#ifdef Q_WS_WINCE
        // FIX on wince some why font size is not correct if not set explicitly
        {
            QFont f(widget->font());
            f.setPixelSize(f.pixelSize());
            widget->setFont(f);
        }
#endif
    }

    int styleHint(StyleHint sh, const QStyleOption *option, const QWidget *widget,
        QStyleHintReturn *hret) const
    {
        switch(sh)
        {
        case SH_ScrollBar_LeftClickAbsolutePosition:
            return true;
        }

        return QCommonStyle::styleHint(sh, option, widget, hret);
    }

    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *option,
        const QPoint &point, const QWidget *widget) const
    {
        switch(cc)
        {
        case CT_ScrollBar:
            return SC_ScrollBarGroove;
        }

        return QCommonStyle::hitTestComplexControl(cc, option, point, widget);
    }

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = 0) const
    {
        switch(element)
        {
        case SE_ProgressBarContents:
        case SE_ProgressBarGroove:
        case SE_ProgressBarLabel:
            return option->rect;
        }

        return QCommonStyle::subElementRect(element, option, widget);
    }

    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *option,
        SubControl sc, const QWidget *widget) const
    {
        switch(cc)
        {
        case CC_ScrollBar:
            {
                if(const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option))
                {
                    switch(sc)
                    {
                    case SC_ScrollBarSlider:
                        {
                            Q_ASSERT(scrollbar->orientation == Qt::Vertical);

                            const float vh = scrollbar->rect.height();
                            const float hh = scrollbar->maximum - scrollbar->minimum;
                            
                            // limit slider height
                            int sh = (int)(vh * (vh / (hh + vh)));
                            sh = qMax(scrollbar->rect.width(), sh);
                            //sh = qMin(scrollbar->rect.height() / 3 * 2, sh);

                            if(_night)
                                sh += (scrollbar->rect.height() - sh) * 0.33;
                            
                            const float vv = scrollbar->sliderPosition;
                            const float sp = qRound((vh - sh) * (vv / hh));

                            if(sh >= scrollbar->rect.height())
                                return QRect();

                            return scrollbar->rect.adjusted(0, sp,
                                    0, sp + sh - scrollbar->rect.height());
                        }
                    case SC_ScrollBarGroove:
                        return scrollbar->rect;
                    case SC_ScrollBarAddPage:
                    case SC_ScrollBarSubPage:
                        {
                            QRect slider = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider, widget);
                            if(sc == SC_ScrollBarSubPage)
                                return scrollbar->rect.adjusted(0, 0, 0,
                                    -scrollbar->rect.height() + slider.top());
                            else
                                return scrollbar->rect.adjusted(0, slider.bottom(), 0, 0);
                        }
                    default:
                        return QRect(scrollbar->rect.topLeft(), QSize());
                    }
                }
            }
            break;
        }

        return QCommonStyle::subControlRect(cc, option, sc, widget);
    }

    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz, const QWidget *widget) const
    {
        switch(ct)
        {
        case CT_PushButton:
            {
                QSize s = QCommonStyle::sizeFromContents(ct, opt, csz, widget);
                return QSize(s.width(), s.height() * 1.4);
            }
        }
        return QCommonStyle::sizeFromContents(ct, opt, csz, widget);
    }

#if QT_VERSION < 0x050000
public slots:
    QIcon standardIconImplementation(StandardPixmap icon, const QStyleOption *option = 0, const QWidget *widget = 0) const
#else
	QIcon standardIcon(StandardPixmap icon, const QStyleOption *option = 0, const QWidget *widget = 0) const
#endif
    {
        switch(icon)
        {
        case SP_ArrowLeft:
            {
                QIcon i(":/style-mini/arrow-left.png");
                i.addFile(":/style-mini/arrow-left-inactive.png", QSize(), QIcon::Disabled);
                return i;
            }
        case SP_ArrowRight:
            {
                QIcon i(":/style-mini/arrow-right.png");
                i.addFile(":/style-mini/arrow-right-inactive.png", QSize(), QIcon::Disabled);
                return i;
            }
        default:
            return standardIcon(icon, option, widget);
        }
    }

private:
    Q_DISABLE_COPY(BtMiniStyle)

    QPixmap *_menuFrame;
    int      _menuFrameWidth;

    bool     _night;

    QPalette _palette;
};

class BtMiniStylePlugin : public QStylePlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface")
#endif

public:
    BtMiniStylePlugin() {;}
    ~BtMiniStylePlugin() {;}

    QStyle * create(const QString &key)
    {
        if (key.toLower() == "mini")
            return new BtMiniStyle();
        if (key.toLower() == "mini-night")
            return new BtMiniStyle(true);
        return 0;
    }

    QStringList	keys() const
    {
        return QStringList() << "mini" << "mini-night";
    }
};


#include "moc_btministyle.cpp"


// HACK declare static plugin
#if QT_VERSION < 0x050000
QObject * qt_plugin_instance_BtMiniStylePlugin() Q_PLUGIN_INSTANCE(BtMiniStylePlugin)
#else

#ifdef QT_NO_DEBUG
static const unsigned char qt_pluginMetaData_BtMiniStyle[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    0x71, 0x62, 0x6a, 0x73, 0x01, 0x00, 0x00, 0x00,
    0xec, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0xd8, 0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 0x49, 0x49, 0x44, 0x00, 0x00, 0x00,
    0x28, 0x00, 0x6f, 0x72, 0x67, 0x2e, 0x71, 0x74,
    0x2d, 0x70, 0x72, 0x6f, 0x6a, 0x65, 0x63, 0x74,
    0x2e, 0x51, 0x74, 0x2e, 0x51, 0x53, 0x74, 0x79,
    0x6c, 0x65, 0x46, 0x61, 0x63, 0x74, 0x6f, 0x72,
    0x79, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x66, 0x61,
    0x63, 0x65, 0x00, 0x00, 0x9b, 0x0a, 0x00, 0x00,
    0x09, 0x00, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x4e,
    0x61, 0x6d, 0x65, 0x00, 0x11, 0x00, 0x42, 0x74,
    0x4d, 0x69, 0x6e, 0x69, 0x53, 0x74, 0x79, 0x6c,
    0x65, 0x50, 0x6c, 0x75, 0x67, 0x69, 0x6e, 0x00,
    0x5a, 0x00, 0xa0, 0x00, 0x07, 0x00, 0x76, 0x65,
    0x72, 0x73, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x05, 0x00, 0x64, 0x65,
    0x62, 0x75, 0x67, 0x00, 0x95, 0x12, 0x00, 0x00,
    0x08, 0x00, 0x4d, 0x65, 0x74, 0x61, 0x44, 0x61,
    0x74, 0x61, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x14, 0x03, 0x00, 0x00, 0x04, 0x00, 0x4b, 0x65,
    0x79, 0x73, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x6d, 0x69, 0x6e, 0x69, 0x00, 0x00,
    0x0a, 0x00, 0x6d, 0x69, 0x6e, 0x69, 0x2d, 0x6e,
    0x69, 0x67, 0x68, 0x74, 0x8b, 0x01, 0x00, 0x00,
    0x8b, 0x02, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00,
    0x44, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00,
    0x68, 0x00, 0x00, 0x00
};

#else // QT_NO_DEBUG
static const unsigned char qt_pluginMetaData_BtMiniStyle[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    0x71, 0x62, 0x6a, 0x73, 0x01, 0x00, 0x00, 0x00,
    0xec, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0xd8, 0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 0x49, 0x49, 0x44, 0x00, 0x00, 0x00,
    0x28, 0x00, 0x6f, 0x72, 0x67, 0x2e, 0x71, 0x74,
    0x2d, 0x70, 0x72, 0x6f, 0x6a, 0x65, 0x63, 0x74,
    0x2e, 0x51, 0x74, 0x2e, 0x51, 0x53, 0x74, 0x79,
    0x6c, 0x65, 0x46, 0x61, 0x63, 0x74, 0x6f, 0x72,
    0x79, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x66, 0x61,
    0x63, 0x65, 0x00, 0x00, 0x95, 0x0a, 0x00, 0x00,
    0x08, 0x00, 0x4d, 0x65, 0x74, 0x61, 0x44, 0x61,
    0x74, 0x61, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x14, 0x03, 0x00, 0x00, 0x04, 0x00, 0x4b, 0x65,
    0x79, 0x73, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x6d, 0x69, 0x6e, 0x69, 0x00, 0x00,
    0x0a, 0x00, 0x6d, 0x69, 0x6e, 0x69, 0x2d, 0x6e,
    0x69, 0x67, 0x68, 0x74, 0x8b, 0x01, 0x00, 0x00,
    0x8b, 0x02, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x1b, 0x15, 0x00, 0x00, 0x09, 0x00, 0x63, 0x6c,
    0x61, 0x73, 0x73, 0x4e, 0x61, 0x6d, 0x65, 0x00,
    0x11, 0x00, 0x42, 0x74, 0x4d, 0x69, 0x6e, 0x69,
    0x53, 0x74, 0x79, 0x6c, 0x65, 0x50, 0x6c, 0x75,
    0x67, 0x69, 0x6e, 0x00, 0x31, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x64, 0x65, 0x62, 0x75, 0x67, 0x00,
    0x5a, 0x00, 0xa0, 0x00, 0x07, 0x00, 0x76, 0x65,
    0x72, 0x73, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
    0x98, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00,
    0xc8, 0x00, 0x00, 0x00
};
#endif // QT_NO_DEBUG

static QT_PREPEND_NAMESPACE(QObject) *qt_plugin_instance_BtMiniStylePlugin() Q_PLUGIN_INSTANCE(BtMiniStylePlugin)

static const char *qt_plugin_query_metadata_BtMiniStylePlugin()
{
    return (const char *)qt_pluginMetaData_BtMiniStyle;
}

const QStaticPlugin qt_static_plugin_BtMiniStylePlugin()
{
    QStaticPlugin plugin = { qt_plugin_instance_BtMiniStylePlugin, qt_plugin_query_metadata_BtMiniStylePlugin};
    return plugin;
}
#endif

Q_IMPORT_PLUGIN(BtMiniStylePlugin)
