/******************************************************************************
 JXAdjustIWBoundsTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXAdjustIWBoundsTask.h>
#include <JXImageWidget.h>

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
	itsImageWidget->itsAdjustBoundsTask = NULL;	// we will now be deleted
	itsImageWidget->AdjustBounds();
}
