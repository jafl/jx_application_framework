#ifndef _H_JStrValue
#define _H_JStrValue

/******************************************************************************
 JStrValue.h

	A convenient struct for use with the most common type of JHashTable, one
	with string keys.  JHashTable< JStrValue<V> > is a hash table with a string
	key and an arbitrary value.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

 *****************************************************************************/

#include <JString.h>

template <class V>
class JStrValue
{
public:

	const JString* key;
	V              value;

	JStrValue()
		:
		key(NULL)
	{ };

	JStrValue
		(
		const JString*	k,
		const V&		v
		)
		:
		key(k), value(v)
	{ };
};

#endif
