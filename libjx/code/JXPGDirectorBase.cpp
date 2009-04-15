/******************************************************************************
 JXPGDirectorBase.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPGDirectorBase.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXButton.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kWindowTitle = "Progress";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPGDirectorBase::JXPGDirectorBase
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsExpectsCloseFlag = kJFalse;
	itsCancelButton     = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPGDirectorBase::~JXPGDirectorBase()
{
}

/******************************************************************************
 ProcessContinuing

	Call this to update the window title.

 ******************************************************************************/

void
JXPGDirectorBase::ProcessContinuing
	(
	const JCharacter* value
	)
{
	JString s = value;
	s        += " - ";
	s        += kWindowTitle;
	(GetWindow())->SetTitle(s);
}

/******************************************************************************
 ProcessFinished

	Call this to close the director.

 ******************************************************************************/

void
JXPGDirectorBase::ProcessFinished()
{
	itsExpectsCloseFlag = kJTrue;
	const JBoolean ok = Close();
	assert( ok );
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXPGDirectorBase::Activate()
{
	// We need to be active, regardless of our supervisor's state!

	while (IsSuspended())
		{
		Resume();
		}

	JXWindowDirector::Activate();
}

/******************************************************************************
 Close (virtual)

	Pass unexpected close messages to our JXProgressDisplay as cancel messages.

 ******************************************************************************/

JBoolean
JXPGDirectorBase::Close()
{
	if (itsExpectsCloseFlag)
		{
		return JXWindowDirector::Close();
		}
	else if (itsCancelButton != NULL)
		{
		itsCancelButton->Push();
		return kJFalse;
		}
	else
		{
		(JGetUserNotification())->ReportError(
			"You cannot quit while this process is running.");
		return kJFalse;
		}
}

/******************************************************************************
 Init (protected)

 ******************************************************************************/

void
JXPGDirectorBase::Init
	(
	JXWindow*			window,
	JXStaticText*		text,
	const JCharacter*	message,
	const JBoolean		allowCancel,
	JXButton*			cancelButton
	)
{
	window->SetTitle(kWindowTitle);
	window->HideFromTaskbar();

	// instantaneous redraw when Raise()

	window->UsePixmapAsBackground(kJTrue);
	window->KeepBufferPixmap(kJTrue);

	// adjust window size to fit text

	text->SetText(message);

	const JRect apG = text->GetApertureGlobal();

	JSize dw          = 0;
	const JSize prefw = text->TEGetMinPreferredGUIWidth();
	const JSize apw   = apG.width();
	if (prefw > apw)
		{
		dw = prefw - apw;
		}

	JSize dh        = 0;
	const JSize bdh = text->GetBoundsHeight();
	const JSize aph = apG.height();
	if (bdh > aph)
		{
		dh = bdh - aph;
		}

	window->AdjustSize(dw, dh);
	window->LockCurrentSize();
	window->PlaceAsDialogWindow();

	// show cancel button, if allowed

	if (allowCancel)
		{
		itsCancelButton = cancelButton;
		}
	else
		{
		cancelButton->Hide();
		}
}
