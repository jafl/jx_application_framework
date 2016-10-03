#ifndef _H_JStringMapCursor
#define _H_JStringMapCursor

/******************************************************************************
 JStringMapCursor.h

	Interface for the JStringMapCursor class.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JConstHashCursor.h>
#include <JStrValue.h>

template <class V> class JStringMap;

template <class V>
class JStringMapCursor
{
public:

	JStringMapCursor(const JStringMap<V>* map);
	virtual ~JStringMapCursor();

	JBoolean Next();

	const JCharacter* GetKey() const;
	const V& GetValue() const;

	void Reset();

private:

	JConstHashCursor< JStrValue<V> > itsCursor;

private:

	// not allowed

	JStringMapCursor(const JStringMapCursor& source);
	const JStringMapCursor& operator=(const JStringMapCursor& source);
};

/******************************************************************************
 Next

 *****************************************************************************/

template <class V>
inline JBoolean
JStringMapCursor<V>::Next()
{
	return itsCursor.NextFull();
}

/******************************************************************************
 GetKey

 *****************************************************************************/

template <class V>
inline const JCharacter*
JStringMapCursor<V>::GetKey() const
{
	return itsCursor.GetValue().key;
}

/******************************************************************************
 GetValue

 *****************************************************************************/

template <class V>
inline const V&
JStringMapCursor<V>::GetValue() const
{
	return itsCursor.GetValue().value;
}

/******************************************************************************
 Reset

 *****************************************************************************/

template <class V>
inline void
JStringMapCursor<V>::Reset()
{
	itsCursor.Reset();
}

#endif
