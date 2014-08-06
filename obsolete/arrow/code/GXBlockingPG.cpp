/******************************************************************************
 GXBlockingPG.cc

	Class to display the progress of a long process in a client window.
	The arguments to the constructor are passed to SetItems().  When a
	process begins, the window containing these widgets is deactivated.

	While the process is running, we call
	JXApplication::ProcessOneEventForWindow() so the window is redrawn.

	BASE CLASS = JXProgressDisplay

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <GXBlockingPG.h>

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXProgressIndicator.h>

#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GXBlockingPG::GXBlockingPG
	(
	JXTextButton*			cancelButton,
	JXStaticText*			counter,
	JXProgressIndicator*	indicator
	)
	:
	JXProgressDisplay(),
	itsWindow(NULL)
{
	if (counter != NULL)
		{
		itsWindow = counter->GetWindow();
		}
	else if (indicator != NULL)
		{
		itsWindow = indicator->GetWindow();
		}
	assert( itsWindow != NULL );

	SetItems(cancelButton, counter, indicator);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXBlockingPG::~GXBlockingPG()
{
}

/******************************************************************************
 ProcessBeginning (virtual protected)

 ******************************************************************************/

void
GXBlockingPG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JCharacter*	message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	itsWindow->Deactivate();
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
GXBlockingPG::ProcessContinuing()
{
//	while ((JXGetApplication())->HandleOneEventForWindow(itsWindow, NULL, kJFalse))
//		{ };
	itsWindow->CheckForMapOrExpose();
	itsWindow->Update();
	(itsWindow->GetDisplay())->Flush();

	return JXProgressDisplay::ProcessContinuing();
}

/******************************************************************************
 ProcessFinished (virtual)

 ******************************************************************************/

void
GXBlockingPG::ProcessFinished()
{
	itsWindow->Activate();
	JXProgressDisplay::ProcessFinished();
}
