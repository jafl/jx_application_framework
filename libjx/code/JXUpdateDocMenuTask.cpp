/******************************************************************************
 JXUpdateDocMenuTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JXUpdateDocMenuTask.h"
#include "JXDocumentManager.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUpdateDocMenuTask::JXUpdateDocMenuTask
	(
	JXDocumentManager* docMgr
	)
	:
	JXUrgentTask(docMgr),
	itsDocMgr(docMgr)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXUpdateDocMenuTask::~JXUpdateDocMenuTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXUpdateDocMenuTask::Perform()
{
	itsDocMgr->UpdateAllDocumentMenus();
	if (itsDocMgr != nullptr)
	{
		itsDocMgr->itsUpdateDocMenuTask = nullptr;
	}
}
