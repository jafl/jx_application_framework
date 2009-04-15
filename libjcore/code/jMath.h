/******************************************************************************
 jMath.h

	Header for jMath.cc

	Copyright © 1992-99 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jMath
#define _H_jMath

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <JMinMax.h>	// for convenience
#include <math.h>

// Constants

const JFloat kJPi = 3.14159265358979323846;
const JFloat kJE  = 2.71828182845904523536;

const JFloat kJDegToRad = kJPi/180.0;
const JFloat kJRadToDeg = 180.0/kJPi;

const JFloat kJLog10ToLog2 = 1.0 / log10(2.0);

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
