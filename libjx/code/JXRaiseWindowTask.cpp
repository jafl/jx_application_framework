/******************************************************************************
 JXRaiseWindowTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
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
