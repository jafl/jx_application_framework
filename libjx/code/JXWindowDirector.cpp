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

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
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
	assert( supervisor != NULL );
	itsWindow = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWindowDirector::~JXWindowDirector()
{
	// call GetDisplay() before deleting window!

	JXWDManager* mgr;
	if ((GetDisplay())->GetWDManager(&mgr))
		{
		mgr->DirectorDeleted(this);
		}

	if (itsWindow != NULL && !itsWindow->IsIconified())
		{
		itsWindow->Hide();
		}
	delete itsWindow;
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
 GetColormap

 ******************************************************************************/

JXColormap*
JXWindowDirector::GetColormap()
	const
{
	return itsWindow->GetColormap();
}

/******************************************************************************
 SetWindow (protected)

 ******************************************************************************/

void
JXWindowDirector::SetWindow
	(
	JXWindow* window
	)
{
	assert( itsWindow == NULL && window != NULL );
	itsWindow = window;

	if (IsSuspended())
		{
		PrivateSuspend();
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
		if (itsWindow != NULL)
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
		if (itsWindow != NULL)
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
		itsWindow == NULL || itsWindow->OKToUnfocusCurrentWidget());
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXWindowDirector::Suspend()
{
	JXDirector::Suspend();
	if (itsWindow != NULL && IsSuspended())
		{
		PrivateSuspend();
		}
}

/******************************************************************************
 PrivateSuspend (private)

 ******************************************************************************/

void
JXWindowDirector::PrivateSuspend()
{
	itsWindow->Deactivate();
}

/******************************************************************************
 Resume (virtual)

 ******************************************************************************/

void
JXWindowDirector::Resume()
{
	JXDirector::Resume();
	if (itsWindow != NULL && !IsSuspended())
		{
		itsWindow->Activate();
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
	return (GetWindow())->GetTitle();
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
	*icon = NULL;
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
