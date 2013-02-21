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

#include "btminimenu.h"


class BtMiniMenuPrivate
{
public:
    BtMiniMenuPrivate()
    {
        _result    = -1;
		_eventLoop = 0;
        _modal     = false;
        _canceled  = false;
        _popup     = false;
    }
    
    ~BtMiniMenuPrivate()
    {
        ;
    }

	void updateGeometry(QWidget *menu)
	{
		menu->adjustSize();
		const QSize s = (menu->parentWidget()->geometry().size() - menu->frameSize())/2;
		menu->move(QPoint(s.width(), s.height()));
	}
    
    QList<QWidget*>  _buttons;
    int              _result;
	QEventLoop      *_eventLoop;
    bool             _modal;
	bool             _canceled;
    bool             _popup;

};

BtMiniMenu::BtMiniMenu() : d_ptr(new BtMiniMenuPrivate)
	, QWidget(QApplication::topLevelWidgets()[0], Qt::FramelessWindowHint)
{
	//setWindowModality(Qt::ApplicationModal);
    //setModal(false);
    //setAutoFillBackground(false);
	//setAttribute(Qt::WA_MouseNoMask);
	//setAttribute(Qt::WA_NoMousePropagation);

	if(parentWidget())
	{
		setMaximumSize(parentWidget()->size());

		QFont f(parentWidget()->font());
		f.setBold(true);
		f.setPixelSize(f.pixelSize() * 1.3);
		setFont(f);
	}
	else
		Q_ASSERT(false);

	hide();
}

BtMiniMenu::~BtMiniMenu()
{
	if(d_ptr->_eventLoop)
		d_ptr->_eventLoop->exit();

	delete d_ptr;
}

void BtMiniMenu::show()
{
	d_ptr->updateGeometry(this);

    qApp->installEventFilter(this);

    QWidget::show();
}

void BtMiniMenu::hide()
{
    QWidget::hide();

    qApp->removeEventFilter(this);
}

void BtMiniMenu::exec()
{
	QEventLoop eventLoop;
	d_ptr->_eventLoop = &eventLoop;

	show();

    // on Android virtual keyboard didnt hide when action key is pressed
    setFocus();

	QPointer<QObject> guard = this;
	
	eventLoop.exec();
	
	if(guard.isNull())
		return;

	d_ptr->_eventLoop = 0;
}

void BtMiniMenu::mouseMoveEvent(QMouseEvent *e)
{
}

void BtMiniMenu::mousePressEvent(QMouseEvent *e)
{
}

void BtMiniMenu::mouseDoubleClickEvent(QMouseEvent *e)
{
}

void BtMiniMenu::mouseReleaseEvent(QMouseEvent *e)
{
}

QSize BtMiniMenu::sizeHint() const
{
	QSize s(parentWidget()->size());

	// take some place
	if(s.width() * 0.8 > s.height())
		s.setWidth(s.width() * 0.9 - (parentWidget()->fontMetrics().height() * 2));
	else
		s.setHeight(s.height() * 0.9 - (parentWidget()->fontMetrics().height() * 2));

	return QWidget::sizeHint().boundedTo(s);
}

QSize BtMiniMenu::minimumSizeHint() const
{
	return QWidget::minimumSizeHint().boundedTo(parentWidget()->size());
}

QWidget * BtMiniMenu::buttonAt(int id) const
{
    return d_ptr->_buttons[id];
}

void BtMiniMenu::buttonTrigger()
{
    d_ptr->_result = d_ptr->_buttons.indexOf(qobject_cast<QWidget*>(sender()));
    hide();
}

