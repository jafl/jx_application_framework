/******************************************************************************
 JXAdjustScrollbarTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXAdjustScrollbarTask.h"
#include "JXScrollableWidget.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAdjustScrollbarTask::JXAdjustScrollbarTask
	(
	JXScrollableWidget* widget
	)
	:
	JXUrgentTask(widget),
	itsScrollableWidget(widget)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXAdjustScrollbarTask::~JXAdjustScrollbarTask()
{
}

/******************************************************************************
 Perform (virtual protected)

	By clearing itsAdjustScrollbarTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
JXAdjustScrollbarTask::Perform()
{
	itsScrollableWidget->itsAdjustScrollbarTask = nullptr;	// we will now be deleted
	itsScrollableWidget->AdjustScrollbars();
}
