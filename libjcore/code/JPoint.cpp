/******************************************************************************
 JPoint.cpp

	System independent routines for dealing with points

	The coordinate system is the standard computer graphics system:
	  +---> x
	  |
	  y

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPoint.h>
#include <JRect.h>
#include <JMinMax.h>
#include <jAssert.h>

/******************************************************************************
 JPinInRect

	Shift the given point as little as possible to put it inside the given rect.

 ******************************************************************************/

JPoint
JPinInRect
	(
	const JPoint&	pt,
	const JRect&	r
	)
{
	JPoint newPt = pt;

	newPt.x = JMax(newPt.x, r.left);
	newPt.x = JMin(newPt.x, (JCoordinate) (r.right-1));

	newPt.y = JMax(newPt.y, r.top);
	newPt.y = JMin(newPt.y, (JCoordinate) (r.bottom-1));

	return newPt;
}

/******************************************************************************
 Stream operators for JPoint

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	JPoint&		pt
	)
{
	input >> pt.x >> pt.y;
	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const JPoint&	pt
	)
{
	output << pt.x << ' ' << pt.y;
	return output;
}

#define JTemplateType JPoint
#include <JArray.tmpls>
#undef JTemplateType
