/******************************************************************************
 JXDialogDirector.cpp

	Maintains a dialog window.  If we are modal, we suspend our supervisor
	when activated and resume it when we are deactivated.

	If we successfully deactivate or are cancelled, we broadcast
	kJXDialogDeactivated.  The client can then extract the information.
	After successfully deactivating, we Close() ourselves.  By taking
	this responsibility ourselves, we allow any number of objects to
	safely ListenTo() us, because there is no danger that one will Close()
	us before another gets the message.

	Note that this means that modeless dialog windows that are supposed
	to simply hide themselves instead of closing must be implemented as
	JXWindowDirectors intead of JXDialogDirectors.

	If the client wants actions other than OK and Cancel, it merely has to
	add extra buttons and ListenTo() each one.  If part of the action should
	be to close the dialog, the client can call EndDialog().  Since this
	generates kJXDialogDeactivated, extracting the data only has to be
	done one place: in the "deactivated" message handler.

	The only difference between a modeless JXDialogDirector and a
	JXWindowDirector is that JXDialogDirector is transient by default
	and notifies you when it is closed.

	Derived classes that implement modal windows are required to call
	SetButtons() with at least an okButton.

	Derived classes can also implement extra validation (e.g. one input field
	greater than another) by overriding OKToDeactivate().  One should only
	perform such checks if Cancelled() returns false, however, because
	nothing should be checked if the user cancels the dialog.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXDialogDirector.h"
#include "JXWindow.h"
#include "JXButton.h"
#include "jXGlobals.h"
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JXDialogDirector::kDeactivated = "Deactivated::JXDialogDirector";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDialogDirector::JXDialogDirector
	(
	JXDirector*		supervisor,
	const bool	modal
	)
	:
	JXWindowDirector(supervisor),
	itsModalFlag(modal)
{
	itsAutoGeomFlag = modal;
	itsCancelFlag   = false;
	itsOKButton     = nullptr;
	itsCancelButton = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDialogDirector::~JXDialogDirector()
{
}

/******************************************************************************
 SetButtons

	If the window is not modal, okButton and cancelButton can be nullptr.
	If the window is modal, okButton must not be nullptr.

 ******************************************************************************/

void
JXDialogDirector::SetButtons
	(
	JXButton* okButton,
	JXButton* cancelButton
	)
{
	assert( !itsModalFlag || okButton != nullptr );

	JXWindow* window = GetWindow();
	assert( window != nullptr );

	if (itsModalFlag)
		{
		window->HideFromTaskbar();
		}

	itsOKButton = okButton;
	if (itsOKButton != nullptr)
		{
		ListenTo(itsOKButton);
		}

	itsCancelButton = cancelButton;
	if (itsCancelButton != nullptr)
		{
		ListenTo(itsCancelButton);
		window->InstallShortcuts(itsCancelButton, JGetString("CancelShortcut::JXGlobal"));
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXDialogDirector::Activate()
{
	if (!IsActive())
		{
		assert( !itsModalFlag || itsOKButton != nullptr );

		JXWindow* window = GetWindow();
		assert( window != nullptr );
		window->SetCloseAction(JXWindow::kDeactivateDirector);
		window->ShouldFocusWhenShow(true);

		JXDirector* supervisor = GetSupervisor();
		if (supervisor->IsWindowDirector())
			{
			auto* windowDir =
				dynamic_cast<JXWindowDirector*>(supervisor);
			assert( windowDir != nullptr );
			window->SetTransientFor(windowDir);
			}

		if (itsModalFlag)
			{
			window->SetWMWindowType(JXWindow::kWMDialogType);
			}

		if (itsAutoGeomFlag)
			{
			window->PlaceAsDialogWindow();
			window->LockCurrentSize();
			}

		JXWindowDirector::Activate();
		if (IsActive())
			{
			itsCancelFlag = true;		// so WM_DELETE_WINDOW => cancel
			if (itsModalFlag)
				{
				supervisor->Suspend();
				}

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
 Deactivate (virtual)

 ******************************************************************************/

bool
JXDialogDirector::Deactivate()
{
	if (!IsActive())
		{
		return true;
		}

	if (itsCancelFlag)
		{
		GetWindow()->KillFocus();
		}

	if (JXWindowDirector::Deactivate())
		{
		const bool success = !itsCancelFlag;
		Broadcast(JXDialogDirector::Deactivated(success));
		if (itsModalFlag)
			{
			GetSupervisor()->Resume();
			}
		const bool ok = Close();
		assert( ok );
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JXDialogDirector::OKToDeactivate()
{
	return itsCancelFlag || JXWindowDirector::OKToDeactivate();
}

/******************************************************************************
 Receive (protected)

	Listen for pushed ok and cancel buttons.

 ******************************************************************************/

void
JXDialogDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsOKButton && message.Is(JXButton::kPushed))
		{
		EndDialog(true);
		}
	else if (sender == itsCancelButton && message.Is(JXButton::kPushed))
		{
		EndDialog(false);
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}
