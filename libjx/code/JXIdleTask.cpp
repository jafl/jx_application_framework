/******************************************************************************
 JXIdleTask.cpp

	Idle tasks are performed when there are no other important events
	to process.  If an idle task needs to be performed at fixed intervals,
	it can change the value of *maxSleepTime to the longest interval in
	milliseconds that it is willing to wait.  The system will then try to
	call it again within this time interval.  There are no guarantees,
	however.  It could be called again much sooner or much later.

	Remember that your program will be a CPU hog if you set *maxSleepTime
	to a very small value.  Executing 10 times per second is probably
	a reasonable limit.  (*maxSleepTime = 100)

	maxSleepTime is passed in as a pointer so tasks that don't care about
	the time interval can ignore it.

	delta indicates the amount of time in milliseconds that has elapsed
	since the last call.  This can be used to ensure that a task is not
	performed too often.

	Since most tasks will run at regular intervals, we provide
	Ready() to handle this case.  The constructor asks for the time
	between runs, and Ready() returns true whenever the elapsed time
	exceeds the period.  It also sets maxSleepTime to the period,
	for convenience.

	If your task does not operate with a constant period, then simply
	pass in zero for the period.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXIdleTask.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXIdleTask::JXIdleTask
	(
	const Time period
	)
	:
	itsPeriod(period),
	itsElapsedTime(0)
{
	assert( JXApplication::IsUIThread() );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXIdleTask::~JXIdleTask()
{
	assert( JXApplication::IsUIThread() );
	Stop();
}

/******************************************************************************
 Start

 ******************************************************************************/

void
JXIdleTask::Start()
{
	JXGetApplication()->InstallIdleTask(this);
}

/******************************************************************************
 Stop

 ******************************************************************************/

void
JXIdleTask::Stop()
{
	JXGetApplication()->RemoveIdleTask(this);
}

/******************************************************************************
 Ready

	Returns true if it is time to run the task again.

 ******************************************************************************/

bool
JXIdleTask::Ready
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	itsElapsedTime  += delta;
	const bool ready = itsElapsedTime >= itsPeriod;
	if (ready)
	{
		ResetTimer();
	}

	if (itsPeriod > 0)
	{
		*maxSleepTime = itsPeriod - itsElapsedTime;
	}
	return ready;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JXIdleTask::ToString()
	const
{
	return JString((JUInt64)(void*)this, JString::kBase16);
}
