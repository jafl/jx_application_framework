/******************************************************************************
 SVNRefreshStatusTask.cpp

	Since there can be only one action process, this task waits until it is
	safe to refresh the working copy status.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "SVNRefreshStatusTask.h"
#include "SVNMainDirector.h"
#include <jAssert.h>

const JSize kRetryPeriod = 500;		// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNRefreshStatusTask::SVNRefreshStatusTask
	(
	SVNMainDirector* director
	)
	:
	JXIdleTask(kRetryPeriod),
	itsDirector(director)
{
	ClearWhenGoingAway(itsDirector, &itsDirector);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNRefreshStatusTask::~SVNRefreshStatusTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
SVNRefreshStatusTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (itsDirector == nullptr)
		{
		jdelete this;
		}

	if (TimeToPerform(delta, maxSleepTime) &&
		itsDirector->OKToStartActionProcess())
		{
		itsDirector->RefreshStatus();	// deletes us
		}
}
