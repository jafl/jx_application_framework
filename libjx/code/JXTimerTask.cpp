/******************************************************************************
 JXTimerTask.cpp

	Idle task to periodically broadcast.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "JXTimerTask.h"
#include <jx-af/jcore/jAssert.h>

const JUtf8Byte* JXTimerTask::kTimerWentOff = "TimerWentOff::JXTimerTask";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTimerTask::JXTimerTask
	(
	const Time	period,
	const bool	oneShot
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
 Perform (virtual protected)

 ******************************************************************************/

void
JXTimerTask::Perform
	(
	const Time delta
	)
{
	Broadcast(TimerWentOff());
	if (itsIsOneShotFlag)
	{
		jdelete this;
	}
}
