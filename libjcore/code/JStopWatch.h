/******************************************************************************
 JStopWatch.h

	Interface for the JStopWatch class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_StopWatch
#define _H_StopWatch

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <jTime.h>

class JString;

class JStopWatch
{
public:

	JStopWatch();

	~JStopWatch();

	void	StartTimer();
	void	StopTimer();

	JFloat	GetCPUTimeInterval();
	JFloat	GetUserTimeInterval();
	JString	PrintTimeInterval();

private:

	JBoolean	itsOnFlag;		// kJTrue if stopwatch is running
	clock_t		itsStartClock;	// clock when we were started
	clock_t		itsStopClock;	// clock when we were stopped
	time_t		itsStartTime;	// time when we were started
	time_t		itsStopTime;	// time when we were stopped

private:

	JString	FormatTimeInterval(const JFloat time);
};

#endif
