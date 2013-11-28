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

#include <QBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <QStyleOption>
#include <QtDebug>

#include "btminimenu.h"
#include "btminipanel.h"
#include "btminiui.h"

class BtMiniPanelPrivate
{
public:
    BtMiniPanelPrivate()
    {
        _anchorBased = false;
    }

    /** */
    void addActivity(BtMiniPanel::Activity activity, QWidget *widget)
    {
        Q_ASSERT(!_activities.contains(activity));

        _activities << activity;
        _widgets << widget;
    }

    /** */
    BtMiniPanel::Activity getActivity(QObject *widget)
    {
        Q_CHECK_PTR(widget);

        for(int i = 0; i < _widgets.size(); ++i)
            if(_widgets[i] == widget)
                return _activities[i];

        Q_ASSERT(false);
        return BtMiniPanel::None;
    }


public:
    QVector<BtMiniPanel::Activity>  _activities;
    QVector<QWidget*>               _widgets;
    bool                            _anchorBased;
    QVector<Qt::Alignment>          _anchorPoints;


};

BtMiniPanel::BtMiniPanel(Activities activities, QWidget *parent)
    : d_ptr(new BtMiniPanelPrivate), QWidget(parent)
{
    Q_D(BtMiniPanel);

	QFont f(font());
	f.setPixelSize(f.pixelSize() * 0.9);
	f.setWeight(QFont::Bold);
	setFont(f);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

    QHBoxLayout *l = new QHBoxLayout;

    foreach(Activity a, activities)
    {
#if defined Q_OS_ANDROID || defined Q_OS_IOS || defined Q_OS_LINUX || defined Q_OS_WIN32
        if(a == Exit)
            continue;
#endif

        QString bt;
		QString bo;

        switch(a)
        {
		case Close:
                bo = "Close";
				bt = tr("Close");
            break;
        case Exit:
                bo = "Exit";
				bt = tr("Exit");
            break;
        case Installer:
                bo = "Install";
				bt = tr("Install");
            break;
        case Search:
                bo = "Find";
				bt = tr("Find");
            break;
        case Settings:
                bo = "Settings";
				bt = tr("Settings");
            break;
		case Refresh:
				bo = "Refresh";
                bt = tr("Update Sources");
            break;
        default:
            Q_ASSERT(false);
        }

        QPushButton *b = new QPushButton(bt, this);
        b->setObjectName(bo);

        connect(b, SIGNAL(clicked()), this, SLOT(controlActivated()));

        d->addActivity(a, b);
        l->addWidget(b);
        l->setStretchFactor(b, bt.size() + 3);
    }

    setLayout(l);
}

BtMiniPanel::BtMiniPanel(QWidget *parent)
    : d_ptr(new BtMiniPanelPrivate), QWidget(parent)
{
    Q_D(BtMiniPanel);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    d->_anchorBased = true;
}

BtMiniPanel::~BtMiniPanel()
{
    Q_D(BtMiniPanel);

    delete d;
}

void BtMiniPanel::addWidget(QWidget *widget, Qt::Alignment point)
{
    Q_D(BtMiniPanel);

    Q_ASSERT(d->_anchorBased);

    widget->setParent(this);
    d->_widgets.append(widget);
    d->_anchorPoints.append(point);
    //connect(widget, SIGNAL(resized()))
}

QSize BtMiniPanel::sizeHint() const
{
    return minimumSizeHint();
    //return QWidget::sizeHint().boundedTo(parentWidget()->size());
}

QSize BtMiniPanel::minimumSizeHint() const
{
    Q_D(const BtMiniPanel);

    // -100 required for android, where main window was enlarged each time panel was opened
    return QSize(parentWidget()->size().width() - 100, parentWidget()->font().pixelSize() * 2.2);
    //return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
}

void BtMiniPanel::paintEvent(QPaintEvent *event)
{
    ;
}

void BtMiniPanel::resizeEvent(QResizeEvent *e)
{
    Q_D(BtMiniPanel);

    if(d->_anchorBased)
    {
        int ls = 0, rs = 0, cs = 0, cw = 0;
        //int spacing = 20; //style()->QStyle::combinedLayoutSpacing(QSizePolicy::DefaultType,
                          //                           QSizePolicy::DefaultType, Qt::Horizontal);
        QStyleOption so;
        so.initFrom(this);
        int spacing = style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing, &so, this);

        for(int i = 0; i < d->_widgets.size(); ++i)
        {
            QRect r(QPoint(), d->_widgets[i]->sizeHint());

            if(d->_anchorPoints[i] & Qt::AlignRight)
                r.moveLeft(e->size().width() - r.width()), rs += r.width() + spacing;
            else if(d->_anchorPoints[i] & Qt::AlignHCenter
                    || !(d->_anchorPoints[i] & Qt::AlignLeft))
                r.moveLeft((e->size().width() - r.width()) / 2), cs += r.width(), ++cw;
            else
                ls += r.width() + spacing;

            if(d->_anchorPoints[i] & Qt::AlignBottom)
                r.moveTop(e->size().height() - r.width());
            else if(d->_anchorPoints[i] & Qt::AlignVCenter
                    || !(d->_anchorPoints[i] & Qt::AlignTop))
                r.moveTop((e->size().height() - r.height()) / 2);

            if(d->_widgets[i]->sizePolicy().expandingDirections() & Qt::Vertical)
                r.setTop(0), r.setBottom(height() - 1);

            d->_widgets[i]->setGeometry(r);
        }

        // again, set central widgets
        for(int i = 0, pos = ls; i < d->_widgets.size(); ++i)
        {
            if(d->_anchorPoints[i] & Qt::AlignHCenter)
            {
                QRect r(d->_widgets[i]->geometry());

                if(cw == 1)
                {
                    if(d->_widgets[i]->sizePolicy().expandingDirections() & Qt::Horizontal)
                    {
                        r.setLeft(ls);
                        r.setRight(e->size().width() - rs - 1);
                    }
                    else
                    {
                        r.setLeft(qMax(ls , rs));
                        r.setRight(e->size().width() - qMax(ls , rs));

                        int mw = d->_widgets[i]->sizeHint().width();
                        if(r.width() < mw)
                        {
                            // TODO expand widget if its cut but there is space
                        }
                    }
                }
                else
                {
                    int nw = (e->size().width() - ls - rs) / (qreal)cs * r.width();
                    r.setLeft(pos);
                    r.setWidth(nw);
                    pos += nw;
                }

                d->_widgets[i]->setGeometry(r);
            }
        }
    }
}

void BtMiniPanel::controlActivated()
{
    Q_D(BtMiniPanel);

    switch(d->getActivity(sender()))
    {
    case Exit:
        if(BtMiniMenu::execQuery(tr("Exit?"), QStringList() << tr("Yes") << tr("No")) == 0)
            BtMiniUi::instance()->mainWidget()->close();
        break;
    case Search:
        BtMiniUi::instance()->activateSearch();
        break;
    case Close:
        BtMiniUi::instance()->activateWorks();
        break;
    case Installer:
        BtMiniUi::instance()->activateInstaller();
        break;
    case Settings:
        BtMiniUi::instance()->activateSettings();
        break;
    case Refresh:
        //BtMini::updateRemoteSources();
        break;
    default:
        Q_ASSERT(false);
    }
}
