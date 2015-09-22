/*************************************************************************
 * state.h - items scrolling container
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

#ifndef BT_MINI_STATE_H
#define BT_MINI_STATE_H

#include <vector>

#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

#include "contents.h"

class sbItem;
class sbListPrivate;

class sbList
{
    Q_DECLARE_PRIVATE(sbList)

public:	
    sbList(View::Type type);
	~sbList();

	// event handlers
	void                      onPressed           (int x, int y);
	void                      onReleased          (int x, int y);
	void                      onClicked           (int x, int y);
	void                      onTapped            (int x, int y);

	bool                      onKey               (int key);
	
	void                      onActivate          ();
	void                      onItem              (sbItem *item);

	void                      onUpdate            ();

	// methods
    void                      attach              (sbList *list, View::Type pin);
	bool                      scroll              (float amount );
	void                      render              (QPainter *painter, int sideDisplacement = 0);

	float                     displace;
	int                       screenHeight;

	int                       needScroll;
	bool                      needFill;
	
    const View::Type          type;

	sbList*                   getNext             () const {return next;}
	sbList*                   getPrev             () const {return prev;}

	void                      process             (bool *abort);

	void                      performSearch       (QString request);

private:
    sbListPrivate     * const d_ptr;

	void                      clear               ();
	sbItem *                  getItem             (int x , int y) const;

	sbItem                   *itemCurrent;
	sbItem                   *itemHover;
	sbItem                   *itemSelected;

	sbItem                   *itemBanner;

	// Module List Related

	struct BOUND
	{
		BOUND () { memset (this,0,sizeof(BOUND)); constructed = true; }

		sbItem               *verse;
		int                   verseCount;
		bool                  constructed;
		int                   height;
	};

	BOUND                     before;
    BOUND                     after;
	
	// List Related
	sbList                   *next;
	sbList                   *prev;

	QRect                     rect;

	std::vector<const char *> options;

public:
	const char               *_module;
	sword::VerseKey           _place;

	int                       attachedReading;
	int                       attachedHistory;
};

#endif
