/******************************************************************************
 jTime_UNIX.cpp

	Routines to augment time.c

	Copyright (C) 2005 John Lindal.

 ******************************************************************************/

#include "jTime.h"
#include "jMath.h"
#include <time.h>
#include "jAssert.h"

/******************************************************************************
 JWait

	Waits for the specified number of seconds.

 ******************************************************************************/

inline void
jWait
	(
	const long nsec
	)
{
	timespec requested, remainder;
	requested.tv_sec  = 0;
	requested.tv_nsec = nsec;

	while (true)
	{
		const int result = nanosleep(&requested, &remainder);
		if (result == 0)
		{
			break;
		}
		requested = remainder;
	}
}

void
JWait
	(
	const double delta
	)
{
	double remaining = delta;
	while (remaining > 0.5)
	{
		jWait(500000000L);	// 0.5 seconds
		remaining -= 0.5;
	}
	jWait(JRound(remaining * 1000000000));	// remainder
}
