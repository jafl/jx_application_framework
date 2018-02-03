/******************************************************************************
 JXUpdateDocMenuTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

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
