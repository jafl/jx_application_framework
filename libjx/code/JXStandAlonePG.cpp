/******************************************************************************
 JXStandAlonePG.cpp

	Class to display the progress of a long process in a separate window.

	If the allowBackground flag is false, we call
	JXApplication::ProcessOneEventForWindow() so we can detect the cancel
	button.  If the allowBackground flag is true, we do nothing, because we
	assume that the client is using an IdleTask to schedule the processing time.

	BASE CLASS = JXProgressDisplay

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXStandAlonePG.h"
#include "jx-af/jx/JXFixLenPGDirector.h"
#include "jx-af/jx/JXVarLenPGDirector.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

// Private class data

struct JXPGWindowPosition
{
	JCoordinate			x,y;
	JXPGDirectorBase*	dir;
};

static JArray<JXPGWindowPosition> winPos;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStandAlonePG::JXStandAlonePG()
	:
	JXProgressDisplay()
{
	itsWindowIndex      = 0;
	itsProgressDirector = nullptr;
	itsRaiseWindowFlag  = false;
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
	const bool		allowCancel,
	const bool		allowBackground
	)
{
	// create the window

	if (!allowBackground)
	{
		JXGetApplication()->PrepareForBlockingWindow();
	}

	assert( itsProgressDirector == nullptr );

	if (processType == kFixedLengthProcess)
	{
		itsProgressDirector = 
			jnew JXFixLenPGDirector(JXGetApplication(), this, message, allowCancel);
		itsStepCount = stepCount;
	}
	else if (processType == kVariableLengthProcess)
	{
		itsProgressDirector = 
			jnew JXVarLenPGDirector(JXGetApplication(), this, message, allowCancel);
		itsStepCount = 0;
	}
	assert( itsProgressDirector != nullptr );

	// find the first unused window position and use it

	itsWindowIndex       = 0;
	const JSize winCount = winPos.GetElementCount();
	for (JIndex i=1; i<=winCount; i++)
	{
		const JXPGWindowPosition winInfo = winPos.GetElement(i);
		if (winInfo.dir == nullptr)
		{
			itsWindowIndex = i;
			break;
		}
	}

	if (itsWindowIndex > 0)
	{
		JXPGWindowPosition winInfo = winPos.GetElement(itsWindowIndex);
		(itsProgressDirector->GetWindow())->Place(winInfo.x, winInfo.y);
		winInfo.dir = itsProgressDirector;
		winPos.SetElement(itsWindowIndex, winInfo);
	}
	else if (winCount > 0)
	{
		JXPGWindowPosition winInfo = winPos.GetElement(winCount);
		JXWindow* window = (winInfo.dir)->GetWindow();
		winInfo.y = (window->GlobalToRoot((window->GetFrameGlobal()).bottomLeft())).y;
		(itsProgressDirector->GetWindow())->Place(winInfo.x, winInfo.y);
		winInfo.dir = itsProgressDirector;
		winPos.AppendElement(winInfo);
		itsWindowIndex = winCount+1;
	}
	else
	{
		const JPoint pt = (itsProgressDirector->GetWindow())->GetDesktopLocation();
		JXPGWindowPosition winInfo;
		winInfo.x   = pt.x;
		winInfo.y   = pt.y;
		winInfo.dir = itsProgressDirector;
		winPos.AppendElement(winInfo);
		itsWindowIndex = winCount+1;
	}

	// display the window

	if (processType == kFixedLengthProcess && stepCount == 1)
	{
		DisplayBusyCursor();
	}
	else
	{
		itsProgressDirector->Activate();
		(itsProgressDirector->GetDisplay())->Synchronize();
		(itsProgressDirector->GetWindow())->Update();
	}
	(itsProgressDirector->GetDisplay())->Synchronize();

	JXProgressDisplay::ProcessBeginning(processType, stepCount, message,
										allowCancel, allowBackground);
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
		const JString s((JUInt64) GetCurrentStepCount());
		itsProgressDirector->ProcessContinuing(s);
	}
	else
	{
		JString s(100.0 * (GetCurrentStepCount() / (JFloat) itsStepCount), 0);
		s += "%";
		itsProgressDirector->ProcessContinuing(s);
	}

	if (!AllowBackground())
	{
		while (JXGetApplication()->HandleOneEventForWindow(window, false))
		{ /* process all events in the queue */ }
	}

	itsProgressDirector->GetDisplay()->Flush();

	return JXProgressDisplay::ProcessContinuing();
}

/******************************************************************************
 ProcessFinished (virtual)

 ******************************************************************************/

void
JXStandAlonePG::ProcessFinished()
{
	JXPGWindowPosition winInfo = winPos.GetElement(itsWindowIndex);
	const JPoint pt = (itsProgressDirector->GetWindow())->GetDesktopLocation();
	winInfo.x   = pt.x;
	winInfo.y   = pt.y;
	winInfo.dir = nullptr;
	winPos.SetElement(itsWindowIndex, winInfo);
	itsWindowIndex = 0;

	SetItems(nullptr, nullptr, nullptr);

	assert( itsProgressDirector != nullptr );
	itsProgressDirector->ProcessFinished();
	itsProgressDirector = nullptr;

	if (!AllowBackground())
	{
		JXGetApplication()->BlockingWindowFinished();
	}

	JXProgressDisplay::ProcessFinished();
}
