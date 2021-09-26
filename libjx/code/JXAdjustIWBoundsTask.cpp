/******************************************************************************
 JXAdjustIWBoundsTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXAdjustIWBoundsTask.h"
#include "jx-af/jx/JXImageWidget.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAdjustIWBoundsTask::JXAdjustIWBoundsTask
	(
	JXImageWidget* widget
	)
{
	itsImageWidget = widget;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAdjustIWBoundsTask::~JXAdjustIWBoundsTask()
{
}

/******************************************************************************
 Perform

	By clearing itsAdjustBoundsTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
JXAdjustIWBoundsTask::Perform()
{
	itsImageWidget->itsAdjustBoundsTask = nullptr;	// we will now be deleted
	itsImageWidget->AdjustBounds();
}
