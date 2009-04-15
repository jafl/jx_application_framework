/******************************************************************************
 TestPGTask.cpp

	This class demonstrates how to use a progress display for a background
	task.  When you use this class, notice that things run fairly slowly.
	This is because we do 0.5 seconds of work for every second of event
	processing time.  The best is to split the work into very small
	increments.  The next best thing is to wait a while between chunks.

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestPGTask.h"
#include <jXGlobals.h>
#include <jTime.h>
#include <jAssert.h>

const JSize kStepCount       = 100;
const JSize kQuiescentPeriod = 1000;	// 1 second (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPGTask::TestPGTask
	(
	const JBoolean fixedLength
	)
	:
	JXIdleTask(kQuiescentPeriod)
{
	itsPG      = JNewPG();
	itsCounter = 0;

	if (fixedLength)
		{
		itsPG->FixedLengthProcessBeginning(kStepCount, "Crunching numbers...",
										   kJTrue, kJTrue);
		}
	else
		{
		itsPG->VariableLengthProcessBeginning("Crunching numbers...", kJTrue, kJTrue);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestPGTask::~TestPGTask()
{
	if (itsPG->ProcessRunning())
		{
		itsPG->ProcessFinished();
		}
	delete itsPG;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
TestPGTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	JBoolean keepGoing = kJTrue;
	if (TimeToPerform(delta, maxSleepTime))
		{
		JWait(0.5);		// simulate massive, greedy number crunching
		itsCounter++;
		keepGoing = itsPG->IncrementProgress();
		}
	else
		{
		keepGoing = itsPG->ProcessContinuing();
		}

	if (!keepGoing || itsCounter >= kStepCount)
		{
		itsPG->ProcessFinished();
		delete this;				// safe to commit suicide as last action
		}
}
