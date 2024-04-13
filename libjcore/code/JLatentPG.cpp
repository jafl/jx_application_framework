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
	:
	itsPG(JNewPG()),
	itsOwnsPGFlag(true),
	itsMaxSilentTime(kDefMaxSilentTime),
	itsScaleFactor(scaleFactor)
{
}

JLatentPG::JLatentPG
	(
	JProgressDisplay*	pg,
	const bool			ownIt,
	const JSize			scaleFactor
	)
	:
	itsPG(pg),
	itsOwnsPGFlag(ownIt),
	itsMaxSilentTime(kDefMaxSilentTime),
	itsScaleFactor(scaleFactor)
{
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
	SetPG(JNewPG(), true);
}

/******************************************************************************
 SetPG

	If ownIt == true, we delete the progress display when we no longer
	need it.

	This can only be called when a process is not running.

 ******************************************************************************/

void
JLatentPG::SetPG
	(
	JProgressDisplay*	pg,
	const bool			ownIt
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
	const bool			allowCancel,
	const bool			modal
	)
{
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, modal);

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
			AllowCancel(), IsModal());
	}
	else
	{
		assert( type == kVariableLengthProcess );
		itsPG->VariableLengthProcessBeginning(
			itsMessage, AllowCancel(), IsModal());
	}
}

/******************************************************************************
 IncrementProgress (virtual)

 ******************************************************************************/

bool
JLatentPG::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
	)
{
	assert( ProcessRunning() );

	IncrementStepCount(delta);

	const bool pgRunning = itsPG->ProcessRunning();
	bool result          = true;

	itsCounter++;
	if (!pgRunning && (TimeToStart() || !message.IsEmpty()))
	{
		StartInternalProcess();

		// delta must be calculated *after* ProcessBeginning()

		result = itsPG->IncrementProgress(message,
			GetCurrentStepCount() - itsPG->GetCurrentStepCount());

		itsCounter = 0;
	}
	else if (pgRunning && (TimeToUpdate() || !message.IsEmpty()))
	{
		result = itsPG->IncrementProgress(message,
			GetCurrentStepCount() - itsPG->GetCurrentStepCount());

		itsCounter = 0;
	}

	return result && ProcessContinuing();
}

/******************************************************************************
 ProcessContinuing (virtual)

 ******************************************************************************/

bool
JLatentPG::ProcessContinuing()
{
	return !itsPG->ProcessRunning() || itsPG->ProcessContinuing();
}

/******************************************************************************
 CheckForCancel (virtual protected)

 ******************************************************************************/

bool
JLatentPG::CheckForCancel()
{
	return false;
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
