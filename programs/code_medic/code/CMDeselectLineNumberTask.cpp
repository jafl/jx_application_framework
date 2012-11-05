/******************************************************************************
 CMDeselectLineNumberTask.cpp

	Deselects the line number after the Line menu is closed.

	BASE CLASS = JXIdleTask

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMDeselectLineNumberTask.h"
#include "CMLineNumberTable.h"
#include <JXTextMenu.h>
#include <JTableSelection.h>
#include <jAssert.h>

const Time kCheckInterval = 500;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

CMDeselectLineNumberTask::CMDeselectLineNumberTask
	(
	CMLineNumberTable* table
	)
	:
	JXIdleTask(kCheckInterval),
	itsTable(table)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMDeselectLineNumberTask::~CMDeselectLineNumberTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMDeselectLineNumberTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime) &&
		!(itsTable->itsLineMenu)->IsOpen())
		{
		(itsTable->GetTableSelection()).ClearSelection();
		itsTable->itsDeselectTask = NULL;
		delete this;
		}
}
