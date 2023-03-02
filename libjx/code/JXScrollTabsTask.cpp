/******************************************************************************
 JXScrollTabsTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "JXScrollTabsTask.h"
#include "JXTabGroup.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXScrollTabsTask::JXScrollTabsTask
	(
	JXTabGroup* tabGroup
	)
	:
	JXUrgentTask(tabGroup),
	itsTabGroup(tabGroup)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXScrollTabsTask::~JXScrollTabsTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXScrollTabsTask::Perform()
{
	itsTabGroup->ScrollTabsIntoView();
}
