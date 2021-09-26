/******************************************************************************
 JXRaiseWindowTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXRaiseWindowTask.h"
#include "jx-af/jx/JXWindow.h"

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
	if (itsWindow != nullptr)
	{
		itsWindow->Raise();
	}
}
