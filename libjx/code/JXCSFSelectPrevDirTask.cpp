/******************************************************************************
 JXCSFSelectPrevDirTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXCSFSelectPrevDirTask.h>
#include <JXDirTable.h>
#include <JDirInfo.h>
#include <jAssert.h>

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
	if (itsDirTable != NULL && itsDirInfo->FindEntry(itsDirName, &index))
		{
		itsDirTable->UpdateScrollbars();
		itsDirTable->SelectSingleEntry(index);
		}
}
