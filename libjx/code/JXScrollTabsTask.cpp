/******************************************************************************
 JXScrollTabsTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXScrollTabsTask.h>
#include <JXTabGroup.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXScrollTabsTask::JXScrollTabsTask
	(
	JXTabGroup* tabGroup
	)
	:
	itsTabGroup(tabGroup)
{
	ListenTo(itsTabGroup);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXScrollTabsTask::~JXScrollTabsTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXScrollTabsTask::Perform()
{
	if (itsTabGroup != NULL)
		{
		itsTabGroup->ScrollTabsIntoView();
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JXScrollTabsTask::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsTabGroup)
		{
		itsTabGroup = NULL;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}
