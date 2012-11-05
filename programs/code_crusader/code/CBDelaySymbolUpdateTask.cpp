/******************************************************************************
 CBDelaySymbolUpdateTask.cpp

	BASE CLASS = JXIdleTask

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBDelaySymbolUpdateTask.h"
#include "CBProjectDocument.h"
#include <jAssert.h>

const Time kDelay = 60 * 1000;	// 60 seconds

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDelaySymbolUpdateTask::CBDelaySymbolUpdateTask
	(
	CBProjectDocument* projDoc
	)
	:
	JXIdleTask(kDelay),
	itsProjDoc(projDoc)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDelaySymbolUpdateTask::~CBDelaySymbolUpdateTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CBDelaySymbolUpdateTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		itsProjDoc->UpdateSymbolDatabase();		// deletes us
		}
}
