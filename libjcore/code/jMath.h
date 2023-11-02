/******************************************************************************
 jMath.h

	Copyright (C) 1992-99 John Lindal.

 ******************************************************************************/

#ifndef _H_jMath
#define _H_jMath

#include "jTypes.h"
#include "JMinMax.h"	// for convenience
#include <math.h>
#include <numbers>

// Constants

constexpr JFloat kJDegToRad = std::numbers::pi / 180.0;
constexpr JFloat kJRadToDeg = 180.0 / std::numbers::pi;

// Functions

long JRound(const double);
int  JSign(const double);

double	JASinh(const double);
double	JACosh(const double);
double	JATanh(const double);

inline long
JLFloor
	(
	const double x
	)
{
	return JRound( floor(x) );
}

inline long
JLCeil
	(
	const double x
	)
{
	return JRound( ceil(x) );
}

inline long
JLAbs
	(
	const long x
	)
{
	return (x >= 0 ? x : -x);
}

inline long
JTruncate
	(
	const double x
	)
{
	return (x >= 0.0 ? JLFloor(x) : JLCeil(x));
}

#endif
