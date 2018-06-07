/******************************************************************************
 JXWindowDirector.cpp

	Maintains a window and only a window.  If you need to maintain data as
	well, derive your class from JXDocument so you can override OKToClose()
	to give the user a chance to save the data before it is tossed.

	Close() does not call OKToDeactivate() because we assume that any information
	displayed in the window is either not supposed to be saved or has already
	been saved by the object calling Close().  If a JXDocument wants to
	display information in sub-windows, each of these sub-windows should be
	set to JXWindow::kDeactivateDirector so only the JXDocument will toss
	the sub-window.

	OKToDeactivate() checks that everything in the window is valid.  The default
	is to check whether the object with focus is willing to unfocus.  If
	this is not enough, the derived class can override the method.

	BASE CLASS = JXDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXWindowDirector.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWDManager.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWindowDirector::JXWindowDirector
	(
	JXDirector* supervisor
	)
	:
	JXDirector(supervisor)
{
	assert( supervisor != nullptr );
	itsWindow = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWindowDirector::~JXWindowDirector()
{
	if (itsWindow != nullptr)
		{
		// call GetDisplay() before deleting window!

		JXWDManager* mgr;
		if (GetDisplay()->GetWDManager(&mgr))
			{
			mgr->DirectorDeleted(this);
			}

		if (!itsWindow->IsIconified())
			{
			itsWindow->Hide();
			}
		jdelete itsWindow;
		}
}

/******************************************************************************
 IsWindowDirector (virtual)

 ******************************************************************************/

JBoolean
JXWindowDirector::IsWindowDirector()
	const
{
	return kJTrue;
}

/******************************************************************************
 GetDisplay

 ******************************************************************************/

JXDisplay*
JXWindowDirector::GetDisplay()
	const
{
	return itsWindow->GetDisplay();
}

/******************************************************************************
 SetWindow (private)

 ******************************************************************************/

void
JXWindowDirector::SetWindow
	(
	JXWindow* window
	)
{
	assert( itsWindow == nullptr && window != nullptr );
	itsWindow = window;

	if (IsSuspended())
		{
		itsWindow->Suspend();
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXWindowDirector::Activate()
{
	if (!IsActive())
		{
		JXDirector::Activate();
		if (itsWindow != nullptr)
			{
			itsWindow->Show();
			itsWindow->Activate();
			}
		}
	else
		{
		itsWindow->Raise();
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

JBoolean
JXWindowDirector::Deactivate()
{
	if (!IsActive())
		{
		return kJTrue;
		}
	else if (OKToDeactivate() && JXDirector::Deactivate())
		{
		if (itsWindow != nullptr)
			{
			itsWindow->Hide();
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Check that the active object is willing to unfocus.

 ******************************************************************************/

JBoolean
JXWindowDirector::OKToDeactivate()
{
	return JConvertToBoolean(
		itsWindow == nullptr || itsWindow->OKToUnfocusCurrentWidget());
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXWindowDirector::Suspend()
{
	JXDirector::Suspend();
	if (itsWindow != nullptr)
		{
		itsWindow->Suspend();
		}
}

/******************************************************************************
 Resume (virtual)

 ******************************************************************************/

void
JXWindowDirector::Resume()
{
	JXDirector::Resume();
	if (itsWindow != nullptr)
		{
		itsWindow->Resume();
		}
}

/******************************************************************************
 GetName (virtual)

	Return a name for the director.  The default is to use the window title.

 ******************************************************************************/

const JString&
JXWindowDirector::GetName()
	const
{
	return GetWindow()->GetTitle();
}

/******************************************************************************
 GetMenuIcon (virtual)

	Return an icon to display in JXWDMenu.

	Does not return JXImage* because that would force all derived classes
	to provide an icon, and many don't need it.

	The client retains ownership of the JXImage.

 ******************************************************************************/

JBoolean
JXWindowDirector::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = nullptr;
	return kJFalse;
}

/******************************************************************************
 NeedsSave (virtual)

	Only needed by JXDocuments.

 ******************************************************************************/

JBoolean
JXWindowDirector::NeedsSave()
	const
{
	return kJFalse;
}
