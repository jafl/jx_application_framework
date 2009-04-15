/******************************************************************************
 JXAdjustToolBarGeometryTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXAdjustToolBarGeometryTask.h>
#include <JXToolBar.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAdjustToolBarGeometryTask::JXAdjustToolBarGeometryTask
	(
	JXToolBar* toolBar
	)
	:
	JXUrgentTask()
{
	itsToolBar = toolBar;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAdjustToolBarGeometryTask::~JXAdjustToolBarGeometryTask()
{
}

/******************************************************************************
 Perform

	By clearing itsAdjustTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
JXAdjustToolBarGeometryTask::Perform()
{
	itsToolBar->itsAdjustTask = NULL;	// we will now be deleted
	itsToolBar->AdjustGeometryIfNeeded();
}
