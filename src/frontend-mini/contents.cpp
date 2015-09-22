/*************************************************************************
* contents.cpp - contents widget
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
#include <QElapsedTimer>
#include <QLineEdit>
#include <QListView>
#include <QMap>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QStack>
#include <QtCore/qmath.h>


#include "contents.h"
#include "core.h" // remove if possible
#include "state.h"


// vibration
#ifdef Q_OS_WINCE
#include  <windows.h>
#include  <nled.h>

// from the platform builder <Pwinuser.h>
extern "C"
{
	BOOL WINAPI NLedGetDeviceInfo(UINT nInfoId, void *pOutput);
	BOOL WINAPI NLedSetDevice(UINT nDeviceId, void *pInput);
};

void LedOn(int id)
{
	NLED_SETTINGS_INFO settings;
	settings.LedNum = id;
	settings.OffOnBlink = 1;
	NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);
}
void LedOff(int id)
{
	NLED_SETTINGS_INFO settings;
	settings.LedNum = id;
	settings.OffOnBlink = 0;
	NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);
}
#endif

class ViewPrivate
{
public:
    ViewPrivate()
    {
        _stretchPixmap  = 0;
        _currentView    = View::TYPE_HOME;
        _mousePower     = 0.0f;
        _viewShift      = 0;

		_showFps        = QString((*Core_->getOptions())["Ui"].getWithDefault("showFps", "false")) == "true";
		_mouseDamping   = QString((*Core_->getOptions())["Ui"].getWithDefault("CineticDamping", "0.95")).toFloat();
		
		_stretchSurface    = QString((*Core_->getOptions())["Ui"].getWithDefault("stretchSurface", "false")) == "true";
		_stretchVertical   = QString((*Core_->getOptions())["Ui"].getWithDefault("stretchVertical", "1.0")).toFloat();
		_stretchHorizontal = QString((*Core_->getOptions())["Ui"].getWithDefault("stretchHorizontal", "1.0")).toFloat();

		_refreshRate       = 1000/QString((*Core_->getOptions())["Ui"].getWithDefault("RefreshRate", "30")).toInt();
		_timer.start();
    }

    ~ViewPrivate()
    {
        if(_stretchPixmap)
            delete _stretchPixmap;

        foreach(sbList *list, _views.values())
        {
            while (list->getPrev() != NULL)
                delete list->getPrev();
            while (list->getNext() != NULL)
                delete list->getNext();
            delete list;
        }
    }

    bool                       _stretchSurface;
    QPixmap                   *_stretchPixmap;
    float                      _stretchVertical;
    float                      _stretchHorizontal;

    QRect                      _rect;

    View::Type                 _currentView;
    QMap<View::Type, sbList*>  _views;
	QStack<View::Type>         _viewStack;

    int                        _viewShift;

    bool                       _showFps;

    QPoint                     _mouseLast;
    QPoint                     _mouseStart;
    bool                       _mouseLeaveZone;
    bool                       _mouseDown;
    int                        _mouseTapping;
    QPair<QPoint, qint64>      _mouseScrolling[2];
    float                      _mouseDamping;
	float                      _mousePower;

	int                        _refreshRate;
	QElapsedTimer              _timer;
};

ContentsWidget::ContentsWidget(QWidget *parent) : QWidget(parent), d_ptr(new ViewPrivate)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	startTimer(d_ptr->_refreshRate);
	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);
    setFocusPolicy(Qt::ClickFocus);
}

ContentsWidget::~ContentsWidget()
{
    ;
}

void ContentsWidget::paintEvent(QPaintEvent *e)
{
    Q_D(View);

    QElapsedTimer timerDraw;
    static QElapsedTimer timerCycle;
    timerDraw.start();

    QPainter painter;

    if(d->_stretchSurface)
        painter.begin(d->_stretchPixmap);
    else
        painter.begin(this);

    painter.setPen(QPen(Qt::NoPen));

    //painter.scale(0.33, 0.33);
    //painter.translate(100, 133);

    //painter.setBrush(Qt::white);
    //painter.drawRect(getRect());

    if(d->_views[d->_currentView] != NULL)
    {
        if(d->_viewShift == 0)
            d->_views[d->_currentView]->render(&painter);
        else
        {
            d->_views[d->_currentView]->render(&painter, -d->_viewShift);

            if(d->_viewShift > 0)
                d->_views[d->_currentView]->getNext()->render(&painter, -d->_viewShift + rect().width());
            else
                d->_views[d->_currentView]->getPrev()->render(&painter, -d->_viewShift - rect().width());
        }

        if(d->_views[d->_currentView]->needScroll != 0)
        {
            if(d->_views[d->_currentView]->needScroll < 0)
            {
                d->_mousePower--;

                if(d->_mousePower < 0.11f && d->_mousePower > -0.11f) //fix timer stopping
                    d->_mousePower = -0.11f;

                d->_mousePower = qMax(d->_mousePower, d->_views[d->_currentView]->needScroll*0.11f);
            }
            else
            {
                d->_mousePower++;

                if(d->_mousePower < 0.11f && d->_mousePower > -0.11f) //fix timer stopping
                    d->_mousePower = 0.11f;

                d->_mousePower = qMin(d->_mousePower, d->_views[d->_currentView]->needScroll*0.11f);
            }
        }
    }

    if(d->_showFps)
    {
        const qint64 dc = qMax((qint64)1, timerCycle.elapsed());
        timerCycle.restart();

        const qint64 delta = timerDraw.elapsed();
        static qint64 lastDelta = delta;

        lastDelta = qMax(lastDelta*95/100, qMax(delta, (qint64)1));

        painter.setPen(Qt::red);
        QFont f = QApplication::font();
        painter.setFont(f);
        painter.drawText(5, 5 + f.pixelSize(), QString("fps: %1(%2)").arg(1000/lastDelta).arg(1000/dc));
    }

    if(d->_stretchSurface)
    {
        painter.end();
        painter.begin(this);
        painter.drawPixmap(rect(), *d->_stretchPixmap, QRect(QPoint(), d->_stretchPixmap->size()));
    }
}

bool ContentsWidget::event(QEvent *e)
{
	Q_D(View);

    switch(e->type())
    {
    case QEvent::Show:
        if(d->_stretchSurface)
		{
			d->_rect = rect();
			d->_rect.setHeight(d->_rect.height() * d->_stretchVertical);
			d->_rect.setWidth(d->_rect.width() * d->_stretchHorizontal);

			d->_stretchPixmap = new QPixmap(d->_rect.size());
			if(!d->_stretchPixmap)
				d->_stretchSurface = false;
		}
		if(!d->_stretchSurface)
			d->_rect = rect();
        break;
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        if(d_ptr->_stretchSurface)
        {
            const QMouseEvent *me = reinterpret_cast<const QMouseEvent*>(e);
            QPoint p = me->pos();
            p.setX(p.x() * d_ptr->_rect.width() / rect().width());
            p.setY(p.y() * d_ptr->_rect.height() / rect().height());
            QMouseEvent modified(me->type(), p, me->button(), me->buttons(), me->modifiers());
            return QWidget::event(&modified);
        }
        break;
    }
    return QWidget::event(e);
}

void ContentsWidget::mousePressEvent(QMouseEvent *e)
{
    Q_D(View);

    if(e->button() == Qt::LeftButton)
    {
        if(d->_viewShift != 0) return;

        d->_mouseLast = d_ptr->_mouseStart = e->pos();
        
        d->_mouseLeaveZone = false;
        d->_mouseDown = true;

        if(d->_views[d->_currentView] != NULL)
            d->_views[d->_currentView]->onPressed(e->x(), e->y());

        d->_mousePower                = 0.0f;
        d->_mouseTapping              = 0;
        d->_mouseScrolling[0].first  = e->pos();
        d->_mouseScrolling[0].second = d->_timer.elapsed();
    }
}

void ContentsWidget::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(View);

    if(e->button() != Qt::LeftButton || !d->_mouseDown)
        return;

    d->_mouseScrolling[1].second = d->_timer.elapsed();
    d->_mouseScrolling[1].first  = e->pos();

    if(d->_mouseScrolling[1].second-d->_mouseScrolling[0].second > 200)
    {
        d->_mouseScrolling[0].first  = d->_mouseScrolling[1].first+((d->_mouseScrolling[0].first-\
            d->_mouseScrolling[1].first) * (200.0f/(d->_mouseScrolling[1].second-d->_mouseScrolling[0].second)));
        d->_mouseScrolling[0].second = d->_mouseScrolling[1].second - 200;
    }

    qreal speed = (d->_mouseScrolling[1].first.y()-d->_mouseScrolling[0].first.y()) / \
        (qreal)qMax((d->_mouseScrolling[1].second-d->_mouseScrolling[0].second), (qint64)120);

    d->_mousePower = (speed*50.0f) + (d->_mousePower*qMin(qAbs(speed)*200.0f/(logicalDpiX()/7), (qreal)1.0));

    //sbMessage ("Ticks %i\t Y %i\t cineticPower %f\n", input.scrollDataTicks[0], input.scrollDataY[0], cineticPower);
    //sbMessage ("Ticks %i\t Y %i\t Duration %i\t Speed %f\n", input.scrollDataTicks[1], input.scrollDataY[1],input.scrollDataTicks[1]-input.scrollDataTicks[0],speed);

    if(d->_viewShift != 0)
        d->_mousePower = 0.0f;

    d->_mouseDown = false;

    if(d->_views[d->_currentView] != NULL)
    {
        d->_views[d->_currentView]->onReleased(e->x(), e->y());

        if(!d->_mouseLeaveZone)
            if(d->_mouseTapping >= 12)
            {
                d->_mouseTapping = 0;
                d->_views[d->_currentView]->onTapped(e->x(), e->y());
            }
            else
            {
                d->_mouseTapping = 0;
                d->_views[d->_currentView]->onClicked(e->x(), e->y());
            }
    }
    d->_mouseTapping = 0;
    update();
}

void ContentsWidget::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(View);
    
    if(!d->_mouseDown)
        return;

    const int max = logicalDpiX()/7;

    QPoint delta = e->pos() - d->_mouseLast;
    
    if(qAbs(d->_mouseStart.x()-e->x()) > max || qAbs(d->_mouseStart.y()-e->y()) > max)
    {
        d->_mouseLeaveZone = true;
        d->_mouseTapping = 0;
    }

    d->_mouseScrolling[1].second = d->_timer.elapsed();
    d->_mouseScrolling[1].first  = e->pos();

    if(d->_mouseScrolling[1].second-d->_mouseScrolling[0].second > 200)
    {
        d->_mouseScrolling[0].first  = d->_mouseScrolling[1].first+((d->_mouseScrolling[0].first-\
            d->_mouseScrolling[1].first) * (200.0f/(d->_mouseScrolling[1].second-d->_mouseScrolling[0].second)));
        d->_mouseScrolling[0].second = d->_mouseScrolling[1].second - 200;
    }

    if(d->_views[d->_currentView] != NULL)
    {
        if(qAbs(d->_mouseStart.x()-e->x()) > (rect().right()/6)+(qAbs(d->_mouseStart.y()-e->y())/2))
            d->_viewShift = d->_mouseStart.x()-e->x();
        else
            d->_viewShift = 0;

        if(d->_viewShift < 0 && d->_views[d->_currentView]->getPrev() == NULL)
            d->_viewShift = 0;
        if(d->_viewShift > 0 && d->_views[d->_currentView]->getNext() == NULL)
            d->_viewShift = 0;

        d->_views[d->_currentView]->scroll(static_cast<float>(delta.y()));
        update();
    }
    d->_mouseLast = e->pos();
}


void ContentsWidget::timerEvent(QTimerEvent *e)
{
    Q_D(View);
    sbList * const cv = d->_views[d->_currentView];

    if(cv)
        cv->onUpdate();

    if(!d->_mouseDown && qAbs(d->_mousePower) > 0.1f)
    {
        d->_mousePower *= d->_mouseDamping;

        if(cv && (cv->scroll(d->_mousePower) || cv->needFill))
            update();
    }

    if(d->_mouseDown && d->_mouseTapping < 12 && !d->_mouseLeaveZone)
    {
        if(d->_mouseTapping == 11)
        {
#ifdef Q_OS_WINCE
			LedOn(1); 
			Sleep(30);
			LedOff(1);
#endif
        }
        d->_mouseTapping++;
    }

    if(d->_viewShift != 0 && !d->_mouseDown)
    {
        if(qAbs(d->_viewShift) >= rect().width())
        {
            switchView(d->_viewShift > 0 ? View::PIN_NEXT : View::PIN_PREV);
            d->_viewShift = 0;
        }
        else
        {
            if(d->_viewShift > 0)
                d->_viewShift += (int)qCeil((rect().width() - d->_viewShift)*0.44f);
            else
                d->_viewShift -= (int)qCeil((rect().width() + d->_viewShift)*0.44f);

            if(d->_viewShift > rect().width())
                d->_viewShift = rect().width();

            if(d->_viewShift < -rect().width())
                d->_viewShift = -rect().width();
            update();
        }
    }
}

void ContentsWidget::switchView(QVariant typeVariant)
{
	Q_D(View);

	sbList *oldList = d->_views[d->_currentView];
	View::Type type = static_cast<View::Type>(typeVariant.toInt());

	//qDebug("Switch view to : %i", type);

	switch (type)
	{
	case View::PIN_NEXT:
		if(d->_views[d->_currentView]->getNext() != NULL)
			d->_views[d->_currentView] = d->_views[d->_currentView]->getNext();
		type = d->_currentView;
		break;
	case View::PIN_PREV:
		if(d->_views[d->_currentView]->getPrev() != NULL)
			d->_views[d->_currentView] = d->_views[d->_currentView]->getPrev();
		type = d->_currentView;
		break;
	case View::TYPE_CURRENT:
		switchView(d->_currentView);
		return;
	case View::TYPE_NONE:
		return;
	case View::TYPE_BACK:
		if(!d->_viewStack.isEmpty())
		{
			d->_viewStack.pop();
			if(!d->_viewStack.isEmpty())
				switchView(d->_viewStack.top());
		}
		return;
	default:
		if(d->_views[type] == NULL)
			d->_views[type] = new sbList(type);
	}

	d->_currentView = type;

	Core_->threadDestroy();

	// call update before sbList::onActivate for correct painting of Menu
	update();

	if(oldList != d->_views[d->_currentView])
	{
		d->_views[d->_currentView]->onActivate();

		View::Type pushType = type;

		if(type == View::TYPE_SELECT_MODULE || type == View::TYPE_SEARCH)
			pushType = View::TYPE_SELECT_BOOK;

		if(type == View::TYPE_MODULE_INSTALLER_DISCLAIMER)
			pushType = View::TYPE_MODULE_INSTALLER;

		if(d->_viewStack.isEmpty() || d->_viewStack.top() != pushType)
			d->_viewStack.push(pushType);
	}

	if(d->_currentView == View::TYPE_MODULE_VIEW && d->_views[d->_currentView]->_module != NULL)
		Core_->threadCreate(d->_views[d->_currentView]);

    d->_viewShift  = 0;
    d->_mousePower = 0.0f;
}

QSize ContentsWidget::getSize() const
{
	//return d_ptr->_rect.size();
    return size();
}

void ContentsWidget::performSearch()
{
	Q_D(View);
	QLineEdit *le = qobject_cast<QLineEdit*>(sender());
	if(le)
		d->_views[d->_currentView]->performSearch(le->text());
}

void ContentsWidget::resizeEvent(QResizeEvent *e)
{
    ;
}