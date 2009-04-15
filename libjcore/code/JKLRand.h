#ifndef _H_JKLRand
#define _H_JKLRand

/******************************************************************************
 JKLRand.h

    Interface for the JKLRand class.

	Copyright © 1997 Dustin Laurence. All rights reserved.

 ******************************************************************************/

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <time.h>

#include <jTypes.h>
#include <jRand.h>

	// For avoiding endpoints; probably should have a more sophisticated system
	// someday.
	const double jRandEpsilon = 1.2e-7;
	const double jMaxProb = 1.0 - jRandEpsilon;


class JKLRand
{
public:

	JKLRand();
	JKLRand(const JInt32 seed);

	virtual ~JKLRand();

	// Accept default copy constructor and assignment operator
	// JKLRand(const JKLRand& source);
	// const JKLRand& operator=(const JKLRand& source);

// random ints
	JInt32  UniformInt32() const;
	JUInt32 UniformUInt32() const;

	long          UniformLong(const long lim1, const long lim2) const;

	unsigned long UniformULong(const unsigned long lim1, const unsigned long lim2) const;

// random floats
	double UniformClosedProb() const;
	double UniformSemiOpenProb() const;
	// A default with a shorter name
	double UniformProb() const;

	double UniformDouble(const double lim1, const double lim2) const;

// Methods to manipulate the seed
	JInt32 GetSeed() const;
	void   SetSeed(const JInt32 newSeed);
	JInt32 SetSeedByAbsoluteTime();

private:

	JInt32 itsSeed;
};

/******************************************************************************
 UniformInt32

	Returns a random 32-bit integer in the range [0, 2^32-1], converted to a
	signed quantity.

 ******************************************************************************/

inline JInt32
JKLRand::UniformInt32() const
{
	const_cast<JKLRand*>(this)->itsSeed = JKLRandInt32(itsSeed);
	return itsSeed;
}

/******************************************************************************
 UniformUInt32

	Returns a random 32-bit integer in the range [0, 2^32-1].  This alternative
	to UniformInt32 is provided so that it is as easy as possible to avoid
	'comparison between signed and unsigned' warnings.

 ******************************************************************************/

inline JUInt32
JKLRand::UniformUInt32() const
{
	// Note that the considerations discussed in jTypes.h means that this
	// must be implemented in terms of UniformInt32 rather than the reverse.
	return UniformInt32();
}

/******************************************************************************
 UniformClosedProb

	Returns a random double in the range [0.0, 1.0].  (For the non-mathematically
	inclined this means that 0.0 and 1.0 are possible return values.)

	This version is provided for maximum speed when coding a tight inner loop;
	in numerical work the extra test in UniformSemiOpenProb could conceivably
	affect performance.

 ******************************************************************************/

inline double
JKLRand::UniformClosedProb() const
{
	return (1.0/0xFFFFFFFFL) * UniformUInt32();
}

/******************************************************************************
 UniformSemiOpenProb

	Returns a random double in the range [0.0, 1.0).  (For the non-mathematically
	inclined this means that 0.0 is a possible return value but 1.0 is not.)

	This is the standard uniform deviate function because people often write
	code which assumes 1.0 will not be returned.

 ******************************************************************************/

inline double
JKLRand::UniformSemiOpenProb() const
{
	const double rawVal = UniformClosedProb();
	if (rawVal >= jMaxProb)
		{
		return jMaxProb;
		}
	else
		{
		return rawVal;
		}
}

/******************************************************************************
 UniformProb

	The default random floating point number method, UniformProb simply calls
	UniformSemiOpenProb().  This is the standard uniform deviate function
	because people often carelessly write code which assumes 1.0 will not be
	returned.  If you need maximum performance, I assume you will pay attention
	to the proper method to call.

 ******************************************************************************/

inline double
JKLRand::UniformProb() const
{
	return UniformSemiOpenProb();
}

/******************************************************************************
 GetSeed

	Sets the random number seed to the given value.

 ******************************************************************************/

inline JInt32
JKLRand::GetSeed() const
{
	return itsSeed;
}

/******************************************************************************
 SetSeed

	Sets the random number seed to the given value.

 ******************************************************************************/

inline void
JKLRand::SetSeed
	(
	const JInt32 newSeed
	)
{
	itsSeed = newSeed;
}

/******************************************************************************
 SetSeedByAbsoluteTime

	Sets the generator seed to time(NULL), returning the new seed for
	convenience in testing for failure (see below).

	According to ANSI it is possible for time() to fail, which sets the seed to
	-1.  This error can be tested for with GetSeed as well as the return value.
	For all but the most extreme portability this can probably be ignored, since
	virtually all systems provide a realtime clock and on such systems it is
	unlikely that time() can fail.

 ******************************************************************************/

inline JInt32
JKLRand::SetSeedByAbsoluteTime()
{
	itsSeed = time((time_t*) NULL);		// egcs is -so- picky!
	return itsSeed;
}

#endif
