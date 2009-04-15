/******************************************************************************
 jTime_UNIX.cpp

	Routines to augment time.c

	Copyright © 2005 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jTime.h>
#include <jMath.h>
#include <unistd.h>
#include <jAssert.h>

/******************************************************************************
 JWait

	Waits for the specified number of seconds.

 ******************************************************************************/

void
JWait
	(
	const double delta
	)
{
	// some systems don't want the argument of usleep() to exceed 1 second

	double remaining = delta;
	while (remaining > 0.5)
		{
		usleep(500000UL);					// 0.5 seconds
		remaining -= 0.5;
		}
	usleep((unsigned long) JRound(remaining * 1000000));	// remainder
}
