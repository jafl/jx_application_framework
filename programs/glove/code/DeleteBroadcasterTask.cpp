/******************************************************************************
 DeleteBroadcasterTask.cpp

	Urgent tasks are performed as soon as possible and then deleted.

	BASE CLASS = JXUrgentTask

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <DeleteBroadcasterTask.h>

#include <JXApplication.h>

#include <JBroadcaster.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DeleteBroadcasterTask::DeleteBroadcasterTask
	(
	JBroadcaster* object
	)
	:
	JXUrgentTask()
{
	itsObject = object;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DeleteBroadcasterTask::~DeleteBroadcasterTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
DeleteBroadcasterTask::Perform()
{
	jdelete itsObject;
}
