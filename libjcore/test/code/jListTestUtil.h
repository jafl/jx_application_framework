/******************************************************************************
 jListTestUtil.h

   Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#ifndef _H_jListTestUtil
#define _H_jListTestUtil

#include "jListTestUtil.h"
#include "jx-af/jcore/JList.h"
#include "jx-af/jcore/JString.h"
#include <sstream>
#include "jx-af/jcore/jAssert.h"

#define verify(str, list)	JAssertStringsEqual(str, JPrintList(list));

/******************************************************************************
 JSetList

 ******************************************************************************/

template <class T>
void
JSetList
	(
	const JUtf8Byte*	data,
	JList<T>*			list
	)
{
	list->RemoveAll();

	JListIterator<T>* iter = list->NewIterator();

	std::istringstream input(data);
	T v;
	while (input.good())
		{
		input >> v;
		iter->Insert(v);
		iter->SkipNext();
		}

	jdelete iter;
}

/******************************************************************************
 JPrintList

 ******************************************************************************/

template <class T>
JString
JPrintList
	(
	const JList<T>& list
	)
{
	std::ostringstream data;

	JListIterator<T>* iter = list.NewIterator();

	T v;
	while (iter->Next(&v))
		{
		data << v << ' ';
		}

	jdelete iter;

	JString s(data.str());
	s.TrimWhitespace();
	return s;
}

#endif
