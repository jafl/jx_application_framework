/******************************************************************************
 JRect.h

	Defines portable rectangle type

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JRect
#define _H_JRect

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPoint.h>
#include <jMath.h>

class JRect
{
public:

	JCoordinate top;
	JCoordinate left;
	JCoordinate bottom;
	JCoordinate right;

	JRect(const JPoint& p1, const JPoint& p2);

	JRect()
		:
		top(0), left(0), bottom(0), right(0)
	{ };

	JRect
		(
		const JCoordinate t,
		const JCoordinate l,
		const JCoordinate b,
		const JCoordinate r
		)
		:
		top(t), left(l), bottom(b), right(r)
	{ };

	JCoordinate
	width() const
	{
		return (right - left);
	};

	JCoordinate
	height() const
	{
		return (bottom - top);
	};

	JSize
	area() const
	{
		long a = (right - left)*(bottom - top);
		if (a < 0) a = -a;
		return a;
	};

	JCoordinate
	xcenter() const
	{
		return (left + right)/2;
	};

	JCoordinate
	ycenter() const
	{
		return (top + bottom)/2;
	};

	JPoint
	center() const
	{
		return JPoint(xcenter(), ycenter());
	};

	// Metrowerks <MacHeaders.c> #define's topLeft()
	#ifdef topLeft
	#undef topLeft
	#endif

	JPoint
	topLeft() const
	{
		return JPoint(left, top);
	};

	JPoint
	bottomLeft() const
	{
		return JPoint(left, bottom);
	};

	JPoint
	topRight() const
	{
		return JPoint(right, top);
	};

	JPoint
	bottomRight() const
	{
		return JPoint(right, bottom);
	};

	void
	Set
		(
		const JCoordinate t,
		const JCoordinate l,
		const JCoordinate b,
		const JCoordinate r
		)
	{
		top    = t;
		left   = l;
		bottom = b;
		right  = r;
	};

	void
	SetSize
		(
		const JCoordinate w,
		const JCoordinate h
		)
	{
		right  = left + w;
		bottom = top + h;
	};

	void
	Place
		(
		const JCoordinate t,
		const JCoordinate l
		)
	{
		const JCoordinate w = width();
		const JCoordinate h = height();
		top  = t;
		left = l;
		SetSize(w, h);
	};

	void
	Place
		(
		const JPoint& pt
		)
	{
		Place(pt.y, pt.x);
	};

	JBoolean
	IsEmpty()
		const
	{
		return JConvertToBoolean( left >= right || top >= bottom );
	};

	JBoolean
	Contains
		(
		const JCoordinate x,
		const JCoordinate y
		)
		const
	{
		return JConvertToBoolean( left <= x && x < right &&
								  top  <= y && y < bottom );
	};

	JBoolean
	Contains
		(
		const JPoint& p
		)
		const
	{
		return JConvertToBoolean( left <= p.x && p.x < right &&
								  top  <= p.y && p.y < bottom );
	};

	JBoolean
	Contains
		(
		const JRect& r
		)
		const
	{
		return JConvertToBoolean(
					left <= r.left && r.left <= r.right && r.right <= right &&
					top <= r.top && r.top <= r.bottom && r.bottom <= bottom );
	};

	void
	Shift
		(
		const JCoordinate dx,
		const JCoordinate dy
		)
	{
		top    += dy;
		bottom += dy;
		left   += dx;
		right  += dx;
	};

	void
	Shift
		(
		const JPoint delta
		)
	{
		top    += delta.y;
		bottom += delta.y;
		left   += delta.x;
		right  += delta.x;
	};

	void
	Shrink
		(
		const JCoordinate dx,
		const JCoordinate dy
		)
	{
		top    += dy;
		bottom -= dy;
		left   += dx;
		right  -= dx;
	};

	void
	Expand
		(
		const JCoordinate dx,
		const JCoordinate dy
		)
	{
		top    -= dy;
		bottom += dy;
		left   -= dx;
		right  += dx;
	};
};

JBoolean JIntersection(const JRect& r1, const JRect& r2, JRect* result);
JRect    JCovering(const JRect& r1, const JRect& r2);

istream& operator>>(istream& input, JRect& r);
ostream& operator<<(ostream& output, const JRect& r);

inline int
operator==
	(
	const JRect& r1,
	const JRect& r2
	)
{
	return (r1.top  == r2.top  && r1.bottom == r2.bottom &&
			r1.left == r2.left && r1.right  == r2.right);
}

inline int
operator!=
	(
	const JRect& r1,
	const JRect& r2
	)
{
	return (r1.top  != r2.top  || r1.bottom != r2.bottom ||
			r1.left != r2.left || r1.right  != r2.right);
}

inline JRect
operator+
	(
	const JRect&  r,
	const JPoint& p
	)
{
	return JRect(r.top    + p.y, r.left  + p.x,
				 r.bottom + p.y, r.right + p.x);
}

inline JRect
operator-
	(
	const JRect&  r,
	const JPoint& p
	)
{
	return JRect(r.top    - p.y, r.left  - p.x,
				 r.bottom - p.y, r.right - p.x);
}

#endif
