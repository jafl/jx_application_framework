/******************************************************************************
 JXScrollTabsTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXScrollTabsTask.h"
#include "jx-af/jx/JXTabGroup.h"

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
	if (itsTabGroup != nullptr)
	{
		itsTabGroup->ScrollTabsIntoView();
	}
}
