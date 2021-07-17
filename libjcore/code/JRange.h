/******************************************************************************
 JRange.h

	Copyright (C) 1998-2021 by John Lindal.

 *****************************************************************************/

#ifndef _H_JRange
#define _H_JRange

#include "jTypes.h"

template <class T>
class JRange
{
	static const T	min;

public:

	T	first;
	T	last;

	JRange();
	JRange(const T firstIndex, const T lastIndex);

	bool
	Is
		(
		const T firstIndex,
		const T lastIndex
		)
		const
	{
		return first == firstIndex && last == lastIndex;
	};

	bool
	IsEmpty() const
	{
		return last < first || (min == 0 && last == 0);
	};

	JSize
	GetCount() const
	{
		return (first <= last && (min != 0 || last > 0) ? last-first+1 : 0);
	};

	bool
	Contains
		(
		const T index
		)
		const
	{
		return first <= index && index <= last && (min != 0 || last > 0);
	};

	bool
	Contains
		(
		const JRange<T>& range
		)
		const
	{
		const bool e1 = IsEmpty();
		const bool e2 = range.IsEmpty();
		return (IsNothing() && range.IsNothing()) ||
					( e1 && e2 && first == range.first) ||
					(!e1 && first <= range.first && range.last <= last);
	};

	void
	Set
		(
		const T firstIndex,
		const T lastIndex
		)
	{
		first = firstIndex;
		last  = lastIndex;
	};

	void
	SetFirstAndCount
		(
		const T		firstIndex,
		const JSize	count
		)
	{
		first = firstIndex;
		last  = firstIndex + count - 1;
	};

	bool
	IsNothing() const
	{
		return first == 0 && last == 0;
	};

	void
	SetToNothing()
	{
		first = last = 0;
	};

	void
	SetToEmptyAt
		(
		const T firstIndex
		)
	{
		first = firstIndex;
		last  = first-1;
	};

	JRange&
	operator+=
		(
		const T offset
		)
	{
		first += offset;
		last  += offset;
		return *this;
	};

	JRange&
	operator-=
		(
		const T offset
		)
	{
		first -= offset;
		last  -= offset;
		return *this;
	};

	JRange& operator+=(const JRange& range);	// covering

//	bool	Complement(const JArray<JRange>& subsetList,
//						   JArray<JRange>* complement);
};

template <class T> std::istream& operator>>(std::istream& input, JRange<T>& range);
template <class T> std::ostream& operator<<(std::ostream& output, const JRange<T>& range);

template <class T> bool		JIntersection(const JRange<T>& r1, const JRange<T>& r2, JRange<T>* result);
template <class T> JRange<T>	JCovering(const JRange<T>& r1, const JRange<T>& r2);

template <class T>
inline int
operator==
	(
	const JRange<T>& r1,
	const JRange<T>& r2
	)
{
	return ((r1.IsEmpty() && r2.IsEmpty() && r1.first == r2.first) ||
			(r1.first == r2.first && r1.last == r2.last));
}

template <class T>
inline int
operator!=
	(
	const JRange<T>& r1,
	const JRange<T>& r2
	)
{
	return !(r1 == r2);
}

template <class T>
inline JRange<T>
operator+
	(
	const JRange<T>&	r,
	const T				offset
	)
{
	JRange<T> r1 = r;
	r1          += offset;
	return r1;
}

template <class T>
inline JRange<T>
operator+
	(
	const T				offset,
	const JRange<T>&	r
	)
{
	JRange<T> r1 = r;
	r1          += offset;
	return r1;
}

template <class T>
inline JRange<T>
operator-
	(
	const JRange<T>&	r,
	const T				offset
	)
{
	JRange<T> r1 = r;
	r1          -= offset;
	return r1;
}

template <class T>
inline JRange<T>
operator+
	(
	const JRange<T>& r1,
	const JRange<T>& r2
	)
{
	JRange<T> r3 = r1;
	r3          += r2;
	return r3;
}

#include "JRange.tmpl"

typedef JRange<JInteger> JIntRange;
typedef JRange<JIndex> JIndexRange;

#endif
