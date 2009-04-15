/******************************************************************************
 JIndexRange.cpp

	Lightweight class for storing a range of indices.  The range is considered
	to be empty if last < first.  The range contains a single index if
	last == first.

	The special value (0,0) represents no range at all--this is different than
	a zero length string.  A regular expression subexpression might match a
	zero-length expression at a particular point in a string, which is not the
	same as not matching at all; '^(x*)|(a)' matching on 'bb' would return a
	positive match of (1,0) indicating an overall match of a zero length string
	at the beginning and subexpression matches of (1,0) and (0,0) indicating
	that the first parenthesized subexpression matched the entire overall match
	while the second did not participate in the match at all.

	BASE CLASS = none

	Copyright © 1998 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JIndexRange.h>
#include <JMinMax.h>
#include <jAssert.h>

/******************************************************************************
 operator+= (covering)

 *****************************************************************************/

JIndexRange&
JIndexRange::operator+=
	(
	const JIndexRange& range
	)
{
	*this = JCovering(*this, range);
	return *this;
}

/******************************************************************************
 JCovering

 *****************************************************************************/

JIndexRange
JCovering
	(
	const JIndexRange& r1,
	const JIndexRange& r2
	)
{
	const JBoolean n1 = r1.IsNothing();
	const JBoolean n2 = r2.IsNothing();
	if (n1 && n2)
		{
		return JIndexRange();
		}
	else if (n1)
		{
		return r2;
		}
	else if (n2)
		{
		return r1;
		}
	else
		{
		return JIndexRange( JMin(r1.first, r2.first),
							JMax((r1.IsEmpty() ? r1.first-1 : r1.last),
								 (r2.IsEmpty() ? r2.first-1 : r2.last)) );
		}
}

/******************************************************************************
 JIntersection

	Returns kJTrue if the result is not empty.

 *****************************************************************************/

JBoolean
JIntersection
	(
	const JIndexRange&	r1,
	const JIndexRange&	r2,
	JIndexRange*		result
	)
{
	const JIndex min = JMax(r1.first, r2.first);
	const JIndex max = JMin(r1.last,  r2.last);
	if (max >= min)
		{
		result->Set(min, max);
		return kJTrue;
		}
	else
		{
		result->SetToNothing();
		return kJFalse;
		}
}

/******************************************************************************
 Stream operators for JIndexRange

 ******************************************************************************/

istream&
operator>>
	(
	istream&		input,
	JIndexRange&	range
	)
{
	input >> range.first >> range.last;
	return input;
}

ostream&
operator<<
	(
	ostream&			output,
	const JIndexRange&	range
	)
{
	output << range.first << ' ' << range.last;
	return output;
}

#define JTemplateType JIndexRange
#include <JArray.tmpls>
#undef JTemplateType
