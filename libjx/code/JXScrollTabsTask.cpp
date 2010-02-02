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
	ClearWhenGoingAway(itsTabGroup, &itsTabGroup);
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
