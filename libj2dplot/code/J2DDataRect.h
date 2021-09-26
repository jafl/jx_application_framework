/*********************************************************************************
 J2DDataRect.h

	Copyright (C) 1999 John Lindal.

 ********************************************************************************/

#ifndef _H_J2DDataRect
#define _H_J2DDataRect

#include <jx-af/jcore/jTypes.h>

struct J2DDataRect
{
	JFloat xmin, xmax, ymin, ymax;

	J2DDataRect()
		:
		xmin(0.0), xmax(0.0), ymin(0.0), ymax(0.0)
	{ };

	J2DDataRect(const JFloat xn, const JFloat xx,
				const JFloat yn, const JFloat yx)
		:
		xmin(xn), xmax(xx), ymin(yn), ymax(yx)
	{ };

	bool
	Contains(const JFloat x, const JFloat y) const
	{
		return xmin <= x && x <= xmax &&
					ymin <= y && y <= ymax;
	};
};

#endif
