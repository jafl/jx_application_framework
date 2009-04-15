/******************************************************************************
 JXTLWAdjustToTreeTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTLWAdjustToTreeTask.h>
#include <JXTreeListWidget.h>

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
	itsWidget->itsAdjustToTreeTask = NULL;	// we will now be deleted
	itsWidget->AdjustToTree();
}
