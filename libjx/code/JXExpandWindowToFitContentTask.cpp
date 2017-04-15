/******************************************************************************
 JXExpandWindowToFitContentTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXExpandWindowToFitContentTask.h>
#include <JXWindow.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExpandWindowToFitContentTask::JXExpandWindowToFitContentTask
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

JXExpandWindowToFitContentTask::~JXExpandWindowToFitContentTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXExpandWindowToFitContentTask::Perform()
{
	if (itsWindow != NULL)
		{
		itsWindow->ExpandToFitContent();
		}
}
