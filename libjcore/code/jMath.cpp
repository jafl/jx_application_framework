/******************************************************************************
 jMath.cpp

	Assembled floating point routines.

	Copyright (C) 1992-99 John Lindal.

 ******************************************************************************/

#include "jMath.h"
#include <limits.h>
#include "jErrno.h"
#include "jAssert.h"

/******************************************************************************
 JRound

	Returns long int nearest to x.  Correctly truncates values to the allowed
	range for long ints.

 ******************************************************************************/

long
JRound
	(
	const double x
	)
{
double x1,d1,x2,d2;

	x1 = floor(x);	d1 = x-x1;
	x2 = ceil(x);	d2 = x2-x;

	if (x1 < LONG_MIN) x1 = LONG_MIN;
	if (x1 > LONG_MAX) x1 = LONG_MAX;

	if (x2 < LONG_MIN) x2 = LONG_MIN;
	if (x2 > LONG_MAX) x2 = LONG_MAX;

	if (d2 <= d1)
		{
		return (long) x2;
		}
	else
		{
		return (long) x1;
		}
}

/******************************************************************************
 JSign

	Returns +1, 0, -1 corresponding to the sign of the argument.

 ******************************************************************************/

int
JSign
	(
	const double x
	)
{
	if (x < 0.0)
		{
		return -1;
		}
	else if (x > 0.0)
		{
		return +1;
		}
	else
		{
		return 0;
		}
}

/******************************************************************************
 JASinh

	Inverse hyperbolic sine.

 ******************************************************************************/

double
JASinh
	(
	const double x
	)
{
	return log(x + sqrt(x*x + 1));
}

/******************************************************************************
 JACosh

	Principal value of the inverse hyperbolic cosine.

 ******************************************************************************/

double
JACosh
	(
	const double x
	)
{
	if (x >= 1.0)
		{
		return log(x + sqrt(x*x - 1));
		}
	else
		{
		jset_errno(EDOM);
		return 0.0;
		}
}

/******************************************************************************
 JATanh

	Inverse hyperbolic tangent.

 ******************************************************************************/

double
JATanh
	(
	const double x
	)
{
	if (fabs(x) < 1.0)
		{
		return 0.5 * log((1 + x) / (1 - x));
		}
	else
		{
		jset_errno(EDOM);
		return 0.0;
		}
}
