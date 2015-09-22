/*************************************************************************
* itemview.cpp - model/view arhitecture for BibleTime Mini
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

#include <QElapsedTimer>
#include <QMouseEvent>
#include <QScrollBar>

#include "itemview.h"

class ScrollBar : public QScrollBar
{
public:
    ScrollBar()
    {
        ;
    }

    ~ScrollBar()
    {
        ;
    }

};

class ItemViewPrivate
{
public:
    ItemViewPrivate()
    {
        _mousePower     = 0.0f;
        _mouseDamping   = 0.95f;
        _timer.start();
    }

    ~ItemViewPrivate();

    QPoint                     _mouseLast;
    QPoint                     _mouseStart;
    bool                       _mouseLeaveZone;
    bool                       _mouseDown;
    int                        _mouseTapping;
    QPair<QPoint, qint64>      _mouseScrolling[2];
    float                      _mouseDamping;
    float                      _mousePower;

    QElapsedTimer              _timer;
};

ItemView::ItemView(QWidget *parent) : QListView(parent), d_ptr(new ItemViewPrivate)
{
    setWordWrap(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFlow(QListView::TopToBottom);

    startTimer(1000/30);
}

ItemView::~ItemView()
{
    ;
}

void ItemView::mousePressEvent(QMouseEvent *e)
{
    Q_D(ItemView);

    if(e->button() == Qt::LeftButton)
    {
        d->_mouseLast = d->_mouseStart = e->pos();

        d->_mouseLeaveZone = false;
        d->_mouseDown = true;

        d->_mousePower               = 0.0f;
        d->_mouseTapping             = 0;
        d->_mouseScrolling[0].first  = e->pos();
        d->_mouseScrolling[0].second = d->_timer.elapsed();
    }

    QListView::mousePressEvent(e);
}

void ItemView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(ItemView);

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

    d->_mousePower = (speed*50.0f) + (d->_mousePower*qMin(qAbs(speed)*200.0f/(QApplication::font().pixelSize()), (qreal)1.0));

    d->_mouseDown = false;
    d->_mouseTapping = 0;

    QListView::mouseReleaseEvent(e);

    update();
}

void ItemView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(ItemView);

    if(d->_mouseDown)
    {
        const int max = QApplication::font().pixelSize();

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

        scroll(static_cast<float>(delta.y()));

        d->_mouseLast = e->pos();
    }

    QListView::mouseMoveEvent(e);
}


void ItemView::timerEvent(QTimerEvent *e)
{
    Q_D(ItemView);

    if(!d->_mouseDown && qAbs(d->_mousePower) > 0.1f)
    {
        d->_mousePower *= d->_mouseDamping;
        scroll(d->_mousePower);
    }

    if(d->_mouseDown && d->_mouseTapping < 12 && !d->_mouseLeaveZone)
        d->_mouseTapping++;

    QListView::timerEvent(e);
}

void ItemView::scroll(float amount)
{
    verticalScrollBar()->setValue(verticalScrollBar()->value()-amount);
}
