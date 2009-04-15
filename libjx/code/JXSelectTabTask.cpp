/******************************************************************************
 JXSelectTabTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSelectTabTask.h>
#include <JXTabGroup.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSelectTabTask::JXSelectTabTask
	(
	JXTabGroup*		tabGroup,
	const JIndex	index
	)
	:
	itsTabGroup(tabGroup),
	itsIndex(index)
{
	ListenTo(itsTabGroup);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSelectTabTask::~JXSelectTabTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXSelectTabTask::Perform()
{
	if (itsTabGroup != NULL && itsIndex <= itsTabGroup->GetTabCount())
		{
		itsTabGroup->ShowTab(itsIndex);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JXSelectTabTask::ReceiveGoingAway
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
