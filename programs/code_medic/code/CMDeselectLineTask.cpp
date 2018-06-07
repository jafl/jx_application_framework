/******************************************************************************
 CMDeselectLineTask.cpp

	Deselects the line number after the Line menu is closed.

	BASE CLASS = JXIdleTask

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMDeselectLineTask.h"
#include "CMLineIndexTable.h"
#include <JXTextMenu.h>
#include <JTableSelection.h>
#include <jAssert.h>

const Time kCheckInterval = 500;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

CMDeselectLineTask::CMDeselectLineTask
	(
	CMLineIndexTable* table
	)
	:
	JXIdleTask(kCheckInterval),
	itsTable(table)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMDeselectLineTask::~CMDeselectLineTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMDeselectLineTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime) &&
		!(itsTable->itsLineMenu)->IsOpen())
		{
		(itsTable->GetTableSelection()).ClearSelection();
		itsTable->itsDeselectTask = nullptr;
		jdelete this;
		}
}
