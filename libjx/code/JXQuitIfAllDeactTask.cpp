/******************************************************************************
 JXQuitIfAllDeactTask.cpp

	Idle task to quit if all the application's subdirectors are inactive.

	BASE CLASS = JXIdleTask

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXQuitIfAllDeactTask.h"
#include "jx-af/jx/jXGlobals.h"
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

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
		if (!app->GetSubdirectors(&list) ||
			std::all_of(begin(*list), end(*list),
				[] (JXDirector* dir) { return !dir->IsActive(); }))
		{
				app->Quit();
		}
	}
}
