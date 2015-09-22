/*************************************************************************
* itemview.h - model/view arhitecture for BibleTime Mini
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

#ifndef BT_MINI_ITEMVIEW_H
#define BT_MINI_ITEMVIEW_H

#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QTextDocument>

class ItemView : public QAbstractItemView
{
    Q_OBJECT

public:
    ItemView(QWidget *parent = 0);
    ~ItemView();

};


class Item
{
public:
    Item() : _td(0)
    {
        ;
    }

    ~Item()
    {
        if(_td)
            delete _td;
    }

    void setText(QString text)
    {
        QTextDocument *td = new QTextDocument;

        td->setDefaultFont(QApplication::font());
        td->setHtml(text);

        if(!_size.isNull())
        {
            td->setTextWidth(_size().width());
            _size.setHeight(td->size().height());
        }

        td->moveToThread(QApplication::instance()->thread());

        qSwap(_td, td);

        if(td)
            delete td;
    }

private:
    QTextDocument *_td;
    QSize          _size;

};

class ItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ItemModel(QObject *parent = 0) : QAbstractItemModel(parent)
    {
        ;
    }

    ~ItemModel()
    {
        foreach(Item *i, _data)
            delete i;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        return _data.size();
    }

private:
    QList<Item*> _data;

};

#endif