/******************************************************************************
 JXCSFSelectPrevDirTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCSFSelectPrevDirTask.h>
#include <JXDirTable.h>
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCSFSelectPrevDirTask::JXCSFSelectPrevDirTask
	(
	JDirInfo*			dirInfo,
	JXDirTable*			dirTable,
	const JCharacter*	dirName
	)
	:
	JXUrgentTask()
{
	itsDirInfo  = dirInfo;
	itsDirTable = dirTable;
	ListenTo(itsDirTable);

	itsDirName = new JString(dirName);
	assert( itsDirName != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCSFSelectPrevDirTask::~JXCSFSelectPrevDirTask()
{
	delete itsDirName;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXCSFSelectPrevDirTask::Perform()
{
	JIndex index;
	if (itsDirTable != NULL && itsDirInfo->FindEntry(*itsDirName, &index))
		{
		itsDirTable->UpdateScrollbars();
		itsDirTable->SelectSingleEntry(index);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	When JXDSSFinishSaveTask performs, we will be left hanging in the queue.

 ******************************************************************************/

void
JXCSFSelectPrevDirTask::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsDirTable)
		{
		itsDirTable = NULL;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}
