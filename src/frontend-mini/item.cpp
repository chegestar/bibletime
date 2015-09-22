/*************************************************************************
 * item.cpp - common item
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
#include <QPainter>
#include <QTextDocument>
#include <QWidget>

#include "item.h"

#pragma warning(disable : 4018) // signed/unsigned mismatch

class Text
{
public:
    Text(QFont &font, QColor &color)
    {
        ;
    }

    Text & operator += (QChar c)
    {
        ;
    }

private:
    QPoint   _pos;
    QString  _string;
    QFont    _font;
    QColor   _color;
};

class sbItemPrivate
{
public:
	sbItemPrivate() : _textDocument(0)
	{
		;
	}
	~sbItemPrivate()
	{
		if(_textDocument)
			delete _textDocument;
	}

	QSize                         _size;
	QTextDocument                *_textDocument;
};

sbItem::sbItem(sbItem::TYPE type, const QString &text, int width, long index)
    : type(type), index(index), next(NULL), prev(NULL), left(NULL), right(NULL), d_ptr(new sbItemPrivate)
{
	highlighted = false;

    if(width == -1)
        foreach(QWidget *w, QApplication::topLevelWidgets())
            if(w->isVisible())
                width = w->width();

    Q_ASSERT(width > 0);

	setWidth(width);
	expanding = 0;

	processed = false;

	setText(text);
}

sbItem::~sbItem()
{
	if(next  != NULL)
	    next->prev = prev;
	if(prev  != NULL)
	    prev->next = next;
	if(right != NULL)
	    delete right;
	delete d_ptr;
}

void sbItem::attach(sbItem *item, sbDirection to)
{
	Q_ASSERT(this != item);

	if(to == DIRECTION_NEXT)
	{
		// check space

		if(next != NULL)
		{
			item->prev = this;
			item->next = this->next;

			next->prev = item;
			next = item;
		}
		else
		{
			next = item;
			item->prev = this;
		}
	}
	else if(to == DIRECTION_PREV)
	{
		if(prev != NULL)
		{
			item->next = this;
			item->prev = this->prev;

			prev->next = item;
			prev = item;
		}
		else
		{
			prev = item;
			item->next = this;
		}
	}
	else if(to == DIRECTION_LEFT)
	{
		Q_ASSERT(left == NULL);

		left = item;
		item->right = this;
	}
	else if(to == DIRECTION_RIGHT)
	{
		Q_ASSERT(right == NULL);

		// place next if line filled

		right = item;
		item->left = this;

		if(size().height() < item->size().height())
		{
			sbItem *parse = this;
			while(parse != NULL)
			{
				parse->setHeight(item->size().height());
				parse = parse->left;
			}
		}

		if(size().height() > item->size().height())
			item->setHeight(size().height());
	}
	else
		Q_ASSERT(false);
}

void sbItem::setText(const QString &newText)
{
    //qDebug((QString("Add Text:\n")+newText).toLocal8Bit());

	QTextDocument *td = new QTextDocument;

#ifndef Q_OS_WINCE
    td->setDefaultStyleSheet(qobject_cast<QApplication*>(QApplication::instance())->styleSheet());
#endif

	td->setDefaultFont(QApplication::font());
	td->setTextWidth(size().width());
	td->setHtml(newText);
	setHeight(td->size().height());

    //qDebug((QString("Got Text:\n")+td->toHtml()+QString("\n")+td->defaultStyleSheet()).toLocal8Bit());

	td->moveToThread(QApplication::instance()->thread());

	//d_ptr->_textDocument = td;
    qSwap(d_ptr->_textDocument, td);

    if(td)
        delete td;
}

void sbItem::render(QPainter *painter, const QPoint &pos, QRect clip) const
{
	if(clip.isNull())
		clip = QRect(pos.x(), pos.y(), size().width(), size().height());

	painter->save();
	painter->translate(pos);
	clip.moveTo(clip.topLeft()-pos);
	d_ptr->_textDocument->drawContents(painter, clip);
	painter->restore();
	return;
}

sbItem * sbItem::append(sbItem::TYPE type, const QString &text, int width, long index)
{
	sbItem *n = create(type, text, width, index);

	if(this != NULL)
	{
		sbItem *l = getItem(DIRECTION_NEXT, true);
		l->attach(n, DIRECTION_NEXT);
	}
	return n;
}

sbItem * sbItem::getItem(TYPE type)
{
	sbItem *w = this;

	while(w->prev != NULL)
	    w = w->prev;
	while(w->type != type && w->next != NULL)
	    w = w->next;

	if(w->type == type)
		return w;
	else
		return NULL;
}

sbItem * sbItem::getItem(sbDirection direction, bool last)
{
	sbItem *i = this;

	switch(direction)
	{
	case DIRECTION_NEXT:
		if(last)
		{
			while(i->left  != NULL)
			    i = i->left;
			while(i->next  != NULL)
			    i = i->next;
			while(i->right != NULL)
			    i = i->right;

			return i;
		}
		else
		{
			int c = 0;
			for(; i->left != NULL; ++c, i = i->left)
			    ;
			if((i = i->next) != NULL)
			    for(; c > 0 && i != NULL; --c, i = i->right)
			        ;
			return i;
		}
	default:
		Q_ASSERT(false);
	}

	return i;
}

sbItem * sbItem::create(TYPE type, const QString &text, int width, long index)
{
	return new sbItem(type, text, width, index);
}

const QSize & sbItem::size() const
{
	return d_ptr->_size;
}

void sbItem::setWidth(qreal size)
{
	d_ptr->_size.setWidth(size);
    if(d_ptr->_textDocument)
    {
        d_ptr->_textDocument->setTextWidth(size);
        setHeight(d_ptr->_textDocument->size().height());
    }
}

void sbItem::setHeight(qreal size)
{
	d_ptr->_size.setHeight(size);
}
