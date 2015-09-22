/*************************************************************************
 * menu.cpp - common used menu
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
#include <QBoxLayout>
#include <QEventLoop>
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStyleOptionFrame>
#include <QStyleOptionMenuItem>
#include <QtDebug>

#include "menu.h"

class MenuPrivate
{
public:
    MenuPrivate()
    {
        _result = -1;
		_eventLoop = 0;
        _modal = false;
		_canceled = false;
    }
    
    ~MenuPrivate()
    {
        ;
    }
    
    static QWidget * mainWidget()
    {
        QWidget *m(0);
        foreach(QWidget *w, qApp->topLevelWidgets())
        {
            if(!m)
                m = w;
            else if(m->width() < w->width() && m->height() < w->height())
                m = w;
        }
        Q_ASSERT(m);
        return m;
    }
    
    QList<QWidget*>  _buttons;
    int              _result;
	QEventLoop      *_eventLoop;
    bool             _modal;
	bool             _canceled;

};

Menu::Menu() : d_ptr(new MenuPrivate), QWidget(MenuPrivate::mainWidget(), Qt::FramelessWindowHint)
{
	//setWindowModality(Qt::ApplicationModal);
    //setModal(false);
    //setAutoFillBackground(false);
	//setAttribute(Qt::WA_MouseNoMask);
	//setAttribute(Qt::WA_NoMousePropagation);

	if(parentWidget())
		setMaximumSize(parentWidget()->size());

    QFont f = font();
    f.setBold(true);
    f.setPixelSize(f.pixelSize() * 1.6);
    setFont(f);

	hide();
}

Menu::~Menu()
{
	if(d_ptr->_eventLoop)
		d_ptr->_eventLoop->exit();

	delete d_ptr;
}

void Menu::show()
{
    adjustSize();
    const QSize s = (parentWidget()->geometry().size()-	frameSize())/2;
    move(QPoint(s.width(), s.height()));

    qApp->installEventFilter(this);

    QWidget::show();
}

void Menu::hide()
{
    QWidget::hide();

    qApp->removeEventFilter(this);
}

void Menu::exec()
{
	QEventLoop eventLoop;
	d_ptr->_eventLoop = &eventLoop;

	show();

	QPointer<QObject> guard = this;
	eventLoop.exec();
	if(guard.isNull())
		return;

	d_ptr->_eventLoop = 0;
}

void Menu::mouseMoveEvent(QMouseEvent *e)
{
}

void Menu::mousePressEvent(QMouseEvent *e)
{
}

void Menu::mouseDoubleClickEvent(QMouseEvent *e)
{
}

void Menu::mouseReleaseEvent(QMouseEvent *e)
{
}

QSize Menu::sizeHint() const
{
	return QWidget::sizeHint().boundedTo(parentWidget()->size());
}

QSize Menu::minimumSizeHint() const
{
	return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
}

QWidget * Menu::buttonAt(int id) const
{
    return d_ptr->_buttons[id];
}

void Menu::buttonTrigger()
{
    d_ptr->_result = d_ptr->_buttons.indexOf(qobject_cast<QWidget*>(sender()));
    hide();
}

Menu * Menu::createQuery(QString text, QStringList actions)
{
    Menu *dialog = new Menu;
    
    QVBoxLayout *v = new QVBoxLayout;
    
    // add menu text
    if(!text.isEmpty())
    {
        QLabel *l = new QLabel(text, dialog);
        l->setWordWrap(true);
        l->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        v->addWidget(l, 0, Qt::AlignCenter);
    }
    
    // add actions
    if(actions.size())
    {
        QLayout *l;
        
        if(!text.isEmpty())
            l = new QHBoxLayout;
        else
            l = v;
        
        foreach(QString string, actions)
        {
            QPushButton *b = new QPushButton(string, dialog);
            connect(b, SIGNAL(clicked()), dialog, SLOT(buttonTrigger()));
            dialog->d_ptr->_buttons.append(b);
            l->addWidget(b);
        }
        
        if(!text.isEmpty())
            v->addLayout(l);
    }
    
	dialog->setLayout(v);
	
    return dialog;
}

int Menu::execQuery(QString text, QStringList actions)
{
    QScopedPointer<Menu> dialog(createQuery(text, actions));
    dialog->exec();
    return dialog->d_ptr->_result;
}

int Menu::execMenu(QStringList actions)
{
    QScopedPointer<Menu> dialog(createQuery(QString(), actions));
    dialog->exec();
    return dialog->d_ptr->_result;
}

void Menu::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    
    // QStyle::PE_PanelMenu on windows is hollow
#ifdef Q_WS_WIN
    QStyleOptionButton opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &p, this);
#else
    QStyleOptionMenuItem opt;
    opt.initFrom(this);
    opt.state = QStyle::State_None;
    opt.checkType = QStyleOptionMenuItem::NotCheckable;
    opt.maxIconWidth = 0;
    opt.tabWidth = 0;
    style()->drawPrimitive(QStyle::PE_PanelMenu, &opt, &p, this);
#endif

    if(const int fw = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, this))
    {
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
    }
}

bool Menu::event(QEvent *e)
{
	return QWidget::event(e);
}

void Menu::hideEvent(QHideEvent *e)
{
	if(d_ptr->_eventLoop)
		d_ptr->_eventLoop->exit();
}

bool Menu::eventFilter(QObject *watched, QEvent *e)
{
	switch(e->type())
	{
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
        {
            const QPoint p = mapFromGlobal(static_cast<QMouseEvent*>(e)->globalPos());
            const int w = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
		    if(!rect().adjusted(w, w, -w, -w).contains(p))
		    {
                if(e->type() == QEvent::MouseButtonRelease && !d_ptr->_modal)
                    hide();
			    return true;
		    }
        }
		break;
	}
	return false;
}

Menu * Menu::createProgress(QString text)
{
    Menu *dialog = new Menu;

    QVBoxLayout *vl = new QVBoxLayout;

    QLabel *l = new QLabel(text, dialog);
    l->setWordWrap(true);
    vl->addWidget(l, 0, Qt::AlignCenter);

    QProgressBar *pb = new QProgressBar(dialog);
    pb->setRange(0, 100);
	pb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    vl->addWidget(pb, 0, Qt::AlignCenter);

    QPushButton *b = new QPushButton("Cancel", dialog);
    connect(b, SIGNAL(clicked()), dialog, SLOT(cancel()));
    vl->addWidget(b, 0, Qt::AlignCenter);

    dialog->setLayout(vl);

    dialog->d_ptr->_modal = true;

    return dialog;
}

void Menu::setValue(int percent)
{
    QProgressBar *pb = findChild<QProgressBar*>();
    if(pb)
    {
        pb->setValue(percent);
        qApp->processEvents();
    }
}

void Menu::setText(QString text)
{
    QLabel *l = findChild<QLabel*>();
    if(l)
        l->setText(text);
}

bool Menu::wasCanceled()
{
    return d_ptr->_canceled;
}

void Menu::cancel()
{
	d_ptr->_canceled = true;
    hide();
    emit canceled();
}
