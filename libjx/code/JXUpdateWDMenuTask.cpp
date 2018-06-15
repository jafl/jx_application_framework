/******************************************************************************
 JXUpdateWDMenuTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "JXUpdateWDMenuTask.h"
#include "JXWDManager.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUpdateWDMenuTask::JXUpdateWDMenuTask
	(
	JXWDManager* wdMgr
	)
	:
	JXUrgentTask()
{
	itsWDMgr = wdMgr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXUpdateWDMenuTask::~JXUpdateWDMenuTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXUpdateWDMenuTask::Perform()
{
	itsWDMgr->itsUpdateWDMenuTask = nullptr;	// we will now be deleted
	itsWDMgr->UpdateAllWDMenus();
}
