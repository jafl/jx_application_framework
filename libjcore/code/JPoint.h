/******************************************************************************
 JPoint.h

	Defines portable point type

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPoint
#define _H_JPoint

#include "jMath.h"

class JRect;

class JPoint
{
public:

	JCoordinate x;
	JCoordinate y;

	JPoint()
		:
		x(0), y(0)
	{ };

	JPoint
		(
		const JCoordinate x1,
		const JCoordinate y1
		)
		:
		x(x1), y(y1)
	{ };

	void
	Set
		(
		const JCoordinate x1,
		const JCoordinate y1
		)
	{
		x = x1;
		y = y1;
	};

	JPoint&
	operator+=
		(
		const JPoint& p
		)
	{
		x += p.x;
		y += p.y;
		return *this;
	};

	JPoint&
	operator-=
		(
		const JPoint& p
		)
	{
		x -= p.x;
		y -= p.y;
		return *this;
	};

	JPoint&
	operator*=
		(
		const JFloat s
		)
	{
		x = JRound(s*x);
		y = JRound(s*y);
		return *this;
	};

	JPoint&
	operator/=
		(
		const JFloat s
		)
	{
		x = JRound(x/s);
		y = JRound(y/s);
		return *this;
	};

	JPoint
	operator-()
		const
	{
		return JPoint(-x, -y);
	};
};

std::istream& operator>>(std::istream& input, JPoint& pt);
std::ostream& operator<<(std::ostream& output, const JPoint& pt);

JPoint JPinInRect(const JPoint& pt, const JRect& r);

inline bool
operator==
	(
	const JPoint& p1,
	const JPoint& p2
	)
{
	return (p1.x == p2.x && p1.y == p2.y);
}

inline JPoint
operator+
	(
	const JPoint& p1,
	const JPoint& p2
	)
{
	return JPoint(p1.x + p2.x, p1.y + p2.y);
}

inline JPoint
operator-
	(
	const JPoint& p1,
	const JPoint& p2
	)
{
	return JPoint(p1.x - p2.x, p1.y - p2.y);
}

inline JPoint
operator*
	(
	const JPoint&	p,
	const JFloat	s
	)
{
	return JPoint(JRound(s*p.x), JRound(s*p.y));
}

inline JPoint
operator*
	(
	const JFloat	s,
	const JPoint&	p
	)
{
	return JPoint(JRound(s*p.x), JRound(s*p.y));
}

inline JPoint
operator/
	(
	const JPoint&	p,
	const JFloat	s
	)
{
	return JPoint(JRound(p.x/s), JRound(p.y/s));
}

#endif
