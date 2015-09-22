/*************************************************************************
* style.cpp - modern finger oriented GUI style
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

#include <QApplication>
#include <QCommonStyle>
#include <QLayout>
#include <QStyleOption>
#include <QStylePlugin>
#include <QPainter>
#include <QPushButton>
#include <QtDebug>

class Style : public QCommonStyle
{
    Q_OBJECT

public:
    Style()
    {
        Q_INIT_RESOURCE(style);

        QPalette defaultPalette(QCommonStyle::standardPalette());

        defaultPalette.setColor(QPalette::Window, Qt::white);
        defaultPalette.setColor(QPalette::Button, Qt::white);
        defaultPalette.setColor(QPalette::WindowText, QColor("#333333"));
        defaultPalette.setColor(QPalette::ButtonText, QColor("#333333"));
        defaultPalette.setColor(QPalette::Highlight, QColor("#9cd2ff"));
        defaultPalette.setColor(QPalette::HighlightedText, QColor("#333333"));

        QApplication::setPalette(defaultPalette);
    }
    ~Style()
    {
        if(_menuFrame)
            delete _menuFrame;
    }

    void drawPrimitive(PrimitiveElement element, const QStyleOption *opt, QPainter *p, const QWidget *widget = 0) const
    {
        switch(element)
        {
        case PE_Frame:
            break;
        case PE_FrameMenu:
            if(const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame*>(opt))
            {
                const int m = _menuFrameWidth;
                qDrawBorderPixmap(p, frame->rect, QMargins(m, m, m, m), *_menuFrame);
            }
            return;
        case PE_PanelButtonCommand:
            return;
        case PE_PanelMenu:
            break;
        }
        QCommonStyle::drawPrimitive(element, opt, p, widget);
    }

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *widget = 0) const
    {
        switch(element)
        {
        case CE_PushButtonBevel:
            break;
        case CE_PushButton:
            break;
        default:
            ;
        }
        QCommonStyle::drawControl(element, opt, p, widget);
    }

    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget = 0 ) const
    {
        QCommonStyle::drawComplexControl(cc, opt, p, widget);
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
        }
        return QCommonStyle::pixelMetric(metric, option, widget);
    }

    void drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole) const
    {
        QCommonStyle::drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole);
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

        if(QString(widget->metaObject()->className()) == "Menu")
        {
            widget->setContentsMargins(_menuFrameWidth, _menuFrameWidth, _menuFrameWidth, _menuFrameWidth);
            widget->setAttribute(Qt::WA_TranslucentBackground);
        }

        // bottom panel widget
        if(QString(widget->metaObject()->className()) == "Panel" && \
            widget->parentWidget()->layout()->indexOf(widget) > 0)
        {
            QPalette p = widget->palette();

            p.setColor(QPalette::Window, QColor(102, 102, 102));
            p.setColor(QPalette::Button, QColor(102, 102, 102));
            p.setColor(QPalette::WindowText, Qt::white);
            p.setColor(QPalette::ButtonText, Qt::white);

            foreach(QWidget *w, widget->findChildren<QWidget*>() << widget)
                w->setPalette(p);
        }
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
    Q_DISABLE_COPY(Style)

    QPixmap *_menuFrame;
    int      _menuFrameWidth;
};

class StylePlugin : public QStylePlugin
{
public:
    StylePlugin() {;}
    ~StylePlugin() {;}

    QStyle * create(const QString &key)
    {
        return new Style();
    }

    QStringList	keys() const
    {
        return QStringList("mini");
    }
};

#include "style.moc"

// HACK declare static plugin
QObject * qt_plugin_instance_StylePlugin() Q_PLUGIN_INSTANCE(StylePlugin)

Q_IMPORT_PLUGIN(StylePlugin)
