/******************************************************************************
 TestIdleTask.cpp

	This class tests the mutext used in JXApplication.

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include "TestIdleTask.h"
#include <jx-af/jx/JXTimerTask.h>
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
 Perform

 ******************************************************************************/

void
TestIdleTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
	{
		itsCounter++;
		jnew JXTimerTask(100, true);

		if (itsCounter >= 10)
		{
			jdelete this;		// safe to commit suicide as last action
		}
	}
}
