/*************************************************************************
* sbFeatures.cpp - engine for per-verse data
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

#include <QDateTime>

#include "utility/personalization.h"
#include "core.h"

#include "xmlParser.h"

#pragma warning(disable : 4018)

const int sbFeatures::revision = 1;

sbFeatures::sbFeatures()
{
	;
}

sbFeatures::~sbFeatures()
{
	;
}

void sbFeatures::load ()
{
	if (Core_->defaultModule == NULL) return;

	XMLNode xml = XMLNode::openFileHelper(".\\features.xml");

	if (!xml.isEmpty())
	{
		int loadRevision = 0;
				
		if (!xml.getChildNode("Features").isEmpty())
		{
			const char * text = xml.getChildNode("Features").getAttribute("revision");
			loadRevision = atoi(text);
		}

		if (loadRevision >= revision)
		{
			XMLNode xh = xml.getChildNode("Features").getChildNode("History");

			if (!xh.isEmpty())
			{
				history.reserve(xh.nChildNode("history"));

				for (int i = 0; i < xh.nChildNode("history"); i++)
				{
					XMLNode n = xh.getChildNode("history",i);
					History h;

					h.module     = Core_->holdString(n.getAttribute("module"));
					h.visitStart = QString(n.getAttribute("visitStart")).toLongLong();
					h.visitEnd   = QString(n.getAttribute("visitEnd")).toLongLong();
					h.placeStart = QString(n.getAttribute("placeStart")).toLong();
					h.placeEnd   = QString(n.getAttribute("placeEnd")).toLong();
					
					history.push_back(h);
				}
			}
		}
	}
}

void sbFeatures::save()
{
	if (Core_->defaultModule == NULL) return;
	
	XMLNode xml = XMLNode::createXMLTopNode("xml",true);

	//char buf [32];

	xml.addAttribute("version","1.0");

	XMLNode xf = xml.addChild("Features");

	xf.addAttribute("revision",QString().setNum(revision).toLocal8Bit());

	XMLNode xh = xf.addChild("History");

	// limit history to 100 entries
	for (int i = qMax(100,(int)history.size())-100; i < history.size(); i++)
	{
		XMLNode n = xh.addChild("history");

		n.addAttribute("placeStart", QString().setNum(history[i].placeStart).toLocal8Bit());
		n.addAttribute("placeEnd", QString().setNum(history[i].placeEnd).toLocal8Bit());
		n.addAttribute("visitStart", QString().setNum(history[i].visitStart).toLocal8Bit());
		n.addAttribute("visitEnd", QString().setNum(history[i].visitEnd).toLocal8Bit());
		n.addAttribute("module",history[i].module);
	}

	xml.writeToFile(".\\features.xml");
}

// return history id
// todo, check for previous history item
int sbFeatures::addHistory ( const char * module, long place )
{
	History h;

	h.visitStart = QDateTime::currentMSecsSinceEpoch();
	h.visitEnd   = h.visitStart;
	h.module     = module;
	h.placeStart = h.placeEnd = place;

	history.push_back(h);

	return history.size()-1;
}

void sbFeatures::updateHistory ( int id, long place )
{
	//sbMessage ("sbFeatures::updateHistory id %i place %i\n",id,place);

	if (place < history[id].placeStart)
	{
		history[id].placeStart = place;
	}
	else if (place > history[id].placeEnd)
	{
		history[id].placeEnd = place;
	}

	history[id].visitEnd = QDateTime::currentMSecsSinceEpoch();
}
