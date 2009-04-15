/******************************************************************************
 JXTimerTask.cpp

	Idle task to periodically broadcast.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTimerTask.h>
#include <jAssert.h>

const JCharacter* JXTimerTask::kTimerWentOff = "TimerWentOff::JXTimerTask";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTimerTask::JXTimerTask
	(
	const Time		period,
	const JBoolean	oneShot
	)
	:
	JXIdleTask(period),
	itsIsOneShotFlag(oneShot)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTimerTask::~JXTimerTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXTimerTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		Broadcast(TimerWentOff());
		if (itsIsOneShotFlag)
			{
			delete this;
			}
		}
}
