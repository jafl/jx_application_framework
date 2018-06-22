/******************************************************************************
 JPoint.cpp

	System independent routines for dealing with points

	The coordinate system is the standard computer graphics system:
	  +---> x
	  |
	  y

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JPoint.h"
#include "JRect.h"
#include "JMinMax.h"
#include "jAssert.h"

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

std::istream&
operator>>
	(
	std::istream&	input,
	JPoint&		pt
	)
{
	input >> pt.x >> pt.y;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&		output,
	const JPoint&	pt
	)
{
	output << pt.x << ' ' << pt.y;
	return output;
}
