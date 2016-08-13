/******************************************************************************
 J2DDataPoint.cpp

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <J2DDataPoint.h>
#include <jAssert.h>

/******************************************************************************
 Stream operators for J2DDataPoint

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	J2DDataPoint&		pt
	)
{
	input >> pt.x >> pt.xerr >> pt.xmerr >>pt.y >> pt.yerr >> pt.ymerr;
	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const J2DDataPoint&	pt
	)
{
	output << pt.x << ' ' << pt.xerr << ' ' << pt.xmerr << ' ';
	output << pt.y << ' ' << pt.yerr << ' ' << pt.ymerr;
	return output;
}
