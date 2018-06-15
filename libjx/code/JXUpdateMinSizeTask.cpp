/******************************************************************************
 JXUpdateMinSizeTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "JXUpdateMinSizeTask.h"
#include "JXDockWidget.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUpdateMinSizeTask::JXUpdateMinSizeTask
	(
	JXDockWidget* dock
	)
	:
	itsDockWidget(dock)
{
	ClearWhenGoingAway(itsDockWidget, &itsDockWidget);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXUpdateMinSizeTask::~JXUpdateMinSizeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXUpdateMinSizeTask::Perform()
{
	if (itsDockWidget != nullptr)
		{
		itsDockWidget->UpdateMinSize();
		}
}
