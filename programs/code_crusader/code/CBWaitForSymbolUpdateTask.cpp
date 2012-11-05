/******************************************************************************
 CBWaitForSymbolUpdateTask.cpp

	We cannot do this inside CBProjectDocument::SymbolUpdateProgress()
	because the ACE reactor is not re-entrant.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBWaitForSymbolUpdateTask.h"
#include <JXApplication.h>
#include <JProcess.h>
#include <jTime.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBWaitForSymbolUpdateTask::CBWaitForSymbolUpdateTask
	(
	JProcess* p
	)
	:
	itsProcess(p)
{
	itsKeepWaitingFlag = kJTrue;
	ClearWhenGoingAway(itsProcess, &itsProcess);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBWaitForSymbolUpdateTask::~CBWaitForSymbolUpdateTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CBWaitForSymbolUpdateTask::Perform()
{
	const time_t start = time(NULL);
	while (itsKeepWaitingFlag && itsProcess != NULL)
		{
		if (time(NULL) - start > 30)
			{
			itsProcess->Kill();
			break;
			}

		JWait(0.1);
		JXApplication::CheckACEReactor();
		JProcess::CheckForFinishedChild(kJFalse);
		}
}
