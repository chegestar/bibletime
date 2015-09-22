/*************************************************************************
* biblep13n.h - Bible personalization
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

#ifndef SBFEATURES_H
#define SBFEATURES_H

#include <QtGlobal>

#include <vector>

class sbFeatures
{
public:
	sbFeatures ();
	~sbFeatures();

	/*struct Reading
	{
		const char               * module;
		long                       place;
		time_t                     created;
		time_t                     visited;
	};*/

	struct Bookmark
	{
		//const TCHAR              * name;
		const char               * module;
		long                       place;
//		time_t                     created;
//		time_t                     visited;
	};

	struct History
	{
		const char               * module;
		long                       placeStart;
		long                       placeEnd;
		qint64                     visitStart;
		qint64                     visitEnd;
	};

	int                       addHistory     ( const char * module, long place );
	void                      updateHistory  ( int id, long place );

	void                      save ();
	void                      load ();

	void                      dump ();

	static const int          revision;

	//std::vector<Reading>      readings;
	std::vector<History>      history;

//private:
//	std::vector<const char*>  strings;

};

#endif
