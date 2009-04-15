/******************************************************************************
 JXUpdateWDMenuTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXUpdateWDMenuTask.h>
#include <JXWDManager.h>

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
	itsWDMgr->itsUpdateWDMenuTask = NULL;	// we will now be deleted
	itsWDMgr->UpdateAllWDMenus();
}
