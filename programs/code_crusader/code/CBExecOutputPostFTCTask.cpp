/******************************************************************************
 CBExecOutputPostFTCTask.cpp

	This creates additional widgets in CBExecOutputDocument *after* FTC, to
	keep everything aligned.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#include "CBExecOutputPostFTCTask.h"
#include "CBExecOutputDocument.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExecOutputPostFTCTask::CBExecOutputPostFTCTask
	(
	CBExecOutputDocument* doc
	)
	:
	itsDoc(doc)
{
	ClearWhenGoingAway(itsDoc, &itsDoc);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBExecOutputPostFTCTask::~CBExecOutputPostFTCTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CBExecOutputPostFTCTask::Perform()
{
	if (itsDoc != nullptr)
		{
		itsDoc->PlaceCmdLineWidgets();
		}
}
