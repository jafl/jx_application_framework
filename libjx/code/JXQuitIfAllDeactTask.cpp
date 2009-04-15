/******************************************************************************
 JXQuitIfAllDeactTask.cpp

	Idle task to quit if all the application's subdirectors are inactive.

	BASE CLASS = JXIdleTask

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXQuitIfAllDeactTask.h>
#include <jXGlobals.h>
#include <jAssert.h>

const Time kCheckPeriod = 30001;	// 30 seconds (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

JXQuitIfAllDeactTask::JXQuitIfAllDeactTask()
	:
	JXIdleTask(kCheckPeriod)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXQuitIfAllDeactTask::~JXQuitIfAllDeactTask()
{
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
JXQuitIfAllDeactTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{		
		JXApplication* app = JXGetApplication();

		const JPtrArray<JXDirector>* list;
		if (app->GetSubdirectors(&list))
			{
			const JSize count = list->GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				if ((list->NthElement(i))->IsActive())
					{
					return;
					}
				}
			}

		app->Quit();
		}
}
