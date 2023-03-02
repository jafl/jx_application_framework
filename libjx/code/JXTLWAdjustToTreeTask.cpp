/******************************************************************************
 JXTLWAdjustToTreeTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXTLWAdjustToTreeTask.h"
#include "JXTreeListWidget.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTLWAdjustToTreeTask::JXTLWAdjustToTreeTask
	(
	JXTreeListWidget* widget
	)
	:
	JXUrgentTask(widget),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXTLWAdjustToTreeTask::~JXTLWAdjustToTreeTask()
{
}

/******************************************************************************
 Perform (virtual protected)

	By clearing itsAdjustToTreeTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
JXTLWAdjustToTreeTask::Perform()
{
	itsWidget->itsAdjustToTreeTask = nullptr;	// we will now be deleted
	itsWidget->AdjustToTree();
}
