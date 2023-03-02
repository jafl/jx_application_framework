/******************************************************************************
 JXCSFSelectPrevDirTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXCSFSelectPrevDirTask.h"
#include "JXDirTable.h"
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
	JXUrgentTask(dirTable),
	itsDirInfo(dirInfo),
	itsDirTable(dirTable),
	itsDirName(dirName)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXCSFSelectPrevDirTask::~JXCSFSelectPrevDirTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXCSFSelectPrevDirTask::Perform()
{
	JIndex index;
	if (itsDirInfo->FindEntry(itsDirName, &index))
	{
		itsDirTable->UpdateScrollbars();
		itsDirTable->SelectSingleEntry(index);
	}
}
