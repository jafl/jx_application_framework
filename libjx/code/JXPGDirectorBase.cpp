/******************************************************************************
 JXPGDirectorBase.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997-2023 by John Lindal.

 ******************************************************************************/

#include "JXPGDirectorBase.h"
#include "JXWindow.h"
#include "JXStaticText.h"
#include "JXButton.h"
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPGDirectorBase::JXPGDirectorBase
	(
	const bool modal
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsModalFlag(modal),
	itsExpectsCloseFlag(false),
	itsCancelButton(nullptr)
{
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
	const JString& value
	)
{
	JString s = value;
	s        += " - ";
	s        += JGetString("WindowTitle::JXPGDirectorBase");
	GetWindow()->SetTitle(s);
}

/******************************************************************************
 ProcessFinished

	Call this to close the director.

 ******************************************************************************/

void
JXPGDirectorBase::ProcessFinished()
{
	itsExpectsCloseFlag = true;
	const bool ok = Close();
	assert( ok );
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXPGDirectorBase::Activate()
{
	if (!IsActive())
	{
		JXWindow* window = GetWindow();
		assert( window != nullptr );
		window->ShouldFocusWhenShow(true);
		window->SetWMWindowType(JXWindow::kWMDialogType);
		window->PlaceAsDialogWindow();

		JXWindowDirector::Activate();
		if (IsActive() && itsModalFlag)
		{
			GetSupervisor()->Suspend();

			while (IsSuspended())
			{
				Resume();			// we need to be active
			}
		}
	}
	else
	{
		JXWindowDirector::Activate();
	}
}

/******************************************************************************
 Close (virtual)

	Pass unexpected close messages to our JXProgressDisplay as cancel messages.

 ******************************************************************************/

bool
JXPGDirectorBase::Close()
{
	if (itsExpectsCloseFlag)
	{
		if (itsModalFlag)
		{
			GetSupervisor()->Resume();
		}
		return JXWindowDirector::Close();
	}
	else if (itsCancelButton != nullptr)
	{
		itsCancelButton->Push();
		return false;
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("CannotQuitWhileRunning::JXPGDirectorBase"));
		return false;
	}
}

/******************************************************************************
 Init (protected)

 ******************************************************************************/

void
JXPGDirectorBase::Init
	(
	JXWindow*		window,
	JXStaticText*	text,
	const JString&	message,
	const bool		allowCancel,
	JXButton*		cancelButton
	)
{
	window->HideFromTaskbar();

	// instantaneous redraw when Raise()

	window->UsePixmapAsBackground(true);
	window->KeepBufferPixmap(true);

	// adjust window size to fit text

	text->GetText()->SetText(message);

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
