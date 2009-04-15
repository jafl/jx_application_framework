/******************************************************************************
 JXDNDManager_Win32.cpp

	Global object to manage Drag-And-Drop interactions.

	When the drop is intra-application, we simply call the Widget directly.
	When the drop is inter-application, we send ClientMessages.

	BASE CLASS = virtual JBroadcaster

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDNDManager.h>
#include <JXDNDChooseDropActionDialog.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWidget.h>
#include <jXGlobals.h>
#include <jXUtil.h>
#include <JMinMax.h>
#include <jTime.h>
#include <jAssert.h>

#define JXDND_DEBUG_MSGS	0	// boolean
#define JXDND_SOURCE_DELAY	0	// time in seconds
#define JXDND_TARGET_DELAY	0	// time in seconds

const JCoordinate kHysteresisBorderWidth = 50;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDNDManager::JXDNDManager
	(	
	JXDisplay* display
	)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDNDManager::~JXDNDManager()
{
}

/******************************************************************************
 BeginDND

	Returns kJFalse if we are unable to initiate DND.

	We do not take ownership of targetFinder.

 ******************************************************************************/

JBoolean
JXDNDManager::BeginDND
	(
	JXWidget*				widget,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JXSelectionData*		data,
	TargetFinder*			targetFinder
	)
{
	return kJFalse;
}

/******************************************************************************
 HandleDND

 ******************************************************************************/

void
JXDNDManager::HandleDND
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 FinishDND

 ******************************************************************************/

void
JXDNDManager::FinishDND()
{
}

/******************************************************************************
 CancelDND

	Returns kJTrue if the cancel was during a drag.

 ******************************************************************************/

JBoolean
JXDNDManager::CancelDND()
{
	return kJFalse;
}

/******************************************************************************
 EnableDND

	Creates the XdndAware property on the specified window.

	We cannot include a list of data types because JXWindows contain
	many widgets.

 ******************************************************************************/

void
JXDNDManager::EnableDND
	(
	const Window xWindow
	)
	const
{
}

/******************************************************************************
 IsDNDAware

	Returns kJTrue if the given X window supports the XDND protocol.

	*proxy is the window to which the client messages should be sent.
	*vers is the version to use.

 ******************************************************************************/

JBoolean
JXDNDManager::IsDNDAware
	(
	const Window	xWindow,
	Window*			proxy,
	JSize*			vers
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 GetAskActions

	If successful, returns a list of the actions supported by the source.
	The caller must be prepared for this to fail by supporting a fallback
	operation.

	This function is not hidden inside ChooseDropAction() because the target
	may want to modify the list before presenting it to the user.

 ******************************************************************************/

JBoolean
JXDNDManager::GetAskActions
	(
	JArray<Atom>*		actionList,
	JPtrArray<JString>*	descriptionList
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 ChooseDropAction

	Asks the user which action to perform.  Returns kJFalse if cancelled.

	If the initial value of *action is one of the elements in actionList,
	the corresponding radio button becomes the initial choice.

 ******************************************************************************/

JBoolean
JXDNDManager::ChooseDropAction
	(
	const JArray<Atom>&			actionList,
	const JPtrArray<JString>&	descriptionList,
	Atom*						action
	)
{
	return kJFalse;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXDNDManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseDropActionDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			*itsUserDropAction = itsChooseDropActionDialog->GetAction();
			}
		else
			{
			*itsUserDropAction = None;
			}
		itsChooseDropActionDialog = NULL;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 HandleClientMessage 

 ******************************************************************************/

JBoolean
JXDNDManager::HandleClientMessage
	(
	const XClientMessageEvent& clientMessage
	)
{
	return kJFalse;
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

	This catches:
		1)  target crashes
		2)  source crashes while a message is in the network

 ******************************************************************************/

void
JXDNDManager::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	JBroadcaster::ReceiveWithFeedback(sender, message);
}

/******************************************************************************
 HandleDestroyNotify

	This catches source crashes while the source is processing XdndStatus.

 ******************************************************************************/

JBoolean
JXDNDManager::HandleDestroyNotify
	(
	const XDestroyWindowEvent& xEvent
	)
{
	return kJFalse;
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	This catches destruction of source and target windows.  It is safe to
	do work here because JXDNDManager is deleted by JXDisplay after all
	windows are gone.

 ******************************************************************************/

void
JXDNDManager::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	JBroadcaster::ReceiveGoingAway(sender);
}

/******************************************************************************
 GetDNDCursor

	Returns the appropriate cursor for the given response.

	cursor must be an array of 4 cursors:  copy, move, link, ask

 ******************************************************************************/

JCursorIndex
JXDNDManager::GetDNDCursor
	(
	const JBoolean		dropAccepted,
	const Atom			action,
	const JCursorIndex*	cursor
	)
	const
{
	return kJXDefaultCursor;
}

/******************************************************************************
 JXDNDManager::TargetFinder

	This defines the interface for all objects that can locate a DND target.

	Derived classes must override FindTarget().  The return values are:

		return JBoolean		target supports XDND
		*xWindow			window containing mouse
		*msgWindow			window to which DND messages should be sent
		*target				JXContainer containing mouse, if any
		*vers				XDND version supported by target

	msgWindow and vers can be obtained from JXDNDManager::IsDNDAware().

 ******************************************************************************/

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDNDManager::TargetFinder::TargetFinder
	(
	JXDisplay* display
	)
	:
	itsDisplay(display)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDNDManager::TargetFinder::~TargetFinder()
{
}

/******************************************************************************
 GetSelectionManager

 ******************************************************************************/

JXSelectionManager*
JXDNDManager::TargetFinder::GetSelectionManager()
	const
{
	return itsDisplay->GetSelectionManager();
}

/******************************************************************************
 GetDNDManager

 ******************************************************************************/

JXDNDManager*
JXDNDManager::TargetFinder::GetDNDManager()
	const
{
	return itsDisplay->GetDNDManager();
}
