/******************************************************************************
 JIndexRange.h

	Copyright © 1998 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JIndexRange
#define _H_JIndexRange

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <JMinMax.h>
#include <sys/types.h>

class JIndexRange
{
public:

	JIndex	first;
	JIndex	last;

	JIndexRange()
		:
		first(0),
		last(0)
	{ };

	JIndexRange
		(
		const JIndex firstIndex,
		const JIndex lastIndex
		)
		:
		first(firstIndex),
		last(lastIndex)
	{ };

	JBoolean
	Is
		(
		const JIndex firstIndex,
		const JIndex lastIndex
		)
		const
	{
		return JI2B( first == firstIndex && last == lastIndex );
	};

	JBoolean
	IsEmpty() const
	{
		return JI2B( last < first || last == 0 );
	};

	JSize
	GetLength() const
	{
		return ((first <= last && last > 0) ? last-first+1 : 0);
	};

	JBoolean
	Contains
		(
		const JIndex index
		)
		const
	{
		return JI2B( first <= index && index <= last && last > 0 );
	};

	JBoolean
	Contains
		(
		const JIndexRange& range
		)
		const
	{
		const JBoolean e1 = IsEmpty();
		const JBoolean e2 = range.IsEmpty();
		return JI2B((IsNothing() && range.IsNothing()) ||
					( e1 &&  e2 && first == range.first) ||
					(!e1 &&  e2 && first <  range.first && range.first <= last) ||
					(!e1 && !e2 && first <= range.first && range.last <= last));
	};

	void
	Set
		(
		const JIndex firstIndex,
		const JIndex lastIndex
		)
	{
		first = firstIndex;
		last  = lastIndex;
	};

	void
	SetFirstAndLength
		(
		const JIndex	firstIndex,
		const JSize		length
		)
	{
		first = firstIndex;
		last  = firstIndex + length - 1;
	};

	JBoolean
	IsNothing() const
	{
		return JI2B( first == 0 && last == 0 );
	};

	void
	SetToNothing()
	{
		first = last = 0;
	};

	void
	SetToEmptyAt
		(
		const JIndex firstIndex
		)
	{
		first = firstIndex;
		last  = first-1;
	};

	JIndexRange&
	operator+=
		(
		const JIndex offset
		)
	{
		first += offset;
		last  += offset;
		return *this;
	};

	JIndexRange&
	operator-=
		(
		const JIndex offset
		)
	{
		first -= offset;
		last  -= offset;
		return *this;
	};

	JIndexRange& operator+=(const JIndexRange& range);	// covering

//	JBoolean	Complement(const JArray<JIndexRange>& subsetList,
//						   JArray<JIndexRange>* complement);
};

istream& operator>>(istream& input, JIndexRange& range);
ostream& operator<<(ostream& output, const JIndexRange& range);

JBoolean	JIntersection(const JIndexRange& r1, const JIndexRange& r2, JIndexRange* result);
JIndexRange	JCovering(const JIndexRange& r1, const JIndexRange& r2);

inline int
operator==
	(
	const JIndexRange& r1,
	const JIndexRange& r2
	)
{
	return ((r1.IsEmpty() && r2.IsEmpty() && r1.first == r2.first) ||
			(r1.first == r2.first && r1.last == r2.last));
}

inline int
operator!=
	(
	const JIndexRange& r1,
	const JIndexRange& r2
	)
{
	return !(r1 == r2);
}

inline JIndexRange
operator+
	(
	const JIndexRange&	r,
	const JIndex		offset
	)
{
	JIndexRange r1 = r;
	r1            += offset;
	return r1;
}

inline JIndexRange
operator+
	(
	const JIndex		offset,
	const JIndexRange&	r
	)
{
	JIndexRange r1 = r;
	r1            += offset;
	return r1;
}

inline JIndexRange
operator-
	(
	const JIndexRange&	r,
	const JIndex		offset
	)
{
	JIndexRange r1 = r;
	r1            -= offset;
	return r1;
}

inline JIndexRange
operator+
	(
	const JIndexRange& r1,
	const JIndexRange& r2
	)
{
	JIndexRange r3 = r1;
	r3            += r2;
	return r3;
}

#endif
