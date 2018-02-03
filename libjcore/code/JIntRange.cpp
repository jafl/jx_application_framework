/******************************************************************************
 JIntRange.cpp

	Lightweight class for storing a range of integers.  The range is considered
	to be empty if last < first.  The range contains a single index if
	last == first.

	BASE CLASS = none

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#include <JIntRange.h>
#include <JMinMax.h>
#include <jAssert.h>

/******************************************************************************
 operator+= (covering)

 *****************************************************************************/

JIntRange&
JIntRange::operator+=
	(
	const JIntRange& range
	)
{
	*this = JCovering(*this, range);
	return *this;
}

/******************************************************************************
 JCovering

 *****************************************************************************/

JIntRange
JCovering
	(
	const JIntRange& r1,
	const JIntRange& r2
	)
{
	return JIntRange( JMin(r1.first, r2.first),
					  JMax((r1.IsEmpty() ? r1.first-1 : r1.last),
						   (r2.IsEmpty() ? r2.first-1 : r2.last)) );
}

/******************************************************************************
 JIntersection

	Returns kJTrue if the result is not empty.

 *****************************************************************************/

JBoolean
JIntersection
	(
	const JIntRange&	r1,
	const JIntRange&	r2,
	JIntRange*			result
	)
{
	result->Set(JMax(r1.first, r2.first), JMin(r1.last,  r2.last));
	return !result->IsEmpty();
}

/******************************************************************************
 Stream operators for JIntRange

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&	input,
	JIntRange&	range
	)
{
	input >> range.first >> range.last;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&			output,
	const JIntRange&	range
	)
{
	output << range.first << ' ' << range.last;
	return output;
}
