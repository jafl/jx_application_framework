/******************************************************************************
 JXProgressTask.cpp

	Displays a progress display that ticks once per seconds.  Broadcasts
	Cancelled message if the process is cancelled.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright (C) 2000 John Lindal.

 ******************************************************************************/

#include "JXProgressTask.h"
#include <JLatentPG.h>
#include <JString.h>
#include <jAssert.h>

const JSize kUpdateInterval = 1000;		// 1 second (milliseconds)

// JBroadcaster message types

const JUtf8Byte* JXProgressTask::kCancelled = "Cancelled::JXProgressTask";

/******************************************************************************
 Constructor (protected)

	By providing a constructor taking JString, derived classes can
	construct the message on the fly.

 ******************************************************************************/

JXProgressTask::JXProgressTask
	(
	const JString&	message,
	const bool	allowCancel
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
	itsOwnsPGFlag(false)
{
}

// private

void
JXProgressTask::JXProgressTaskX
	(
	const JString&	message,
	const bool	allowCancel
	)
{
	itsPG = jnew JLatentPG;
	assert( itsPG != nullptr );
	itsPG->VariableLengthProcessBeginning(message, allowCancel, true);

	itsOwnsPGFlag = true;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXProgressTask::~JXProgressTask()
{
	if (itsOwnsPGFlag)
	{
		itsPG->ProcessFinished();
		jdelete itsPG;
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
		jdelete this;	// safe to commit suicide as last action
	}
}
