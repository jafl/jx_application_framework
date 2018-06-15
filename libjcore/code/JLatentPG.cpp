/******************************************************************************
 JLatentPG.cpp

	This class encapsulates a progress display that only pops up after
	N seconds (default 3) and only redraws itself every M increments
	(default 1).

	This allows one to write a loop that doesn't waste time creating
	a progress display unless it takes a while and doesn't waste time
	redrawing every time through the loop.

	The scale factor is especially useful when one has a loop whose contents
	execute very quickly on each pass, so that redrawing the screen after every
	pass adds a huge overhead.

	BASE CLASS = JProgressDisplay

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JLatentPG.h"
#include "JString.h"
#include "jGlobals.h"
#include "jAssert.h"

const time_t kDefMaxSilentTime = 3;		// seconds

/******************************************************************************
 Constructor

 ******************************************************************************/

JLatentPG::JLatentPG
	(
	const JSize scaleFactor
	)
{
	itsPG         = JNewPG();
	itsOwnsPGFlag = kJTrue;

	itsMaxSilentTime = kDefMaxSilentTime;
	itsScaleFactor   = scaleFactor;
}

JLatentPG::JLatentPG
	(
	JProgressDisplay*	pg,
	const JBoolean		ownIt,
	const JSize			scaleFactor
	)
{
	itsPG         = pg;
	itsOwnsPGFlag = ownIt;

	itsMaxSilentTime = kDefMaxSilentTime;
	itsScaleFactor   = scaleFactor;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JLatentPG::~JLatentPG()
{
	if (itsOwnsPGFlag)
		{
		jdelete itsPG;
		}
}

/******************************************************************************
 UseDefaultPG

	Use a progress display obtained from JNewPG().

	This can only be called when a process is not running.

 ******************************************************************************/

void
JLatentPG::UseDefaultPG()
{
	SetPG(JNewPG(), kJTrue);
}

/******************************************************************************
 SetPG

	If ownIt == kJTrue, we delete the progress display when we no longer
	need it.

	This can only be called when a process is not running.

 ******************************************************************************/

void
JLatentPG::SetPG
	(
	JProgressDisplay*	pg,
	const JBoolean		ownIt
	)
{
	assert( !ProcessRunning() );
	assert( pg != nullptr && !pg->ProcessRunning() );

	if (itsOwnsPGFlag)
		{
		jdelete itsPG;
		}

	itsPG         = pg;
	itsOwnsPGFlag = ownIt;
}

/******************************************************************************
 ProcessBeginning (virtual protected)

 ******************************************************************************/

void
JLatentPG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JString&		message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, allowBackground);

	itsMessage   = message;
	itsStartTime = time(nullptr);
	itsCounter   = 0;

	if (itsMaxSilentTime == 0)
		{
		StartInternalProcess();
		}
}

/******************************************************************************
 StartInternalProcess (private)

 ******************************************************************************/

void
JLatentPG::StartInternalProcess()
{
	const ProcessType type = GetCurrentProcessType();
	if (type == kFixedLengthProcess)
		{
		itsPG->FixedLengthProcessBeginning(
			GetMaxStepCount(), itsMessage,
			AllowCancel(), AllowBackground());
		}
	else
		{
		assert( type == kVariableLengthProcess );
		itsPG->VariableLengthProcessBeginning(
			itsMessage, AllowCancel(), AllowBackground());
		}
}

/******************************************************************************
 IncrementProgress (virtual)

 ******************************************************************************/

JBoolean
JLatentPG::IncrementProgress
	(
	const JString& message
	)
{
	return message.IsEmpty() ? IncrementProgress(1) : IncrementProgress(message, 1);
}

/******************************************************************************
 IncrementProgress (virtual)

 ******************************************************************************/

JBoolean
JLatentPG::IncrementProgress
	(
	const JSize delta
	)
{
	assert( ProcessRunning() );

	IncrementStepCount(delta);

	const JBoolean pgRunning = itsPG->ProcessRunning();
	JBoolean result          = kJTrue;

	itsCounter++;
	if (!pgRunning && TimeToStart())
		{
		StartInternalProcess();
		result = itsPG->IncrementProgress(GetCurrentStepCount() -
										  itsPG->GetCurrentStepCount());
		itsCounter = 0;
		}
	else if (TimeToUpdate() && pgRunning)
		{
		result = itsPG->IncrementProgress(GetCurrentStepCount() -
										  itsPG->GetCurrentStepCount());
		itsCounter = 0;
		}

	return result;
}

/******************************************************************************
 IncrementProgress (virtual)

 ******************************************************************************/

JBoolean
JLatentPG::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
	)
{
	assert( ProcessRunning() );

	IncrementStepCount(delta);

	const JBoolean pgRunning = itsPG->ProcessRunning();
	JBoolean result          = kJTrue;

	itsCounter++;
	if (!pgRunning && (TimeToStart() || !message.IsEmpty()))
		{
		StartInternalProcess();

		// delta must be calculated -after- ProcessBeginning()

		const JSize delta = GetCurrentStepCount() - itsPG->GetCurrentStepCount();
		if (delta > 1)
			{
			itsPG->IncrementProgress(delta - 1);
			}
		result     = itsPG->IncrementProgress(message);
		itsCounter = 0;
		}
	else if (pgRunning && message != nullptr)
		{
		const JSize delta = GetCurrentStepCount() - itsPG->GetCurrentStepCount();
		if (delta > 1)
			{
			itsPG->IncrementProgress(delta - 1);
			}
		result     = itsPG->IncrementProgress(message);
		itsCounter = 0;
		}
	else if (pgRunning && TimeToUpdate())
		{
		result = itsPG->IncrementProgress(GetCurrentStepCount() -
										  itsPG->GetCurrentStepCount());
		itsCounter = 0;
		}

	return result;
}

/******************************************************************************
 ProcessContinuing (virtual)

 ******************************************************************************/

JBoolean
JLatentPG::ProcessContinuing()
{
	if (itsPG->ProcessRunning())
		{
		return itsPG->ProcessContinuing();
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 CheckForCancel (virtual protected)

 ******************************************************************************/

JBoolean
JLatentPG::CheckForCancel()
{
	return kJFalse;
}

/******************************************************************************
 ProcessFinished (virtual)

 ******************************************************************************/

void
JLatentPG::ProcessFinished()
{
	JProgressDisplay::ProcessFinished();

	if (itsPG->ProcessRunning())
		{
		itsPG->ProcessFinished();
		}
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
JLatentPG::DisplayBusyCursor()
{
	itsPG->DisplayBusyCursor();
}

/******************************************************************************
 When to increment display (private)

 ******************************************************************************/

inline int
JLatentPG::TimeToStart()
	const
{
	return (itsCounter % itsScaleFactor == 0 &&				// avoid calling time() too often
			time(nullptr) - itsStartTime >= itsMaxSilentTime &&
			(GetCurrentProcessType() == kVariableLengthProcess ||
			 GetCurrentStepCount() < GetMaxStepCount()));
}

inline int
JLatentPG::TimeToUpdate()
	const
{
	return (itsCounter >= itsScaleFactor);
}
