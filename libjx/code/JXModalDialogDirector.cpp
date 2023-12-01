/******************************************************************************
 JXModalDialogDirector.cpp

	Maintains a modal dialog window.  Call DoDialog() to show the window.
	We suspend all other windows while active.  If DoDialog() returns true,
	the client can extract the information.  We Close() automatically.

	Modeless dialog windows that are supposed to simply hide themselves
	instead of closing must be implemented as JXWindowDirectors intead of
	JXModalDialogDirectors.

	If the client wants actions other than OK and Cancel, it merely has to
	add extra buttons and ListenTo() each one.  If part of the action should
	be to close the dialog, the client can call EndDialog().

	Derived classes that implement modal windows are required to call
	SetButtons() with at least an okButton.

	Derived classes can also implement extra validation (e.g. one input field
	greater than another) by overriding OKToDeactivate().  One should only
	perform such checks if Cancelled() returns false, however, because
	nothing should be checked if the user cancels the dialog.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996-2023 by John Lindal.

 ******************************************************************************/

#include "JXModalDialogDirector.h"
#include "JXWindow.h"
#include "JXButton.h"
#include "JXInputField.h"
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
	itsExecutionContext(nullptr),
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
	itsExecutionContext = boost::fibers::context::active();

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
		// else, assume app locks appropriate min size and then restores from saved state

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

		assert( itsExecutionContext != nullptr );
		itsExecutionContext->join();	// wait until "done" processing finishes
		itsExecutionContext = nullptr;

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
	if (itsCancelFlag)
	{
		return true;
	}
	else if (!JXWindowDirector::OKToDeactivate())
	{
		return false;
	}

	bool ok = true;
	GetWindow()->ForEach([this, &ok](JXContainer* obj)
	{
		if (!ok)
		{
			return;
		}

		auto* f = dynamic_cast<JXInputField*>(obj);
		if (f != nullptr && !f->InputValid())
		{
			ok = false;
			Resume();
			f->Focus();
		}
	},
	true);

	return ok;
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
