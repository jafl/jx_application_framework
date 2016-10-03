/******************************************************************************
 GMessageDirUpdateTask.cc

	Idle task to update a directory listing.

	BASE CLASS = JXIdleTask

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GMessageDirUpdateTask.h>
#include <GMessageTableDir.h>

const Time kDirUpdateDelay = 5000;	// milliseconds

JBoolean GMessageDirUpdateTask::itsStopUpdate;

/******************************************************************************
 Constructor

 ******************************************************************************/

GMessageDirUpdateTask::GMessageDirUpdateTask
	(
	GMessageTableDir* dir
	)
	:
	JXIdleTask(kDirUpdateDelay)
{
	itsDir = dir;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMessageDirUpdateTask::~GMessageDirUpdateTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
GMessageDirUpdateTask::Perform
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
		itsDir->Update();
		}
}

/******************************************************************************
 StopUpdate (static public)

 ******************************************************************************/

void
GMessageDirUpdateTask::StopUpdate
	(
	const JBoolean stop
	)
{
	itsStopUpdate	= stop;
}
