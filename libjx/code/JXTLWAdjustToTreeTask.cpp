/******************************************************************************
 JXTLWAdjustToTreeTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXTLWAdjustToTreeTask.h"
#include "jx-af/jx/JXTreeListWidget.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTLWAdjustToTreeTask::JXTLWAdjustToTreeTask
	(
	JXTreeListWidget* widget
	)
{
	itsWidget = widget;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTLWAdjustToTreeTask::~JXTLWAdjustToTreeTask()
{
}

/******************************************************************************
 Perform

	By clearing itsAdjustToTreeTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
JXTLWAdjustToTreeTask::Perform()
{
	itsWidget->itsAdjustToTreeTask = nullptr;	// we will now be deleted
	itsWidget->AdjustToTree();
}
