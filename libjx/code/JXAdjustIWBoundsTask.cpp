/******************************************************************************
 JXAdjustIWBoundsTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXAdjustIWBoundsTask.h"
#include "JXImageWidget.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAdjustIWBoundsTask::JXAdjustIWBoundsTask
	(
	JXImageWidget* widget
	)
	:
	JXUrgentTask(widget),
	itsImageWidget(widget)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXAdjustIWBoundsTask::~JXAdjustIWBoundsTask()
{
}

/******************************************************************************
 Perform (virtual protected)

	By clearing itsAdjustBoundsTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
JXAdjustIWBoundsTask::Perform()
{
	itsImageWidget->itsAdjustBoundsTask = nullptr;	// we will now be deleted
	itsImageWidget->AdjustBounds();
}
