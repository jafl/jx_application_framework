/******************************************************************************
 JXCheckModTimeTask.cpp

	Idle task to periodically check mod time and broadcast if changed.

	BASE CLASS = JXIdleTask

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCheckModTimeTask.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JCharacter* JXCheckModTimeTask::kFileChanged = "FileChanged::JXCheckModTimeTask";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCheckModTimeTask::JXCheckModTimeTask
	(
	const Time			period,
	const JCharacter*	fullName
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
