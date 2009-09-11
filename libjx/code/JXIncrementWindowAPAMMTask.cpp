/******************************************************************************
 JXIncrementWindowAPAMMTask.cpp

	In OSX, the placement of windows that are not initially visible will
	need adjustment when they are later shown.

	BASE CLASS = JXUrgentTask

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXIncrementWindowAPAMMTask.h>
#include <JXWindow.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXIncrementWindowAPAMMTask::JXIncrementWindowAPAMMTask
	(
	JXWindow* window
	)
{
	itsWindow = window;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXIncrementWindowAPAMMTask::~JXIncrementWindowAPAMMTask()
{
	itsWindow->itsIncrAPAMMTask = NULL;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXIncrementWindowAPAMMTask::Perform()
{
	itsWindow->itsAdjustPlacementAfterMapMode++;
}
