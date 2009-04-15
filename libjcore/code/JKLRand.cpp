/******************************************************************************
 JKLRand.cpp

	A random-number generator class based on the JKLRand generator function.  A
	class is a natural way to maintain the seed and has no special implications
	in a multi-threaded or re-entrant environment.  Further, the absence of
	virtual methods (except for the destructor, as discussed below) means that
	there is virtually performance penalty in using a class method as opposed to
	a function call.  (Well, technically there may be a performance penalty for
	de-referencing the seed when this would otherwise not be necessary.  If this
	is a problem, you should probably be hand-coding in assembly anyway.)

	It can be quite useful to subclass JKLRand, for example to add methods to
	generate non-uniformly distributed variables.  Accordingly, JKLRand has a
	virtual destructor even though it does nothing.  Users are unlikely to
	create thousands of separate generators, so the performance penalty involved
	should not be a problem (assuming that the compiler does not simply optimize
	away the destructor calls in the first place).

	Design and Performance:

	Because random numbers are often generated inside tight inner loops, more
	attention than usual should be paid to performance in their generation.  This
	is doubly true for JKLRand; since this class uses the fastest implementation
	that is likely to be available, it will be the generator of choice.

	Implementation:

	The JKLRand class uses the JKLRandInt function to generate its values.  The
	price of speed is that this	function has all the usual defects of simple
	linear congruential generators.  See the notes in jRand.cc, and make sure you
	understand how to use it properly.

	Copyright © 1997 Dustin Laurence.  All rights reserved.

	Notes:

		Could also provide PeekRand... functions which return the next value
		but do not update the seed.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JKLRand.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	The default constructor generates an initial seed based on time() in
	exactly the same manner as the SetSeedByAbsoluteTime method.  Failure
	(see the SetSeedByAbsoluteTime documentation) can be tested for with
	GetSeed().

 *****************************************************************************/

JKLRand::JKLRand()
	:
	itsSeed(0)
{
	SetSeedByAbsoluteTime();
}

JKLRand::JKLRand
	(
	const JInt32 seed
	)
	:
	itsSeed(seed)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JKLRand::~JKLRand()
{
}

/******************************************************************************
 UniformLong

	Returns an integer value between lim1 and lim2 inclusive.
	The limits may be in either order.

 ******************************************************************************/

long
JKLRand::UniformLong
	(
	const long lim1,
	const long lim2
	)
	const
{
	long max,min;

	if (lim1 <= lim2)
		{
		min = lim1;
		max = lim2;
		}
	else
		{
		min = lim2;
		max = lim1;
		}

	return JLFloor(UniformSemiOpenProb()*(max-min + 1) + min);
}

/******************************************************************************
 UniformULong

	Returns an integer value between lim1 and lim2 inclusive.
	The limits may be in either order.

 ******************************************************************************/

unsigned long
JKLRand::UniformULong
	(
	const unsigned long lim1,
	const unsigned long lim2
	)
	const
{
	unsigned long max,min;

	if (lim1 <= lim2)
		{
		min = lim1;
		max = lim2;
		}
	else
		{
		min = lim2;
		max = lim1;
		}

	// truncation is automatic

	return static_cast<unsigned long>(UniformSemiOpenProb()*(max-min + 1) + min);
}

/******************************************************************************
 UniformDouble

	Returns a floating-point value between lim1 and lim2 inclusive.
	The limits may be in either order.

 ******************************************************************************/

double
JKLRand::UniformDouble
	(
	const double lim1,
	const double lim2
	)
	const
{
	double	max,min;

	if (lim1 <= lim2)
		{
		min = lim1;
		max = lim2;
		}
	else
		{
		min = lim2;
		max = lim1;
		}

	double val = UniformClosedProb()*(max-min) + min;

	// Must ensure we don't round to outside the range
	if (val < min)
		{
		return min;
		}
	else if (val > max)
		{
		return max;
		}
	else
		{
		return val;
		}
}
