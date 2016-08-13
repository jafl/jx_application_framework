/******************************************************************************
 jDeriv.cpp

	Numerical differentiation routines.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jDeriv.h>

/******************************************************************************
 Centered Difference for first derivative

 ******************************************************************************/

JFloat
JCenterDiff2
	(
	const JFloat x,
	const JFloat dx,
	JFloat (fn)(const JFloat)
	)
{
	return (fn(x+dx) - fn(x-dx))/(2.0*dx);
}

JFloat
JCenterDiff4
	(
	const JFloat x,
	const JFloat dx,
	JFloat (fn)(const JFloat)
	)
{
	return (-fn(x+2.0*dx) + 8.0*fn(x+dx) - 8.0*fn(x-dx) + fn(x-2.0*dx))/(12.0*dx);
}

JFloat
JCenterDiff6
	(
	const JFloat x,
	const JFloat dx,
	JFloat (fn)(const JFloat)
	)
{
	return (fn(x+3.0*dx) - 9.0*fn(x+2.0*dx) + 45.0*fn(x+dx)
			- 45.0*fn(x-dx) + 9.0*fn(x-2.0*dx) - fn(x-3.0*dx))/(60.0*dx);
}
