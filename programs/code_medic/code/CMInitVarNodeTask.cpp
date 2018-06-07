/******************************************************************************
 CMInitVarNodeTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "CMInitVarNodeTask.h"
#include "CMVarNode.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMInitVarNodeTask::CMInitVarNodeTask
	(
	CMVarNode* node
	)
{
	itsNode = node;
	ClearWhenGoingAway(itsNode, &itsNode);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMInitVarNodeTask::~CMInitVarNodeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMInitVarNodeTask::Perform()
{
	if (itsNode != nullptr)
		{
		itsNode->NameChanged();
		}
}
