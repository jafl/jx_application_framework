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
	JXUrgentTask(wdMgr),
	itsWDMgr(wdMgr)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXUpdateWDMenuTask::~JXUpdateWDMenuTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXUpdateWDMenuTask::Perform()
{
	itsWDMgr->UpdateAllWDMenus();
	if (itsWDMgr != nullptr)
	{
		itsWDMgr->itsUpdateWDMenuTask = nullptr;
	}
}
