/******************************************************************************
 JStopWatch.h

	Interface for the JStopWatch class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_StopWatch
#define _H_StopWatch

#include "jTypes.h"
#include "jTime.h"

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

	bool	itsOnFlag;		// true if stopwatch is running
	clock_t	itsStartClock;	// clock when we were started
	clock_t	itsStopClock;	// clock when we were stopped
	time_t	itsStartTime;	// time when we were started
	time_t	itsStopTime;	// time when we were stopped

private:

	JString	FormatTimeInterval(const JFloat time);
};

#endif
