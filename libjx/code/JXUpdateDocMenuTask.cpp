/******************************************************************************
 JXUpdateDocMenuTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXUpdateDocMenuTask.h>
#include <JXDocumentManager.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUpdateDocMenuTask::JXUpdateDocMenuTask
	(
	JXDocumentManager* docMgr
	)
	:
	JXUrgentTask()
{
	itsDocMgr = docMgr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXUpdateDocMenuTask::~JXUpdateDocMenuTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXUpdateDocMenuTask::Perform()
{
	itsDocMgr->itsUpdateDocMenuTask = NULL;	// we will now be deleted
	itsDocMgr->UpdateAllDocumentMenus();
}
