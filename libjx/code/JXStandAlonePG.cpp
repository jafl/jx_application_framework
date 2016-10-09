/******************************************************************************
 JXStandAlonePG.cpp

	Class to display the progress of a long process in a separate window.

	If the allowBackground flag is kJFalse, we call
	JXApplication::ProcessOneEventForWindow() so we can detect the cancel
	button.  If the allowBackground flag is kJTrue, we do nothing, because we
	assume that the client is using an IdleTask to schedule the processing time.

	BASE CLASS = JXProgressDisplay

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.
	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStandAlonePG.h>
#include <JXFixLenPGDirector.h>
#include <JXVarLenPGDirector.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <jAssert.h>

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
	itsProgressDirector = NULL;
	itsRaiseWindowFlag  = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStandAlonePG::~JXStandAlonePG()
{
	assert( itsProgressDirector == NULL );
}

/******************************************************************************
 ProcessBeginning (virtual protected)

 ******************************************************************************/

void
JXStandAlonePG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JCharacter*	message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	// create the window

	if (!allowBackground)
		{
		(JXGetApplication())->PrepareForBlockingWindow();
		}

	assert( itsProgressDirector == NULL );

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
	assert( itsProgressDirector != NULL );

	// find the first unused window position and use it

	itsWindowIndex       = 0;
	const JSize winCount = winPos.GetElementCount();
	for (JIndex i=1; i<=winCount; i++)
		{
		const JXPGWindowPosition winInfo = winPos.GetElement(i);
		if (winInfo.dir == NULL)
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

	Returns kJFalse if process was cancelled by user.

 ******************************************************************************/

JBoolean
JXStandAlonePG::ProcessContinuing()
{
	JXWindow* window = itsProgressDirector->GetWindow();
	if (itsProgressDirector->IsActive() && itsRaiseWindowFlag)
		{
		window->Raise(kJFalse);
		}

	if (itsStepCount == 0)
		{
		const JString s(GetCurrentStepCount(), JString::kBase10);
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
		while ((JXGetApplication())->HandleOneEventForWindow(window, kJFalse))
			{ };
		}

	(itsProgressDirector->GetDisplay())->Flush();

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
	winInfo.dir = NULL;
	winPos.SetElement(itsWindowIndex, winInfo);
	itsWindowIndex = 0;

	SetItems(NULL, NULL, NULL);

	assert( itsProgressDirector != NULL );
	itsProgressDirector->ProcessFinished();
	itsProgressDirector = NULL;

	if (!AllowBackground())
		{
		(JXGetApplication())->BlockingWindowFinished();
		}

	JXProgressDisplay::ProcessFinished();
}
