/******************************************************************************
 JXCheckModTimeTask.cpp

	Idle task to periodically check mod time and broadcast if changed.

	BASE CLASS = JXIdleTask

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXCheckModTimeTask.h"
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JUtf8Byte* JXCheckModTimeTask::kFileChanged = "FileChanged::JXCheckModTimeTask";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCheckModTimeTask::JXCheckModTimeTask
	(
	const Time		period,
	const JString&	fullName
	)
	:
	JXIdleTask(period),
	itsFullName(fullName)
{
	UpdateModTime();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCheckModTimeTask::~JXCheckModTimeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXCheckModTimeTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
	{
		time_t t;
		if (JGetModificationTime(itsFullName, &t) == kJNoError &&
			t != itsModTime)
		{
			Broadcast(FileChanged());
		}
		itsModTime = t;		// set to zero if file doesn't exist, so bcast when created
	}
}

/******************************************************************************
 UpdateModTime

	Useful for when you change the file.

 ******************************************************************************/

void
JXCheckModTimeTask::UpdateModTime()
{
	JGetModificationTime(itsFullName, &itsModTime);		// auto zero if doesn't exist
}
