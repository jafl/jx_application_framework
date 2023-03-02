/******************************************************************************
 J2DDataPoint.cpp

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "J2DDataPoint.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Stream operators for J2DDataPoint

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&	input,
	J2DDataPoint&		pt
	)
{
	input >> pt.x >> pt.xerr >> pt.xmerr >>pt.y >> pt.yerr >> pt.ymerr;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&		output,
	const J2DDataPoint&	pt
	)
{
	output << pt.x << ' ' << pt.xerr << ' ' << pt.xmerr << ' ';
	output << pt.y << ' ' << pt.yerr << ' ' << pt.ymerr;
	return output;
}
