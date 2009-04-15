/******************************************************************************
 JStopWatch.cpp

	Class for timing an operation.

	Typical usage:

		#include <JStopWatch.h>
		...
		JStopWatch timer;
		timer.StartTimer();
		...
		timer.StopTimer();
		JString duration = timer.PrintTimeInterval();

	This class was not designed to be a base class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JStopWatch.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JStopWatch::JStopWatch()
{
	itsOnFlag = kJFalse;
	itsStartClock = itsStopClock = 0;
	itsStartTime  = itsStopTime  = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStopWatch::~JStopWatch()
{
}

/******************************************************************************
 StartTimer

 ******************************************************************************/

void
JStopWatch::StartTimer()
{
	if (!itsOnFlag)
		{
		itsOnFlag = kJTrue;
		itsStartClock = itsStopClock = clock();
		itsStartTime  = itsStopTime  = time(NULL);
		}
}

/******************************************************************************
 StopTimer

 ******************************************************************************/

void
JStopWatch::StopTimer()
{
	if (itsOnFlag)
		{
		itsOnFlag = kJFalse;
		itsStopClock = clock();
		itsStopTime  = time(NULL);
		}
}

/******************************************************************************
 GetCPUTimeInterval

	Return the time interval in seconds from clock.

	If we are still running, then we return the amount of time since we started.

 ******************************************************************************/

JFloat
JStopWatch::GetCPUTimeInterval()
{
	clock_t stopClock;
	if (itsOnFlag)
		{
		stopClock = clock();
		}
	else
		{
		stopClock = itsStopClock;
		}

	return ((JFloat) (stopClock - itsStartClock) / (JFloat) CLOCKS_PER_SEC);
}

/******************************************************************************
 GetUserTimeInterval

	Return the time interval in seconds from time.

	If we are still running, then we return the amount of time since we started.

 ******************************************************************************/

JFloat
JStopWatch::GetUserTimeInterval()
{
	time_t stopTime;
	if (itsOnFlag)
		{
		stopTime  = time(NULL);
		}
	else
		{
		stopTime  = itsStopTime;
		}

	return (JFloat) (stopTime - itsStartTime);
}

/******************************************************************************
 PrintTimeInterval

	Return the time interval as a text string:  "<cpu time>; <user time>"

 ******************************************************************************/

JString
JStopWatch::PrintTimeInterval()
{
	const JString timeString =
		FormatTimeInterval(GetCPUTimeInterval()) +
		"; " +
		FormatTimeInterval(GetUserTimeInterval());
	return timeString;
}

/******************************************************************************
 FormatTimeInterval (private)

	Return the time interval as a text string.

 ******************************************************************************/

JString
JStopWatch::FormatTimeInterval
	(
	const JFloat time
	)
{
	JString timeString;
	if (time < 60.0)									// less than 1 min
		{
		timeString = JString(time) + " seconds";
		}
	else if (time < 3600.0)								// less than 1 hour
		{
		timeString = JString(time/60.0) + " minutes";
		}
	else if (time < 86400.0)							// less than 1 day
		{
		timeString = JString(time/3600.0) + " hours";
		}
	else
		{
		timeString = JString(time/86400.0) + " days";
		}

	return timeString;
}
