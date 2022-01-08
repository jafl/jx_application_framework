#ifndef _H_jRand
#define _H_jRand

/******************************************************************************
 jRand.h

	Interface for the jRand library.

	Copyright (C) 1997 by Dustin Laurence.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "jx-af/jcore/jTypes.h"

/******************************************************************************
 JKLRandInt32

 ******************************************************************************/

inline JInt32
JKLRandInt32
	(
	const JInt32 seed
	)
{
	return 1664525L*seed + 1013904223L;
}

/******************************************************************************
 JKHRandInt64

 ******************************************************************************/

inline JInt64
JKHRandInt64
	(
	const JInt64 seed
	)
{
	return 6364136223846793005LL*seed + 1LL;
}

/******************************************************************************
 JRandWord

 ******************************************************************************/

inline JWord
JRandWord
	(
	const JWord seed
	)
{
	return sizeof(JWord) == 8 ? JWord(JKHRandInt64(seed)) : JKLRandInt32(seed);
}

#endif
