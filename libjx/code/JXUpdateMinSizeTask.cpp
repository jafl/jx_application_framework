/******************************************************************************
 JXUpdateMinSizeTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXUpdateMinSizeTask.h>
#include <JXDockWidget.h>

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
	ListenTo(itsDockWidget);
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
	if (itsDockWidget != NULL)
		{
		itsDockWidget->UpdateMinSize();
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JXUpdateMinSizeTask::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsDockWidget)
		{
		itsDockWidget = NULL;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}
