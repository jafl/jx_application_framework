/******************************************************************************
 JXRaiseWindowTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "JXRaiseWindowTask.h"
#include "JXWindow.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRaiseWindowTask::JXRaiseWindowTask
	(
	JXWindow* window
	)
	:
	JXUrgentTask(window),
	itsWindow(window)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXRaiseWindowTask::~JXRaiseWindowTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXRaiseWindowTask::Perform()
{
	itsWindow->Raise();
}
