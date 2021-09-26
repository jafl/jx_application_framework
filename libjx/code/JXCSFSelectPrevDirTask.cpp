/******************************************************************************
 JXCSFSelectPrevDirTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXCSFSelectPrevDirTask.h"
#include "jx-af/jx/JXDirTable.h"
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCSFSelectPrevDirTask::JXCSFSelectPrevDirTask
	(
	JDirInfo*		dirInfo,
	JXDirTable*		dirTable,
	const JString&	dirName
	)
	:
	JXUrgentTask(),
	itsDirInfo(dirInfo),
	itsDirTable(dirTable),
	itsDirName(dirName)
{
	ClearWhenGoingAway(itsDirTable, &itsDirTable);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCSFSelectPrevDirTask::~JXCSFSelectPrevDirTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXCSFSelectPrevDirTask::Perform()
{
	JIndex index;
	if (itsDirTable != nullptr && itsDirInfo->FindEntry(itsDirName, &index))
	{
		itsDirTable->UpdateScrollbars();
		itsDirTable->SelectSingleEntry(index);
	}
}
