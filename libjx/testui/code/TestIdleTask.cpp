/******************************************************************************
 TestIdleTask.cpp

	This class tests the mutex used in JXApplication.

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include "TestIdleTask.h"
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jx/JXUrgentFunctionTask.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestIdleTask::TestIdleTask()
	:
	JXIdleTask(100),
	itsCounter(0)
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
TestIdleTask::Perform
	(
	const Time delta
	)
{
	itsCounter++;
	JXFunctionTask* task = jnew JXFunctionTask(100, []()
	{
		// automatically deleted
	},
	true);
	task->Start();

	auto* utask = jnew JXUrgentFunctionTask(JXGetDocumentManager(), []()
	{
		// automatically deleted
	});
	utask->Go();

	if (itsCounter >= 10)
	{
		jdelete this;		// safe to commit suicide as last action
	}
}
