/******************************************************************************
 JRect.cpp

	System independent routines for dealing with rectangles

	The coordinate system is the standard computer graphics system:
	  +---> x
	  |
	  y

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JRect.h>
#include <JMinMax.h>
#include <jAssert.h>

/******************************************************************************
 JIntersection

	Returns kJTrue if the intersection of r1 and r2 is not empty.
	It is safe for r1 or r2 to also be the result.

 ******************************************************************************/

JBoolean
JIntersection
	(
	const JRect&	r1,
	const JRect&	r2,
	JRect*			result
	)
{
	result->top    = JMax(r1.top,    r2.top);
	result->left   = JMax(r1.left,   r2.left);
	result->bottom = JMin(r1.bottom, r2.bottom);
	result->right  = JMin(r1.right,  r2.right);

	if (result->top < result->bottom && result->left < result->right)
		{
		return kJTrue;
		}
	else
		{
		*result = JRect(0,0,0,0);
		return kJFalse;
		}
}

/******************************************************************************
 JCovering

	Returns the smallest rectangle enclosing the two given rectangles.

 ******************************************************************************/

JRect
JCovering
	(
	const JRect& r1,
	const JRect& r2
	)
{
	JRect result;
	result.top    = JMin(r1.top,    r2.top);
	result.left   = JMin(r1.left,   r2.left);
	result.bottom = JMax(r1.bottom, r2.bottom);
	result.right  = JMax(r1.right,  r2.right);
	return result;
}

/******************************************************************************
 Routines for JRects

 ******************************************************************************/

JRect::JRect
	(
	const JPoint& p1,
	const JPoint& p2
	)
	:
	top    ( JMin(p1.y, p2.y)   ),
	left   ( JMin(p1.x, p2.x)   ),
	bottom ( JMax(p1.y, p2.y)+1 ),
	right  ( JMax(p1.x, p2.x)+1 )
{
}

istream&
operator>>
	(
	istream&	input,
	JRect&		r
	)
{
	input >> r.top >> r.left >> r.bottom >> r.right;
	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const JRect&	r
	)
{
	output << r.top << ' ' << r.left << ' ' << r.bottom << ' ' << r.right;
	return output;
}

#define JTemplateType JRect
#include <JArray.tmpls>
#undef JTemplateType
