/******************************************************************************
 SVNBusyTabTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "SVNBusyTabTask.h"
#include "SVNTabGroup.h"
#include <jAssert.h>

const JSize kAnimationPeriod = 100;		// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNBusyTabTask::SVNBusyTabTask
	(
	SVNTabGroup* tabGroup
	)
	:
	JXIdleTask(kAnimationPeriod),
	itsTabGroup(tabGroup)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNBusyTabTask::~SVNBusyTabTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
SVNBusyTabTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		itsTabGroup->IncrementSpinnerIndex();
		}
}
