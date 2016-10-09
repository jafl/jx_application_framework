/******************************************************************************
 CMQuitTask.cpp

	Quit if all windows are hidden.

	BASE CLASS = JXIdleTask

	Copyright (C) 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMQuitTask.h"
#include <jXGlobals.h>
#include <jAssert.h>

const Time kCheckInterval = 1000;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

CMQuitTask::CMQuitTask()
	:
	JXIdleTask(kCheckInterval)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMQuitTask::~CMQuitTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMQuitTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	JBoolean quit = kJFalse;

	const JPtrArray<JXDirector>* list;
	if (TimeToPerform(delta, maxSleepTime) &&
		(JXGetApplication())->GetSubdirectors(&list))
		{
		quit = kJTrue;

		const JSize count = list->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if ((list->NthElement(i))->IsActive())
				{
				quit = kJFalse;
				break;
				}
			}
		}

	if (quit)
		{
		(JXGetApplication())->Quit();
		jdelete this;
		}
}
