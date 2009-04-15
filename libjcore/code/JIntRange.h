/******************************************************************************
 JIntRange.h

	Copyright © 1998 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JIntRange
#define _H_JIntRange

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JIntRange
{
public:

	JInteger	first;
	JInteger	last;

	JIntRange()
		:
		first(0),
		last(-1)
	{ };

	JIntRange
		(
		const JInteger firstIndex,
		const JInteger lastIndex
		)
		:
		first(firstIndex),
		last(lastIndex)
	{ };

	JBoolean
	Is
		(
		const JInteger firstIndex,
		const JInteger lastIndex
		)
		const
	{
		return JI2B( first == firstIndex && last == lastIndex );
	};

	JBoolean
	IsEmpty() const
	{
		return JI2B( last < first );
	};

	JSize
	GetLength() const
	{
		return (first <= last ? last-first+1 : 0);
	};

	JBoolean
	Contains
		(
		const JInteger index
		)
		const
	{
		return JI2B( first <= index && index <= last );
	};

	JBoolean
	Contains
		(
		const JIntRange& range
		)
		const
	{
		const JBoolean e1 = IsEmpty();
		const JBoolean e2 = range.IsEmpty();
		return JI2B(( e1 &&  e2 && first == range.first) ||
					(!e1 &&  e2 && first <  range.first && range.first <= last) ||
					(!e1 && !e2 && first <= range.first && range.last <= last));
	};

	void
	Set
		(
		const JInteger firstIndex,
		const JInteger lastIndex
		)
	{
		first = firstIndex;
		last  = lastIndex;
	};

	void
	SetFirstAndLength
		(
		const JInteger	firstIndex,
		const JSize		length
		)
	{
		first = firstIndex;
		last  = firstIndex + length - 1;
	};

	void
	SetToEmptyAt
		(
		const JInteger firstIndex
		)
	{
		first = firstIndex;
		last  = first-1;
	};

	JIntRange&
	operator+=
		(
		const JInteger offset
		)
	{
		first += offset;
		last  += offset;
		return *this;
	};

	JIntRange&
	operator-=
		(
		const JInteger offset
		)
	{
		first -= offset;
		last  -= offset;
		return *this;
	};

	JIntRange& operator+=(const JIntRange& range);	// covering

//	static JSize	Complement(const JIntRange& set, const JArray<JIntRange>& subsetList,
//							   JArray<JIntRange>* complement);
};

istream& operator>>(istream& input, JIntRange& range);
ostream& operator<<(ostream& output, const JIntRange& range);

JBoolean	JIntersection(const JIntRange& r1, const JIntRange& r2, JIntRange* result);
JIntRange	JCovering(const JIntRange& r1, const JIntRange& r2);

inline int
operator==
	(
	const JIntRange& r1,
	const JIntRange& r2
	)
{
	return ((r1.IsEmpty() && r2.IsEmpty() && r1.first == r2.first) ||
			(r1.first == r2.first && r1.last == r2.last));
}

inline int
operator!=
	(
	const JIntRange& r1,
	const JIntRange& r2
	)
{
	return !(r1 == r2);
}

#endif
