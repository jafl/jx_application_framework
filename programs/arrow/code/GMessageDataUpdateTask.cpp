/******************************************************************************
 GMessageDataUpdateTask.cc

	Idle task to update a directory listing.

	BASE CLASS = JXIdleTask

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GMessageDataUpdateTask.h>
#include <GMMailboxData.h>

const Time kDirUpdateDelay = 5000;	// milliseconds

JBoolean GMessageDataUpdateTask::itsStopUpdate;

/******************************************************************************
 Constructor

 ******************************************************************************/

GMessageDataUpdateTask::GMessageDataUpdateTask
	(
	GMMailboxData* data
	)
	:
	JXIdleTask(kDirUpdateDelay)
{
	itsData = data;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMessageDataUpdateTask::~GMessageDataUpdateTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
GMessageDataUpdateTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (itsStopUpdate)
		{
		return;
		}
	if (TimeToPerform(delta, maxSleepTime))
		{
		itsData->Update();
		}
}

/******************************************************************************
 StopUpdate (static public)

 ******************************************************************************/

void
GMessageDataUpdateTask::StopUpdate
	(
	const JBoolean stop
	)
{
	itsStopUpdate	= stop;
}
