/******************************************************************************
 GDBPingTask.cpp

	On OSX, we have to periodically ping gdb to see if it is available,
	because it does not notify us of state changes.

	BASE CLASS = JXIdleTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "GDBPingTask.h"
#include "GDBLink.h"
#include "cmGlobals.h"

const Time kCheckInterval = 1000;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBPingTask::GDBPingTask()
	:
	JXIdleTask(kCheckInterval)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBPingTask::~GDBPingTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
GDBPingTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		dynamic_cast<GDBLink*>(CMGetLink())->SendPing();
		}
}
