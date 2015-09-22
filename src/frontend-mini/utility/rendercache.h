/*************************************************************************
 * rendercache.h - simple cahce for rendered pixmaps
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

#ifndef BT_MINI_RENDERCACHE_H
#define BT_MINI_RENDERCACHE_H

#include <QPainter>
#include <QPixmap>

#include "../item.h"

class RenderCache
{
public:
	struct CACHE
	{
		sbItem *item;
		QRect   rect;
		int     used;

		bool operator < (const CACHE &other) const
		{
			if(rect.top() < other.rect.top())
				return true;
			return false;
		}
	};

	RenderCache ()
	{
		initialized = 0;
		surface = NULL;
	}

	~RenderCache()
	{
		if(surface)
			delete surface;
	}

	bool blit(QPainter *painter, sbItem *item, const QPoint &pos, const QRect &rect)
	{
		// can place in cache
		if(!initialized)
			return false;

		// check if cached
		for ( std::list<CACHE>::iterator it = cache.begin(); it != cache.end(); it++ )
		{
			if ( (*it).item == item )
			{
				if (debug) qDebug() << "In cache " << (*it).item << " rect: " << (*it).rect << " used: " << (*it).used;

				if ((*it).used < -500) return false;

				(*it).used += 3;

				painter->drawPixmap(rect, *surface, QRect((*it).rect.topLeft()+(rect.topLeft()-pos),rect.size()));
				return true;
			}
		}

		// try cache
		QRect place(0, 0, item->size().width(), item->size().height());

		for(std::list<CACHE>::iterator it = cache.begin() ; it != cache.end(); it++)
		{
			if(!(place.top() > (*it).rect.bottom() || place.bottom() < (*it).rect.top()))
			{
				if(debug)
                    qDebug() << "Move cursor " << place << " after " << (*it).rect;
				place.moveTop((*it).rect.bottom()+1);
			}
			else
			{
				break;
			}
		}

		if(place.bottom() > height)
		{
			if(debug)
				qDebug("Cache full\n");
			full = true;
			return false;
		}

		// place in cache
		CACHE ci;
		ci.item = item;
		ci.used = 15;
		ci.rect = place;

		cache.push_back(ci);

		cache.sort();

		{
			if(debug)
				qDebug() << "Place in cache " << ci.item << " " << ci.rect;

			QPainter p2(surface);
			item->render(&p2, ci.rect.topLeft(), ci.rect);
		}
		painter->drawPixmap(rect, *surface, QRect(ci.rect.topLeft()+(rect.topLeft()-pos), rect.size()));
		return true;
	}

	void prepare()
	{
		if(initialized == 0)
		{
			full = false;

			int memoryUse = atoi((*Core_->getOptions())["General"].getWithDefault("DrawCacheSize","0"));

			if(memoryUse == 0)
                memoryUse = Core_->getRect().width() * Core_->getRect().height() * 3 /*bits per pixel*/ * 5 /*multiplier*/;

			width = Core_->getRect().width();

			height = memoryUse/width/3;

			if(height > width)
			{
				surface = new QPixmap(width, height);
				if(surface)
				{
					initialized = 1;
					qDebug("Cache created of size %i x %i\n", width, height);

					debug = atoi((*Core_->getOptions())["General"].getWithDefault("DrawCacheDebug","0")) == 0 ? false : true;
				}
			}

			if(!initialized)
			{
				qDebug("Draw cache not created\n");
				initialized = -1;
			}
		}

		if(!initialized)
            return;

		if(debug)
            qDebug("Begin Paint\n");

		//we can remove from cache only in main thread
		for(std::list<CACHE>::iterator it = cache.begin(); it != cache.end();)
		{
			if((*it).used < -300)
			{
				if(debug)
                    qDebug("Erased %i\n",(*it).item);

				cache.erase(it);
				it = cache.begin();
			}
			else
				it++;
		}

		cache.sort();

		std::list<CACHE>::iterator useless(cache.begin());

		for(std::list<CACHE>::iterator it = cache.begin(); it != cache.end(); it++)
		{
			(*it).used--;

			if((*it).used < (*useless).used)
				useless = it;

			if (debug) qDebug() << "Cache has " << (*it).item << " " << (*it).rect << " used: " << (*it).used;
		}

		if(useless != cache.end() && (full || (*useless).used < -100))
		{
			if(debug)
                qDebug() << "Erased " << (*useless).item;
			cache.erase(useless);
			full = false;
		}

		if(debug)
            qDebug("\n");
	}

	void remove(sbItem *item)
	{
		if(!initialized)
            return;

		for(std::list<CACHE>::iterator it = cache.begin(); it != cache.end(); it++)
		{
			if((*it).item == item)
			{
				(*it).used = -1000;
				return;
			}
		}
	}

private:
	std::list<CACHE>            cache;
	QPixmap                    *surface;

	int                         width;
	int                         height;

	int                         initialized;
	bool                        full;

	bool                        debug;
};

#endif
