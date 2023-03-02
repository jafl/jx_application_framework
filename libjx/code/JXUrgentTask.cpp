/******************************************************************************
 JXUrgentTask.cpp

	Urgent tasks are performed as soon as possible and then deleted.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-2023 by John Lindal.

 ******************************************************************************/

#include "JXUrgentTask.h"
#include "jXGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUrgentTask::JXUrgentTask
	(
	JBroadcaster* target
	)
	:
	itsTarget(target)
{
	ClearWhenGoingAway(itsTarget, &itsTarget);
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXUrgentTask::~JXUrgentTask()
{
}

/******************************************************************************
 Go

 ******************************************************************************/

void
JXUrgentTask::Go()
{
	JXGetApplication()->InstallUrgentTask(this);
}
