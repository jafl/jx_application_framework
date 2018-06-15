/******************************************************************************
 JXAdjustToolBarGeometryTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "JXAdjustToolBarGeometryTask.h"
#include "JXToolBar.h"

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
	itsToolBar->itsAdjustTask = nullptr;	// we will now be deleted
	itsToolBar->AdjustGeometryIfNeeded();
}
