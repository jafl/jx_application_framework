/******************************************************************************
 TestIdleTask.cpp

	This class tests the mutex used in JXApplication.

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include "TestIdleTask.h"
#include <jx-af/jx/JXTimerTask.h>
#include <jx-af/jx/JXUpdateDocMenuTask.h>
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
	(jnew JXTimerTask(100, true))->Start();
	(jnew JXUpdateDocMenuTask(JXGetDocumentManager()))->Go();

	if (itsCounter >= 10)
	{
		jdelete this;		// safe to commit suicide as last action
	}
}
