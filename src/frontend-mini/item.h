/*************************************************************************
 * item.h - common item
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

#ifndef BT_MINI_ITEM_H
#define BT_MINI_ITEM_H

//#include <vector>
//#include <string>

// Global directions
enum sbDirection
{
	DIRECTION_NONE  = 0,
	DIRECTION_NEXT  = 1 << 0,
	DIRECTION_PREV  = 1 << 1,
	DIRECTION_LEFT  = 1 << 2,
	DIRECTION_RIGHT = 1 << 3,
	DIRECTION_COUNT
};

class sbItemPrivate;

class sbItem
{
	Q_DECLARE_PRIVATE(sbItem)
public:
	// type responds for visual appearance and work that item do
	// some types like LOOK_CONTAINER should create derived classes
	enum TYPE
	{
		DO_NONE = 0,

		LOOK_NORMAL = 0,
		TYPE_VERSE,
		TYPE_BUTTON_BOOKMARK,
		TYPE_GOTO_MODULES,
		TYPE_GO_HISTORY,
		TYPE_HISTORY,
		TYPE_MODULE_INSTALLER_MODULE,
		TYPE_MODULE_INSTALLER_UPDATE,
		TYPE_GO_READING,
		TYPE_BUTTON_SELECTION_MODULE,
		TYPE_BUTTON_SELECTION_BOOK,
		TYPE_BUTTON_SELECTION_CHAPTER,
		TYPE_BOOKMARK_VERSE_NUMBER,
		LOOK_NORMAL_COUNT,

		LOOK_CONTAINER,
		TYPE_CONTAINER_PANEL,
		TYPE_CONTAINER_DIALOG,
		TYPE_CONTAINER_MENU,
		LOOK_CONTAINER_COUNT,

		LOOK_VERSE,

		LOOK_SEPARATOR,
		TYPE_CENTRED_DESCRIPTION,

		LOOK_DESCRIPTION,

		LOOK_CAPTION,

		LOOK_INPUT,
		TYPE_TEXT_FIELD,

		LOOK_BUTTON,
		TYPE_OPEN_MODULE_INSTALLER,
		TYPE_SWITCH_MODULE_OPTION,
		TYPE_START_SEARCH,
		TYPE_MODULE_INSTALLER_REPO,
		LOOK_BUTTON_COUNT,

		TYPE_COUNT
	};

	sbItem * getItem(sbDirection direction, bool last = false);
	sbItem * getItem(TYPE type );

	// hide
	sbItem(sbItem::TYPE type, const QString &text, int width = -1, long index = -1);
	virtual ~sbItem();

	virtual void                  render(QPainter *painter, const QPoint &pos, QRect clip = QRect()) const;

	const TYPE                    type;
	char                          style;
	bool                          processed;

	// none, highlighted, inactive, hover, hover+highlighted
	bool                          highlighted;
	bool                          active;
	QString                       text;

private:
	sbItemPrivate         * const d_ptr;

public:
	const QSize &                 size() const;
	void                          setWidth(qreal size);
	void                          setHeight(qreal size);

	unsigned short                expanding;
	
	//struct LINE
	//{
	//	LINE () { memset (this,0,sizeof(LINE)); }
    //
	//	enum TYPE
	//	{
	//		TYPE_NONE = 0,
	//		TYPE_VERSE_NUMBER,
	//		TYPE_SPACE,
	//		TYPE_STRONG,
	//		TYPE_MORPH,
	//		TYPE_REDWORDS,
	//		TYPE_LINE,
	//		TYPE_IMAGE,
	//		TYPE_IMAGE_COUNT = TYPE_IMAGE + DIRECTION_COUNT /*alignment*/
	//	};
    //
	//	unsigned short            pos;
	//	unsigned short            count;
	//	unsigned short            width;
	//	unsigned short            height;
	//	char                      style;
	//	TYPE                      type;
	//	bool                      ending;
	//};
    //
	//std::vector<LINE>             lines;

	void                          setText       (const QString &text);

	long                          index;

	// TODO ? make private
    void                          attach        (sbItem * item, sbDirection to);

    sbItem                       *next;
    sbItem                       *prev;
    sbItem                       *left;
    sbItem                       *right;
	
	// TODO ? automatic item width using sbTheme 
	// perform automatic layout and hidden derived class creation
	sbItem *                      append        (sbItem::TYPE type, const QString &text, int width, long index = -1);
	static sbItem *               create        (sbItem::TYPE type, const QString &text, int width, long index = -1);
};

#endif