BtMiniMenu * BtMiniMenu::createQuery(QString text, QStringList actions)
{
    BtMiniMenu *dialog = new BtMiniMenu;
    
    QVBoxLayout *v = new QVBoxLayout;

    const int m = dialog->font().pixelSize() / 4;
    v->setSpacing(m);
    v->setContentsMargins(m, m, m, m);
    
    // add menu text
    if(!text.isEmpty())
    {
        QLabel *l = new QLabel(text, dialog);
		l->setWordWrap(true);
		
		//l->setMinimumWidth(1);
		//l->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred, QSizePolicy::Label));

		//QSizePolicy p(l->sizePolicy());
		//p.setHorizontalPolicy(QSizePolicy::Expanding);
		//p.setVerticalPolicy(QSizePolicy::Expanding);
		//l->setSizePolicy(p);

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
    else
        dialog->d_ptr->_popup = true;
    
	dialog->setLayout(v);
	
    return dialog;
}

int BtMiniMenu::execQuery(QString text, QStringList actions)
{
    QScopedPointer<BtMiniMenu> dialog(createQuery(text, actions));
    dialog->exec();
    return dialog->d_ptr->_result;
}

int BtMiniMenu::execMenu(QStringList actions)
{
    QScopedPointer<BtMiniMenu> dialog(createQuery(QString(), actions));
    dialog->exec();
    return dialog->d_ptr->_result;
}

void BtMiniMenu::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    
    // QStyle::PE_PanelMenu on windows is hollow
#if defined Q_WS_WIN || defined ANDROID
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

bool BtMiniMenu::event(QEvent *e)
{
	return QWidget::event(e);
}

void BtMiniMenu::hideEvent(QHideEvent *e)
{
	if(d_ptr->_eventLoop)
		d_ptr->_eventLoop->exit();
}

bool BtMiniMenu::eventFilter(QObject *o, QEvent *e)
{
    //qDebug() << "menu event filter" << watched << e;

	switch(e->type())
	{
	case QEvent::Resize:
		if(o == parentWidget())
		{
			setMaximumSize(parentWidget()->size());
			d_ptr->updateGeometry(this);
		}
		break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove:
        {
            const QPoint p = mapFromGlobal(static_cast<QMouseEvent*>(e)->globalPos());
            const int w = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
            if(d_ptr->_popup || !rect().adjusted(w, w, -w, -w).contains(p))
		    {
                if(e->type() == QEvent::MouseButtonPress && !d_ptr->_modal)
                    cancel();
			    return true;
		    }
        }
		break;
    case QEvent::Close:
        if(children().contains(o))
            hide();
        break;
	}
	return false;
}

BtMiniMenu * BtMiniMenu::createProgress(QString text)
{
    BtMiniMenu *dialog = new BtMiniMenu;

    QVBoxLayout *vl = new QVBoxLayout;

	const int m = dialog->font().pixelSize() / 4;
	vl->setSpacing(m);
	vl->setContentsMargins(m, m, m, m);

    QLabel *l = new QLabel(text, dialog);
    l->setWordWrap(true);
    vl->addWidget(l, 0, Qt::AlignCenter);

    QProgressBar *pb = new QProgressBar(dialog);
    pb->setRange(0, 100);
	pb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    vl->addWidget(pb, 0, Qt::AlignCenter);

    QPushButton *b = new QPushButton(tr("Cancel"), dialog);
    connect(b, SIGNAL(clicked()), dialog, SLOT(cancel()));
    vl->addWidget(b, 0, Qt::AlignCenter);

    dialog->setLayout(vl);
	dialog->d_ptr->_modal = true;

    return dialog;
}

void BtMiniMenu::setValue(int percent)
{
    QProgressBar *pb = findChild<QProgressBar*>();
    if(pb)
    {
        pb->setValue(percent);
        qApp->processEvents();
    }
}

void BtMiniMenu::setText(QString text)
{
    QLabel *l = findChild<QLabel*>();
    if(l)
        l->setText(text);
}

bool BtMiniMenu::wasCanceled()
{
    return d_ptr->_canceled;
}

void BtMiniMenu::cancel()
{
	d_ptr->_canceled = true;
    hide();
    emit canceled();
}
