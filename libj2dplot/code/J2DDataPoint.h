/*********************************************************************************
 J2DDataPoint.h

	Interface for the J2DDataPoint struct.

	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#ifndef _H_J2DDataPoint
#define _H_J2DDataPoint

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

struct J2DDataPoint
{
	JFloat x;
	JFloat xerr;
	JFloat xmerr;
	JFloat y;
	JFloat yerr;
	JFloat ymerr;

	J2DDataPoint()
		:
		x(0.0), xerr(0.0), xmerr(0.0),
		y(0.0), yerr(0.0), ymerr(0.0)
	{ };

	J2DDataPoint
		(
		const JFloat x1,
		const JFloat x1err,
		const JFloat x1merr,
		const JFloat y1,
		const JFloat y1err,
		const JFloat y1merr
		)
		:
		x(x1), xerr(x1err), xmerr(x1merr),
		y(y1), yerr(y1err), ymerr(y1merr)
	{ };
};

istream& operator>>(istream& input, J2DDataPoint& pt);
ostream& operator<<(ostream& output, const J2DDataPoint& pt);

#endif
