﻿/*********
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

#include "btminimenu.h"
#include "btminipanel.h"
#include "btmini.h"

class BtMiniPanelPrivate
{
public:
    BtMiniPanelPrivate()
    {
        ;
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

private:
    QVector<BtMiniPanel::Activity>  _activities;
    QVector<QWidget*>               _widgets;

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
        QString bt;

        switch(a)
        {
		case Close:
                bt = "Close";
            break;
        case Exit:
                bt = "Exit";
            break;
        case Installer:
                bt = "Install";
            break;
        case Search:
                bt = "Find";
            break;
        case Settings:
                bt = "Settings";
            break;
        default:
            Q_ASSERT(false);
        }

        QString br(tr(bt.toLatin1()));

        QPushButton *b = new QPushButton(br, this);
        b->setObjectName(bt);

        connect(b, SIGNAL(clicked()), this, SLOT(controlActivated()));

        d->addActivity(a, b);
        l->addWidget(b);
        l->setStretchFactor(b, br.size() + 3);
    }

    setLayout(l);
}

BtMiniPanel::~BtMiniPanel()
{
    Q_D(BtMiniPanel);

    delete d;
}

QSize BtMiniPanel::sizeHint() const
{
    return QWidget::sizeHint().boundedTo(parentWidget()->size());
}

QSize BtMiniPanel::minimumSizeHint() const
{
    return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
}

void BtMiniPanel::paintEvent(QPaintEvent *event)
{
    ;
}

void BtMiniPanel::controlActivated()
{
    Q_D(BtMiniPanel);

    switch(d->getActivity(sender()))
    {
    case Exit:
        if(BtMiniMenu::execQuery(tr("Exit?"), QStringList() << tr("Yes") << tr("No")) == 0)
            BtMini::mainWidget()->close();
        break;
    case Search:
        BtMini::setActiveWidget(BtMini::searchWidget());
        break;
    case Close:
        BtMini::setActiveWidget(BtMini::worksWidget());
        break;
    case Installer:
        BtMini::setActiveWidget(BtMini::installerWidget());
        break;
    case Settings:
        BtMini::setActiveWidget(BtMini::settingsWidget());
        break;
    default:
        Q_ASSERT(false);
    }
}