#ifndef _H_JStrValue
#define _H_JStrValue

/******************************************************************************
 JStrValue.h

	A convenient struct for use with the most common type of JHashTable, one
	with string keys.  JHashTable< JStrValue<V> > is a hash table with a string
	key and an arbitrary value.

	Copyright (C) 1997 by Dustin Laurence.

 *****************************************************************************/

#include "JString.h"
#include "jHashFunctions.h"

template <class V>
class JStrValue
{
public:

	const JString* key;
	V              value;

	JStrValue()
		:
		key(nullptr)
	{ };

	JStrValue
		(
		const JString*	k,
		const V&		v
		)
		:
		key(k), value(v)
	{ };

	JHashValue
	Hash()
		const
	{
		return JHash7Bit(key->GetBytes());
	};
};

template <class V>
bool
operator==
	(
	const JStrValue<V>& lhs,
	const JStrValue<V>& rhs
	)
{
	return *lhs.key == *rhs.key;
}

#endif
