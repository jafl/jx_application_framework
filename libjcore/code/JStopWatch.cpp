/******************************************************************************
 JStopWatch.cpp

	Class for timing an operation.

	Typical usage:

		#include "JStopWatch.h"
		...
		JStopWatch timer;
		timer.StartTimer();
		...
		timer.StopTimer();
		JString duration = timer.PrintTimeInterval();

	This class was not designed to be a base class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#include "JStopWatch.h"
#include "JString.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JStopWatch::JStopWatch()
{
	itsOnFlag = false;
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
		itsOnFlag     = true;
		itsStartClock = itsStopClock = clock();
		itsStartTime  = itsStopTime  = time(nullptr);
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
		itsOnFlag    = false;
		itsStopClock = clock();
		itsStopTime  = time(nullptr);
	}
}

/******************************************************************************
 GetCPUTimeInterval

	Return the time interval in seconds from clock.

	If we are still running, then we return the amount of time since we started.

 ******************************************************************************/

JFloat
JStopWatch::GetCPUTimeInterval()
	const
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
	const
{
	time_t stopTime;
	if (itsOnFlag)
	{
		stopTime  = time(nullptr);
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
	const
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
	const
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
