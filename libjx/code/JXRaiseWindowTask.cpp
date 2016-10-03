/******************************************************************************
 JXRaiseWindowTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXRaiseWindowTask.h>
#include <JXWindow.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRaiseWindowTask::JXRaiseWindowTask
	(
	JXWindow* window
	)
	:
	itsWindow(window)
{
	ClearWhenGoingAway(itsWindow, &itsWindow);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRaiseWindowTask::~JXRaiseWindowTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXRaiseWindowTask::Perform()
{
	if (itsWindow != NULL)
		{
		itsWindow->Raise();
		}
}
