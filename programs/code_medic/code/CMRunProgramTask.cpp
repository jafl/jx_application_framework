/******************************************************************************
 CMRunProgramTask.cpp

	We cannot run immediately after the program is selected or even when we
	receive kSymbolsLoaded.  We can only run after everybody else has
	finished processing kSymbolsLoaded.

	BASE CLASS = JXIdleTask

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "CMRunProgramTask.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"

const Time kCheckInterval = 1;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

CMRunProgramTask::CMRunProgramTask()
	:
	JXIdleTask(kCheckInterval)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMRunProgramTask::~CMRunProgramTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMRunProgramTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (!CMGetLink()->HasPendingCommands())
		{
		(CMGetCommandDirector())->RunProgram();
		jdelete this;
		}
}
