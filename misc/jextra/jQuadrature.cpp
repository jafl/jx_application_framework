/******************************************************************************
 jQuadrature.cpp

	Numerical quadrature routines.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jQuadrature.h>
#include <JIndex.h>

/******************************************************************************
 Simpson's method

 ******************************************************************************/

JFloat
JQuadratureSimpson
	(
	const JFloat	xmin,
	const JFloat	xmax,
	const JSize		intCount,
	JFloat (fn)(const JFloat)
	)
{
	const JFloat dx = (xmax - xmin)/(2.0*intCount);
	JFloat s = fn(xmin) + 4.0*fn(xmin+dx);
	for (JIndex i=1; i<intCount; i++)
		{
		const JFloat x = xmin + 2.0*i*dx;
		s += 2.0*fn(x) + 4.0*fn(x+dx);
		}
	s += fn(xmax);
	return s*dx/3.0;
}
