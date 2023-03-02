/******************************************************************************
 JXModalDialogDirector.cpp

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

	The only difference between a modeless JXModalDialogDirector and a
	JXWindowDirector is that JXModalDialogDirector is transient by default
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

#include "JXModalDialogDirector.h"
#include "JXWindow.h"
#include "JXButton.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXModalDialogDirector::JXModalDialogDirector
	(
	const bool allowResizing
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsAllowResizeFlag(allowResizing),
	itsCancelFlag(false),
	itsOKButton(nullptr),
	itsCancelButton(nullptr),
	itsDoneFlag(false),
	itsDeletedFlag(nullptr)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXModalDialogDirector::~JXModalDialogDirector()
{
	if (itsDeletedFlag != nullptr)
	{
		*itsDeletedFlag = true;
	}
}

/******************************************************************************
 SetButtons

	If the window is not modal, okButton and cancelButton can be nullptr.
	If the window is modal, okButton must not be nullptr.

 ******************************************************************************/

void
JXModalDialogDirector::SetButtons
	(
	JXButton* okButton,
	JXButton* cancelButton
	)
{
	assert( okButton != nullptr );

	JXWindow* window = GetWindow();
	assert( window != nullptr );

	window->HideFromTaskbar();

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
 DoDialog

	Returns true if the dialog is ok'd.

 ******************************************************************************/

bool
JXModalDialogDirector::DoDialog()
{
	assert( JXApplication::IsWorkerFiber() );

	Activate();

	std::unique_lock lock(itsMutex);
	itsCondition.wait(lock, [this](){ return itsDoneFlag; });

	return !itsCancelFlag;
}

/******************************************************************************
 EndDialog

	If success is true, we close ourselves with success.
	Otherwise, we close as if cancelled.

	We don't bother to return the result of Deactivate() because, if it
	is successful, kJXDialogDeactivated will be broadcast.

 ******************************************************************************/

void
JXModalDialogDirector::EndDialog
	(
	const bool success
	)
{
	itsCancelFlag = !success;
	if (!Deactivate())
	{
		itsCancelFlag = true;		// so WM_DELETE_WINDOW => cancel
	}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXModalDialogDirector::Activate()
{
	if (!IsActive())
	{
		assert( itsOKButton != nullptr );

		JXWindow* window = GetWindow();
		assert( window != nullptr );
		window->SetCloseAction(JXWindow::kDeactivateDirector);
		window->ShouldFocusWhenShow(true);
		window->SetWMWindowType(JXWindow::kWMDialogType);

		window->PlaceAsDialogWindow();
		if (!itsAllowResizeFlag)
		{
			window->LockCurrentSize();
		}
		else
		{
			window->LockCurrentMinSize();
		}

		JXWindowDirector::Activate();
		if (IsActive())
		{
			itsCancelFlag = true;	// so WM_DELETE_WINDOW => cancel
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
 Deactivate (virtual)

 ******************************************************************************/

bool
JXModalDialogDirector::Deactivate()
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
		bool deleted   = false;
		itsDeletedFlag = &deleted;

		std::unique_lock lock(itsMutex);
		itsDoneFlag = true;
		lock.unlock();

		itsCondition.notify_one();
		boost::this_fiber::yield();

		if (!deleted)
		{
			itsDeletedFlag = nullptr;

			GetSupervisor()->Resume();

			const bool ok = Close();
			assert( ok );
		}
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
JXModalDialogDirector::OKToDeactivate()
{
	return itsCancelFlag || JXWindowDirector::OKToDeactivate();
}

/******************************************************************************
 Close (virtual)

	Close all sub-directors and delete ourselves.

 ******************************************************************************/

bool
JXModalDialogDirector::Close()
{
	if (IsActive())
	{
		EndDialog(false);
		return true;
	}
	else
	{
		return JXWindowDirector::Close();
	}
}

/******************************************************************************
 Receive (protected)

	Listen for pushed ok and cancel buttons.

 ******************************************************************************/

void
JXModalDialogDirector::Receive
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
