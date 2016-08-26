/*********************************************************************************
 J2DVectorPoint.h

	Interface for the J2DVectorPoint struct.

	Copyright @ 1998 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#ifndef _H_J2DVectorPoint
#define _H_J2DVectorPoint

#include <jMath.h>

struct J2DVectorPoint
{
	JFloat x, y;	// position of tail
	JFloat vx, vy;	// length

	J2DVectorPoint()
		:
		x(0.0), y(0.0), vx(0.0), vy(0.0)
	{ };

	J2DVectorPoint
		(
		const JFloat x1,
		const JFloat y1,
		const JFloat vx1,
		const JFloat vy1
		)
		:
		x(x1), y(y1), vx(vx1), vy(vy1)
	{ };

	void
	GetHead(JFloat* hx, JFloat* hy) const
	{
		*hx = x + vx;
		*hy = y + vy;
	};

	void
	GetTail(JFloat* tx, JFloat* ty) const
	{
		*tx = x;
		*ty = y;
	};

	JFloat
	GetLength() const
	{
		return sqrt(vx*vx + vy*vy);
	};
};

#endif
