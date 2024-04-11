/******************************************************************************
 JXStandAlonePG.cpp

	Class to display the progress of a long process in a separate window.

	BASE CLASS = JXProgressDisplay

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JXStandAlonePG.h"
#include "JXFixLenPGDirector.h"
#include "JXVarLenPGDirector.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStandAlonePG::JXStandAlonePG()
	:
	JXProgressDisplay(),
	itsProgressDirector(nullptr),
	itsRaiseWindowFlag(false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStandAlonePG::~JXStandAlonePG()
{
	assert( itsProgressDirector == nullptr );
}

/******************************************************************************
 ProcessBeginning (virtual protected)

 ******************************************************************************/

void
JXStandAlonePG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JString&		message,
	const bool			allowCancel,
	const bool			modal
	)
{
	// create the window

	assert( itsProgressDirector == nullptr );

	if (processType == kFixedLengthProcess)
	{
		itsProgressDirector = jnew JXFixLenPGDirector(this, message, allowCancel, modal);
		itsStepCount = stepCount;
	}
	else if (processType == kVariableLengthProcess)
	{
		itsProgressDirector = jnew JXVarLenPGDirector(this, message, allowCancel, modal);
		itsStepCount = 0;
	}
	assert( itsProgressDirector != nullptr );

	// display the window

	if (processType == kFixedLengthProcess && stepCount == 1)
	{
		DisplayBusyCursor();
	}
	else
	{
		itsProgressDirector->Activate();
	}

	JXProgressDisplay::ProcessBeginning(processType, stepCount, message,
										allowCancel, modal);
}

/******************************************************************************
 ProcessContinuing (virtual)

	Derived classes should give the system some background time (system dependent)
	without updating the progress display.

	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
JXStandAlonePG::ProcessContinuing()
{
	JXWindow* window = itsProgressDirector->GetWindow();
	if (itsProgressDirector->IsActive() && itsRaiseWindowFlag)
	{
		window->Raise(false);
	}

	if (itsStepCount == 0)
	{
		const JString s(GetCurrentStepCount());
		itsProgressDirector->ProcessContinuing(s);
	}
	else
	{
		JString s(100.0 * (GetCurrentStepCount() / (JFloat) itsStepCount), 0);
		s += "%";
		itsProgressDirector->ProcessContinuing(s);
	}

	return JXProgressDisplay::ProcessContinuing();
}

/******************************************************************************
 ProcessFinished (virtual)

 ******************************************************************************/

void
JXStandAlonePG::ProcessFinished()
{
	SetItems(nullptr, nullptr, nullptr);

	assert( itsProgressDirector != nullptr );
	itsProgressDirector->ProcessFinished();
	itsProgressDirector = nullptr;

	JXProgressDisplay::ProcessFinished();
}
