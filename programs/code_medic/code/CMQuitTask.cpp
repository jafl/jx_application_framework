/******************************************************************************
 CMQuitTask.cpp

	Quit if all windows are hidden.

	BASE CLASS = JXIdleTask

	Copyright (C) 2009 by John Lindal.

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
	bool quit = false;

	const JPtrArray<JXDirector>* list;
	if (TimeToPerform(delta, maxSleepTime) &&
		JXGetApplication()->GetSubdirectors(&list))
		{
		quit = true;

		const JSize count = list->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if ((list->GetElement(i))->IsActive())
				{
				quit = false;
				break;
				}
			}
		}

	if (quit)
		{
		JXGetApplication()->Quit();
		jdelete this;
		}
}
