/******************************************************************************
 GNBArrayFlushTask.cc

	Idle task to periodically broadcast.

	BASE CLASS = JXIdleTask

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GNBArrayFlushTask.h>
#include <GNBApp.h>

#include <jAssert.h>

const Time kSafetySavePeriod = 30000;	// 30 seconds (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

GNBArrayFlushTask::GNBArrayFlushTask
	(
	GNBApp* app
	)
	:
	JXIdleTask(kSafetySavePeriod),
	itsApp(app)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GNBArrayFlushTask::~GNBArrayFlushTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
GNBArrayFlushTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{		
//		itsApp->Flush(kJTrue);
		delete this;
		}
}
