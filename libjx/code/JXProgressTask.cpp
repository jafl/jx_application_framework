/******************************************************************************
 JXProgressTask.cpp

	Displays a progress display that ticks once per seconds.  Broadcasts
	Cancelled message if the process is cancelled.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright © 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "JXProgressTask.h"
#include <JLatentPG.h>
#include <JString.h>
#include <jAssert.h>

const JSize kUpdateInterval = 1000;		// 1 second (milliseconds)

// JBroadcaster message types

const JCharacter* JXProgressTask::kCancelled = "Cancelled::JXProgressTask";

/******************************************************************************
 Constructor (protected)

	By providing a constructor taking JString, derived classes can
	construct the message on the fly.

 ******************************************************************************/

JXProgressTask::JXProgressTask
	(
	const JString&	message,
	const JBoolean	allowCancel
	)
	:
	JXIdleTask(kUpdateInterval)
{
	JXProgressTaskX(message, allowCancel);
}

JXProgressTask::JXProgressTask
	(
	const JCharacter*	message,
	const JBoolean		allowCancel
	)
	:
	JXIdleTask(kUpdateInterval)
{
	JXProgressTaskX(message, allowCancel);
}

JXProgressTask::JXProgressTask
	(
	JProgressDisplay* pg
	)
	:
	JXIdleTask(kUpdateInterval),
	itsPG(pg),
	itsOwnsPGFlag(kJFalse)
{
}

// private

void
JXProgressTask::JXProgressTaskX
	(
	const JCharacter*	message,
	const JBoolean		allowCancel
	)
{
	itsPG = new JLatentPG;
	assert( itsPG != NULL );
	itsPG->VariableLengthProcessBeginning(message, allowCancel, kJTrue);

	itsOwnsPGFlag = kJTrue;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXProgressTask::~JXProgressTask()
{
	if (itsOwnsPGFlag)
		{
		itsPG->ProcessFinished();
		delete itsPG;
		}
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
JXProgressTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime) &&
		(( itsOwnsPGFlag && !itsPG->IncrementProgress()) ||
		 (!itsOwnsPGFlag && !itsPG->ProcessContinuing())))
		{
		Broadcast(Cancelled());
		delete this;	// safe to commit suicide as last action
		}
}
