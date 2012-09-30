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
#include <QCommonStyle>
#include <QLayout>
#include <QStyleOption>
#include <QStylePlugin>
#include <QPainter>
#include <QPushButton>
#include <QtDebug>

class BtMiniStyle : public QCommonStyle
{
    Q_OBJECT

public:
    BtMiniStyle()
    {
        Q_INIT_RESOURCE(btministyle);
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
				QRect r(opt->rect.adjusted(0, 0, 0, opt->fontMetrics.height() - opt->rect.height()));

				QLinearGradient g(r.topLeft(), r.bottomLeft());

				g.setColorAt(0.0, QColor(0, 0, 0, 100));
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
				int s = opt->rect.width() / 3;
				p->setBrush(Qt::white);
				p->drawRect(opt->rect.adjusted(s, s / 2, -s, -s / 2));
				p->drawEllipse(opt->rect.adjusted(s, 0, -s, s - opt->rect.height()));
				p->drawEllipse(opt->rect.adjusted(s, opt->rect.height() - s, -s, 0));
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
                    p->setBrush(QColor(127, 196, 255));
                    p->drawRect(scrollbar->rect);

                    if(scrollbar->subControls & SC_ScrollBarSlider)
                    {
                        QStyleOptionSlider to = *scrollbar;

                        to.rect = subControlRect(cc, &to, SC_ScrollBarSlider, widget);
                        if(to.rect.isValid())
                        {
                            if(!(scrollbar->activeSubControls & SC_ScrollBarSlider))
                                to.state &= ~(State_Sunken | State_MouseOver);

                            drawControl(CE_ScrollBarSlider, &to, p, widget);
                        }
					}

					QCommonStyle::drawComplexControl(cc, option, p, widget);

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
		static QPalette defaultPalette(Qt::white);

	    defaultPalette.setColor(QPalette::Window, Qt::white);
		defaultPalette.setColor(QPalette::Button, Qt::white);
	    defaultPalette.setColor(QPalette::WindowText, QColor("#000000"));
	    defaultPalette.setColor(QPalette::ButtonText, QColor("#000000"));
	    defaultPalette.setColor(QPalette::Highlight, QColor("#9cd2ff"));
	    defaultPalette.setColor(QPalette::HighlightedText, QColor("#000000"));

	    return defaultPalette;
	}


	void polish(QWidget *widget)
    {
        static bool initialized = false;

        if(!initialized)
        {
            _menuFrame = new QPixmap(":/style-mini/menu-frame.png");
            _menuFrameWidth = _menuFrame->width()/3;

            initialized = true;
        }

        if(QString(widget->metaObject()->className()) == "BtMiniMenu")
        {
			QMargins m = widget->contentsMargins();
			widget->setContentsMargins(m.left()+_menuFrameWidth, m.top()+_menuFrameWidth,
				m.right()+_menuFrameWidth, m.bottom()+_menuFrameWidth);
            widget->setAttribute(Qt::WA_TranslucentBackground);
        }

        if(QString(widget->metaObject()->className()) == "QWidget")
		{
			widget->setAutoFillBackground(true);
			foreach(QWidget *w, widget->findChildren<QWidget*>() << widget)
				w->setPalette(standardPalette());
		}

        // bottom panel widget
        if(QString(widget->metaObject()->className()) == "BtMiniPanel")
		{
			widget->setAutoFillBackground(true);

            QPalette p = widget->palette();
            p.setColor(QPalette::Window, QColor(102, 102, 102));
            p.setColor(QPalette::Button, QColor(102, 102, 102));
            p.setColor(QPalette::WindowText, Qt::white);
            p.setColor(QPalette::ButtonText, Qt::white);

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


public slots:
    QIcon standardIconImplementation(StandardPixmap icon, const QStyleOption *option = 0, const QWidget *widget = 0) const
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
};

class BtMiniStylePlugin : public QStylePlugin
{
public:
    BtMiniStylePlugin() {;}
    ~BtMiniStylePlugin() {;}

    QStyle * create(const QString &key)
    {
        return new BtMiniStyle();
    }

    QStringList	keys() const
    {
        return QStringList("mini");
    }
};

#include "btministyle.moc"

// HACK declare static plugin
QObject * qt_plugin_instance_BtMiniStylePlugin() Q_PLUGIN_INSTANCE(BtMiniStylePlugin)

Q_IMPORT_PLUGIN(BtMiniStylePlugin)
