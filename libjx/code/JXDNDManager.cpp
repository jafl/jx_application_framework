/******************************************************************************
 JXDNDManager.cpp

	Global object to manage Drag-And-Drop interactions.

	When the drop is intra-application, we simply call the Widget directly.
	When the drop is inter-application, we send ClientMessages.

	Refer to http://johnlindal.wixsite.com/xdnd for the complete protocol.

	When we are the source, itsDragger, itsDraggerWindow, and itsDraggerTypeList
	refer to the source Widget.  If the mouse is in a local window, itsMouseContainer
	points to the Widget that the mouse is in.  If the mouse is in the window
	of another application, itsMouseWindow is the XID of the window.

	When we are the target (always inter-application), itsDraggerWindow
	refers to the source window.  itsDraggerTypeList is the list of types from
	the source.  itsMouseWindow refers to the target window, and itsMouseContainer
	is the Widget that the mouse is in.  Since we only receive window-level
	XdndEnter and XdndLeave messages, we have to manually generate DNDEnter()
	and DNDLeave() calls to each widget.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1997-2009 by John Lindal.

 ******************************************************************************/

#include "JXDNDManager.h"
#include "JXDNDChooseDropActionDialog.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXWidget.h"
#include "jXGlobals.h"
#include "jXUtil.h"
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

#define JXDND_DEBUG_MSGS	0	// boolean
#define JXDND_SOURCE_DELAY	0	// time in seconds
#define JXDND_TARGET_DELAY	0	// time in seconds

const Atom kCurrentDNDVersion = 4;
const Atom kMinDNDVersion     = 3;

const clock_t kWaitForLastStatusTime = 10 * CLOCKS_PER_SEC;

// atom names

static const JUtf8Byte* kAtomNames[ JXDNDManager::kAtomCount ] =
{
	"XdndSelection",

	"XdndProxy",
	"XdndAware",
	"XdndTypeList",

	"XdndEnter",
	"XdndPosition",
	"XdndStatus",
	"XdndLeave",
	"XdndDrop",
	"XdndFinished",

	"XdndActionCopy",
	"XdndActionMove",
	"XdndActionLink",
	"XdndActionAsk",
	"XdndActionPrivate",
	"XdndActionDirectSave",

	"XdndActionList",
	"XdndActionDescription",

	"XdndDirectSave0"
};

// message data

enum
{
	kDNDEnterTypeCount = 3,	// max number of types in XdndEnter message

// DNDEnter

	kDNDEnterWindow = 0,	// source window (sender)
	kDNDEnterFlags,			// version in high byte, bit 0 => more data types
	kDNDEnterType1,			// first available data type
	kDNDEnterType2,			// second available data type
	kDNDEnterType3,			// third available data type

// DNDEnter flags

	kDNDEnterMoreTypesFlag = 1,		// set if there are more than kDNDEnterTypeCount
	kDNDEnterVersionRShift = 24,	// right shift to position version number
	kDNDEnterVersionMask   = 0xFF,	// mask to get version after shifting

// DNDHere

	kDNDHereWindow = 0,		// source window (sender)
	kDNDHereFlags,			// reserved
	kDNDHerePt,				// x,y coords of mouse (root window coords)
	kDNDHereTimeStamp,		// timestamp for requesting data
	kDNDHereAction,			// action requested by user

// DNDStatus

	kDNDStatusWindow = 0,	// target window (sender)
	kDNDStatusFlags,		// flags returned by target
	kDNDStatusPt,			// x,y of "no msg" rectangle (root window coords)
	kDNDStatusArea,			// w,h of "no msg" rectangle
	kDNDStatusAction,		// action accepted by target

// DNDStatus flags

	kDNDStatusAcceptDropFlag = 1,	// set if target will accept the drop
	kDNDStatusSendHereFlag   = 2,	// set if target wants stream of XdndPosition

// DNDLeave

	kDNDLeaveWindow = 0,	// source window (sender)
	kDNDLeaveFlags,			// reserved

// DNDDrop

	kDNDDropWindow = 0,		// source window (sender)
	kDNDDropFlags,			// reserved
	kDNDDropTimeStamp,		// timestamp for requesting data

// DNDFinished

	kDNDFinishedWindow = 0,	// target window (sender)
	kDNDFinishedFlags		// reserved
};

const JUInt32 kDNDScrollTargetMask = 0x00000400;
const JUInt32 kDNDScrollButtonMask = 0x00000300;
const JSize kDNDScrollButtonShift  = 8;
const JUInt32 kDNDScrollModsMask   = 0x000000FF;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDNDManager::JXDNDManager
	(	
	JXDisplay* display
	)
	:
	itsPrevHandleDNDModifiers(display)
{
	itsDisplay            = display;
	itsIsDraggingFlag     = false;
	itsDragger            = nullptr;
	itsDraggerWindow      = None;
	itsTargetFinder       = nullptr;
	itsMouseWindow        = None;
	itsMouseWindowIsAware = false;
	itsMouseContainer     = nullptr;
	itsMsgWindow          = None;

	itsDraggerTypeList = jnew JArray<Atom>;
	assert( itsDraggerTypeList != nullptr );

	itsDraggerAskActionList = jnew JArray<Atom>;
	assert( itsDraggerAskActionList != nullptr );

	itsDraggerAskDescripList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsDraggerAskDescripList != nullptr );

	itsSentFakePasteFlag = false;

	InitCursors();

	itsDisplay->RegisterXAtoms(kAtomCount, kAtomNames, itsAtoms);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDNDManager::~JXDNDManager()
{
	jdelete itsDraggerTypeList;
	jdelete itsDraggerAskActionList;
	jdelete itsDraggerAskDescripList;
}

/******************************************************************************
 GetCurrentDNDVersion

 ******************************************************************************/

Atom
JXDNDManager::GetCurrentDNDVersion()
	const
{
	return kCurrentDNDVersion;
}

/******************************************************************************
 BeginDND

	Returns false if we are unable to initiate DND.

	We do not take ownership of targetFinder.

 ******************************************************************************/

bool
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
	if (itsDragger != nullptr)
	{
		return false;
	}

	assert( (widget->GetWindow())->IsDragging() );

	if (itsMouseContainer != nullptr)
	{
		StopListening(itsMouseContainer);
	}

	itsMouseWindow               = None;
	itsMouseWindowIsAware        = false;
	itsMouseContainer            = nullptr;
	itsMsgWindow                 = None;
	itsPrevHandleDNDAction       = None;
	itsPrevHandleDNDScrollButton = (JXMouseButton) 0;
	itsPrevHandleDNDModifiers.Clear();

	if (itsDisplay->GetSelectionManager()->SetData(itsAtoms[ kDNDSelectionAtomIndex ], data))
	{
		itsIsDraggingFlag   = true;
		itsDragger          = widget;
		itsDraggerWindow    = (itsDragger->GetWindow())->GetXWindow();
		*itsDraggerTypeList = data->GetTypeList();
		itsTargetFinder     = targetFinder;

		ListenTo(itsDragger);
		itsDragger->BecomeDNDSource();
		itsDragger->DNDInit(pt, buttonStates, modifiers);
		itsDragger->HandleDNDResponse(nullptr, false, None);		// set initial cursor

		AnnounceTypeList(itsDraggerWindow, *itsDraggerTypeList);

		HandleDND(pt, buttonStates, modifiers, (JXMouseButton) 0);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 HandleDND

	scrollButton can be 4,5,6,7

 ******************************************************************************/

void
JXDNDManager::HandleDND
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	const JXMouseButton		scrollButton
	)
{
	assert( itsDragger != nullptr );

	JXContainer* dropWidget;
	Window xWindow, msgWindow;
	Atom dndVers;
	const bool isDNDAware =
		FindTarget(itsDragger, pt, &xWindow, &msgWindow, &dropWidget, &dndVers);

	// check if we have entered a different drop target

	if (xWindow != itsMouseWindow || dropWidget != itsMouseContainer)
	{
		SendDNDLeave();
		SendDNDEnter(xWindow, msgWindow, dropWidget, isDNDAware, dndVers);
	}

	// get the user's preferred action

	const Atom dropAction =
		itsDragger->GetDNDAction(itsMouseContainer, buttonStates, modifiers);
	if (dropAction == itsAtoms[ kDNDActionAskAtomIndex ] &&
		itsPrevHandleDNDAction != itsAtoms[ kDNDActionAskAtomIndex ])
	{
		AnnounceAskActions(buttonStates, modifiers);
	}

	// contact the target

	itsPrevHandleDNDPt           = pt;
	itsPrevHandleDNDAction       = dropAction;
	itsPrevHandleDNDScrollButton = scrollButton;
	itsPrevHandleDNDModifiers    = modifiers;
	SendDNDHere(pt, dropAction, scrollButton, modifiers);

	// slow to receive next mouse event

	#if JXDND_SOURCE_DELAY > 0
	JWait(JXDND_SOURCE_DELAY);
	#endif
}

/******************************************************************************
 FindTarget (private)

	Searches for XdndAware window under the cursor.

	We have to search the tree all the way to the leaf because we have to
	pass through whatever layers the window manager has inserted.

	We have to check from scratch every time instead of just checking the
	bounds of itsMouseWindow because windows can overlap.

 ******************************************************************************/

bool
JXDNDManager::FindTarget
	(
	const JXContainer*	coordOwner,
	const JPoint&		pt,
	Window*				xWindow,
	Window*				msgWindow,
	JXContainer**		target,
	Atom*				vers
	)
	const
{
	bool isAware = false;
	*xWindow         = None;
	*msgWindow       = None;
	*target          = nullptr;
	*vers            = 0;

	if (itsTargetFinder != nullptr)
	{
		return itsTargetFinder->FindTarget(coordOwner, pt, xWindow, msgWindow,
										   target, vers);
	}

	JPoint ptG              = coordOwner->LocalToGlobal(pt);
	const Window rootWindow = itsDisplay->GetRootWindow();

	Window xWindow1 = (coordOwner->GetWindow())->GetXWindow();
	Window xWindow2 = rootWindow;
	Window childWindow;
	int x1 = ptG.x, y1 = ptG.y, x2,y2;
	while (XTranslateCoordinates(*itsDisplay, xWindow1, xWindow2,
								 x1, y1, &x2, &y2, &childWindow))
	{
		if (xWindow2 != rootWindow &&
			IsDNDAware(xWindow2, msgWindow, vers))
		{
			bool isDock = false;		// not required for reference impl

			XClassHint hint;
			if (XGetClassHint(*itsDisplay, xWindow2, &hint))
			{
				if (hint.res_name != nullptr &&
					strcmp(hint.res_name, JXGetDockWindowClass()) == 0)
				{
					isDock = true;
				}

				XFree(hint.res_name);
				XFree(hint.res_class);
			}

			// We need to drill through the dock to look for child windows,
			// but if there are none, then let the target be the dock, so
			// we can switch tabs.

			if (!isDock || childWindow == None)
			{
				*xWindow = xWindow2;
				isAware  = true;
				ptG.Set(x2, y2);
				break;
			}
		}

		if (childWindow == None)
		{
			*xWindow   = xWindow2;
			*msgWindow = xWindow2;
			isAware    = false;
			*vers      = 0;
			ptG.Set(x2, y2);
			break;
		}

		xWindow1 = xWindow2;
		xWindow2 = childWindow;
		x1       = x2;
		y1       = y2;
	}

	assert( *xWindow != None && *msgWindow != None );

	JXWindow* window;
	if (isAware && itsDisplay->FindXWindow(*xWindow, &window))
	{
		// If a local window is deactivated, FindContainer() returns false.
		// To avoid sending ourselves client messages, we have to treat
		// this as a non-XdndAware window.

		isAware = window->FindContainer(ptG, target);
	}

	return isAware;
}

/******************************************************************************
 FinishDND

 ******************************************************************************/

void
JXDNDManager::FinishDND()
{
	if (itsDragger != nullptr)
	{
		itsIsDraggingFlag = false;		// don't grab ESC any longer

		if (WaitForLastStatusMsg())
		{
			SendDNDDrop();
		}
		else
		{
			SendDNDLeave(true);
		}

		CleanUpAfterDND();
	}
}

/******************************************************************************
 CancelDND

	Returns true if the cancel was during a drag.

 ******************************************************************************/

bool
JXDNDManager::CancelDND()
{
	if (itsIsDraggingFlag)	// can't use IsDragging() because called if dragger deleted
	{
		if (itsDragger != nullptr)
		{
			itsDragger->DNDFinish(false, nullptr);
		}
		SendDNDLeave();
		CleanUpAfterDND();
		return true;
	}

	return false;
}

/******************************************************************************
 CleanUpAfterDND (private)

 ******************************************************************************/

void
JXDNDManager::CleanUpAfterDND()
{
	// need to check itsMouseContainer because if drop in other app,
	// the data has not yet been transferred

	if (itsDragger != nullptr && itsMouseContainer != nullptr)
	{
		itsDragger->DNDCompletelyFinished();
	}
	if (itsDragger != nullptr)
	{
		itsDragger->FinishDNDSource();
		StopListening(itsDragger);
	}
	if (itsMouseContainer != nullptr)
	{
		StopListening(itsMouseContainer);
	}

	itsIsDraggingFlag     = false;
	itsDragger            = nullptr;
	itsDraggerWindow      = None;
	itsTargetFinder       = nullptr;
	itsMouseWindow        = None;
	itsMouseWindowIsAware = false;
	itsMouseContainer     = nullptr;
	itsMsgWindow          = None;
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
	XChangeProperty(*itsDisplay, xWindow,
					itsAtoms[ kDNDAwareAtomIndex ], XA_ATOM, 32,
					PropModeReplace,
					(unsigned char*) &kCurrentDNDVersion, 1);
}

/******************************************************************************
 IsDNDAware

	Returns true if the given X window supports the XDND protocol.

	*proxy is the window to which the client messages should be sent.
	*vers is the version to use.

 ******************************************************************************/

bool
JXDNDManager::IsDNDAware
	(
	const Window	xWindow,
	Window*			proxy,
	JSize*			vers
	)
	const
{
	bool result = false;
	*proxy          = xWindow;
	*vers           = 0;

	// check XdndProxy

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* rawData = nullptr;
	XGetWindowProperty(*itsDisplay, xWindow, itsAtoms[ kDNDProxyAtomIndex ],
					   0, LONG_MAX, False, XA_WINDOW,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &rawData);

	if (actualType == XA_WINDOW && actualFormat == 32 && itemCount > 0)
	{
		*proxy = *(reinterpret_cast<Window*>(rawData));

		XFree(rawData);
		rawData = nullptr;

		// check XdndProxy on proxy window -- must point to itself

		XGetWindowProperty(*itsDisplay, *proxy, itsAtoms[ kDNDProxyAtomIndex ],
						   0, LONG_MAX, False, XA_WINDOW,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &rawData);

		if (actualType != XA_WINDOW || actualFormat != 32 || itemCount == 0 ||
			*(reinterpret_cast<Window*>(rawData)) != *proxy)
		{
			*proxy = xWindow;
		}
	}

	XFree(rawData);
	rawData = nullptr;

	// check XdndAware

	XGetWindowProperty(*itsDisplay, *proxy, itsAtoms[ kDNDAwareAtomIndex ],
					   0, LONG_MAX, False, XA_ATOM,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &rawData);

	if (actualType == XA_ATOM && actualFormat == 32 && itemCount > 0)
	{
		Atom* data = reinterpret_cast<Atom*>(rawData);
		if (data[0] >= kMinDNDVersion)
		{
			result = true;
			*vers  = JMin(kCurrentDNDVersion, data[0]);

			#if JXDND_DEBUG_MSGS
			std::cout << "Using XDND version " << *vers << std::endl;
			#endif
		}
	}

	XFree(rawData);
	return result;
}

/******************************************************************************
 AnnounceTypeList (private)

	Creates the XdndTypeList property on the specified window if there
	are more than kDNDEnterTypeCount data types.

 ******************************************************************************/

void
JXDNDManager::AnnounceTypeList
	(
	const Window		xWindow,
	const JArray<Atom>&	list
	)
	const
{
	const JSize typeCount = list.GetElementCount();
	if (typeCount > kDNDEnterTypeCount)
	{
		XChangeProperty(*itsDisplay, xWindow,
						itsAtoms[ kDNDTypeListAtomIndex ], XA_ATOM, 32,
						PropModeReplace,
						(unsigned char*) list.GetCArray(), typeCount);
	}
}

/******************************************************************************
 DraggerCanProvideText (private)

	Returns true if text/plain is in itsDraggerTypeList.

 ******************************************************************************/

bool
JXDNDManager::DraggerCanProvideText()
	const
{
	if (itsDragger == nullptr)
	{
		return false;
	}

	JXSelectionManager* selMgr = itsDisplay->GetSelectionManager();

	const Atom textAtom1 = selMgr->GetMimePlainTextXAtom(),
			   textAtom2 = selMgr->GetMimePlainTextUTF8XAtom();
	for (const auto type : *itsDraggerTypeList)
	{
		if (type == textAtom1 || type == textAtom2)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 AnnounceAskActions (private)

	Creates the XdndActionList and XdndActionDescription properties on
	itsDraggerWindow.

 ******************************************************************************/

void
JXDNDManager::AnnounceAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
	const
{
	assert( itsDragger != nullptr );

	itsDraggerAskActionList->RemoveAll();
	itsDraggerAskDescripList->CleanOut();

	itsDragger->GetDNDAskActions(buttonStates, modifiers,
								 itsDraggerAskActionList, itsDraggerAskDescripList);

	const JSize count = itsDraggerAskActionList->GetElementCount();
	assert( count >= 2 && count == itsDraggerAskDescripList->GetElementCount() );

	XChangeProperty(*itsDisplay, itsDraggerWindow,
					itsAtoms[ kDNDActionListAtomIndex ], XA_ATOM, 32,
					PropModeReplace,
					(unsigned char*) itsDraggerAskActionList->GetCArray(), count);

	const JString descripData = JXPackStrings(*itsDraggerAskDescripList);
	XChangeProperty(*itsDisplay, itsDraggerWindow,
					itsAtoms[ kDNDActionDescriptionAtomIndex ], XA_STRING, 8,
					PropModeReplace,
					(unsigned char*) descripData.GetRawBytes(),
					descripData.GetByteCount());
}

/******************************************************************************
 GetAskActions

	If successful, returns a list of the actions supported by the source.
	The caller must be prepared for this to fail by supporting a fallback
	operation.

	This function is not hidden inside ChooseDropAction() because the target
	may want to modify the list before presenting it to the user.

 ******************************************************************************/

bool
JXDNDManager::GetAskActions
	(
	JArray<Atom>*		actionList,
	JPtrArray<JString>*	descriptionList
	)
	const
{
	if (itsDragger != nullptr)
	{
		*actionList = *itsDraggerAskActionList;
		descriptionList->CopyObjects(*itsDraggerAskDescripList,
									 descriptionList->GetCleanUpAction(), false);
	}
	else
	{
		actionList->RemoveAll();
		descriptionList->CleanOut();

		Atom actualType;
		int actualFormat;
		unsigned long itemCount, remainingBytes;
		unsigned char* rawData = nullptr;

		// get action atoms

		XGetWindowProperty(*itsDisplay, itsDraggerWindow, itsAtoms[ kDNDActionListAtomIndex ],
						   0, LONG_MAX, False, XA_ATOM,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &rawData);

		if (actualType == XA_ATOM && actualFormat == 32 && itemCount > 0)
		{
			Atom* data = reinterpret_cast<Atom*>(rawData);
			for (JUnsignedOffset i=0; i<itemCount; i++)
			{
				actionList->AppendElement(data[i]);
			}
		}

		XFree(rawData);

		// get action descriptions

		XGetWindowProperty(*itsDisplay, itsDraggerWindow, itsAtoms[ kDNDActionDescriptionAtomIndex ],
						   0, LONG_MAX, False, XA_STRING,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &rawData);

		if (actualType == XA_STRING && actualFormat == 8 && itemCount > 0)
		{
			JXUnpackStrings(reinterpret_cast<JUtf8Byte*>(rawData), itemCount,
							descriptionList);
		}

		XFree(rawData);
	}

	// return status

	if (actionList->GetElementCount() != descriptionList->GetElementCount())
	{
		actionList->RemoveAll();
		descriptionList->CleanOut();
		return false;
	}
	else
	{
		return !actionList->IsEmpty();
	}
}

/******************************************************************************
 ChooseDropAction

	Asks the user which action to perform.  Returns false if cancelled.

	If the initial value of *action is one of the elements in actionList,
	the corresponding radio button becomes the initial choice.

 ******************************************************************************/

bool
JXDNDManager::ChooseDropAction
	(
	const JArray<Atom>&			actionList,
	const JPtrArray<JString>&	descriptionList,
	Atom*						action
	)
	const
{
	auto* dlog = jnew JXDNDChooseDropActionDialog(actionList, descriptionList, *action);

	*action = None;
	if (dlog->DoDialog())
	{
		*action = dlog->GetAction();
	}

	return *action != None;
}

/******************************************************************************
 SendDNDEnter (private)

 ******************************************************************************/

void
JXDNDManager::SendDNDEnter
	(
	const Window	xWindow,
	const Window	msgWindow,
	JXContainer*	widget,
	const bool	isAware,
	const Atom		vers
	)
{
	assert( xWindow != None );

	itsDNDVersion         = vers;
	itsMouseWindow        = xWindow;
	itsMouseWindowIsAware = isAware;
	itsMouseContainer     = widget;
	itsMsgWindow          = msgWindow;

	if (itsMouseContainer != nullptr)
	{
		ListenTo(itsMouseContainer);
	}

	itsWillAcceptDropFlag = false;
	itsWaitForStatusFlag  = false;
	itsSendHereMsgFlag    = false;
	itsReceivedStatusFlag = false;
	itsUseMouseRectFlag   = false;
	itsMouseRectR         = JRect(0,0,0,0);
	itsPrevStatusAction   = None;

	itsDragger->HandleDNDResponse(nullptr, false, None);		// reset cursor

	if (itsMouseContainer != nullptr)
	{
		itsPrevHereAction = None;
	}
	else if (itsMouseWindowIsAware)
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "Sent XdndEnter" << std::endl;
		#endif

		#if JXDND_SOURCE_DELAY > 0
		JWait(JXDND_SOURCE_DELAY);
		#endif

		const JSize typeCount = itsDraggerTypeList->GetElementCount();

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsMouseWindow;
		message.message_type = itsAtoms[ kDNDEnterAtomIndex ];
		message.format       = 32;

		message.data.l[ kDNDEnterWindow ] = itsDraggerWindow;

		message.data.l[ kDNDEnterFlags ]  = 0;
		message.data.l[ kDNDEnterFlags ] |= (itsDNDVersion << kDNDEnterVersionRShift);
		if (typeCount > kDNDEnterTypeCount)
		{
			message.data.l[ kDNDEnterFlags ] |= kDNDEnterMoreTypesFlag;
		}

		message.data.l[ kDNDEnterType1 ] = None;
		message.data.l[ kDNDEnterType2 ] = None;
		message.data.l[ kDNDEnterType3 ] = None;

		const JSize msgTypeCount = JMin(typeCount, (JSize) kDNDEnterTypeCount);
		for (JIndex i=1; i<=msgTypeCount; i++)
		{
			message.data.l[ kDNDEnterType1 + i-1 ] = itsDraggerTypeList->GetElement(i);
		}

		itsDisplay->SendXEvent(itsMsgWindow, &xEvent);
	}
}

/******************************************************************************
 SendDNDHere (private)

 ******************************************************************************/

void
JXDNDManager::SendDNDHere
	(
	const JPoint&			pt1,
	const Atom				action,
	const JXMouseButton		scrollButton,
	const JXKeyModifiers&	modifiers
	)
{
	const JPoint ptG1 = itsDragger->LocalToGlobal(pt1);
	const JPoint ptR  = (itsDragger->GetWindow())->GlobalToRoot(ptG1);

	const bool shouldSendMessage = itsMouseWindowIsAware &&
						(!itsUseMouseRectFlag || !itsMouseRectR.Contains(ptR) ||
						 scrollButton != 0);

	if (itsMouseContainer != nullptr)
	{
		// Scroll first, because it affects drop location.
		// Always call it, in case mouse is inside a scroll zone.

		const JPoint ptG2 = (itsMouseContainer->GetWindow())->RootToGlobal(ptR);
		const JPoint pt2  = itsMouseContainer->GlobalToLocal(ptG2);
		itsMouseContainer->DNDScroll(pt2, scrollButton, modifiers);

		const bool savedAccept = itsWillAcceptDropFlag;
		Atom acceptedAction        = action;
		itsWillAcceptDropFlag =
			itsMouseContainer->WillAcceptDrop(*itsDraggerTypeList, &acceptedAction,
											  pt2, CurrentTime, itsDragger);
		if (itsWillAcceptDropFlag)
		{
			itsMouseContainer->DNDEnter();
		}
		else
		{
			itsMouseContainer->DNDLeave();
			acceptedAction = None;
		}

		if (itsWillAcceptDropFlag != savedAccept ||
			itsPrevStatusAction != acceptedAction)
		{
			itsDragger->HandleDNDResponse(itsMouseContainer, itsWillAcceptDropFlag,
										  acceptedAction);
		}

		itsReceivedStatusFlag = true;
		itsPrevHereAction     = action;
		itsPrevStatusAction   = acceptedAction;

		if (itsWillAcceptDropFlag)
		{
			itsPrevMousePt = pt2;
			itsMouseContainer->DNDHere(itsPrevMousePt, itsDragger);
		}
	}

	else if (!itsWaitForStatusFlag && shouldSendMessage)
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "Sent XdndPosition: " << ptR.x << ' ' << ptR.y << std::endl;
		#endif

		#if JXDND_SOURCE_DELAY > 0
		JWait(JXDND_SOURCE_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsMouseWindow;
		message.message_type = itsAtoms[ kDNDHereAtomIndex ];
		message.format       = 32;

		message.data.l[ kDNDHereWindow    ] = itsDraggerWindow;
		message.data.l[ kDNDHereFlags     ] = 0;
		message.data.l[ kDNDHerePt        ] = PackPoint(ptR);
		message.data.l[ kDNDHereTimeStamp ] = itsDisplay->GetLastEventTime();
		message.data.l[ kDNDHereAction    ] = action;

		if (scrollButton != 0)
		{
			message.data.l[ kDNDHereFlags ] |= kDNDScrollTargetMask;
			message.data.l[ kDNDHereFlags ] |= ((scrollButton - 4) << kDNDScrollButtonShift) & kDNDScrollButtonMask;
			message.data.l[ kDNDHereFlags ] |= (modifiers.GetState() & kDNDScrollModsMask);
		}

		itsDisplay->SendXEvent(itsMsgWindow, &xEvent);

		itsWaitForStatusFlag = true;
		itsSendHereMsgFlag   = false;
	}

	else if (itsWaitForStatusFlag && shouldSendMessage)
	{
		itsSendHereMsgFlag = true;
	}
}

/******************************************************************************
 SendDNDLeave (private)

 ******************************************************************************/

void
JXDNDManager::SendDNDLeave
	(
	const bool sendPasteClick
	)
{
	if (itsMouseContainer != nullptr)
	{
		if (itsWillAcceptDropFlag)
		{
			itsMouseContainer->DNDLeave();
		}
	}
	else if (itsMouseWindowIsAware)
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "Sent XdndLeave" << std::endl;
		#endif

		#if JXDND_SOURCE_DELAY > 0
		JWait(JXDND_SOURCE_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsMouseWindow;
		message.message_type = itsAtoms[ kDNDLeaveAtomIndex ];
		message.format       = 32;

		message.data.l[ kDNDLeaveWindow ] = itsDraggerWindow;
		message.data.l[ kDNDLeaveFlags  ] = 0;

		itsDisplay->SendXEvent(itsMsgWindow, &xEvent);
	}
	else if (sendPasteClick && DraggerCanProvideText())
	{
		JXContainer* dropWidget;
		Window xWindow;
		JPoint ptG, ptR;
		itsDisplay->FindMouseContainer(itsDragger, itsPrevHandleDNDPt,
									   &dropWidget, &xWindow, &ptG, &ptR);

		const Window rootWindow = itsDisplay->GetRootWindow();
		if (dropWidget == nullptr && xWindow != None && xWindow != rootWindow)
		{
			// this isn't necessary -- our clipboard continues to work as usual
			// (itsDisplay->GetSelectionManager())->ClearData(kJXClipboardName);

			const Window xferWindow  = (itsDisplay->GetSelectionManager())->GetDataTransferWindow();
			const Time lastEventTime = itsDisplay->GetLastEventTime();
			XSetSelectionOwner(*itsDisplay, kJXClipboardName, xferWindow, CurrentTime);
			if (XGetSelectionOwner(*itsDisplay, kJXClipboardName) == xferWindow)
			{
				PrepareForDrop(nullptr);

				itsSentFakePasteFlag     = true;
				itsFakeButtonPressTime   = lastEventTime+1;
				itsFakeButtonReleaseTime = lastEventTime+2;

				XEvent xEvent;
				xEvent.xbutton.type        = ButtonPress;
				xEvent.xbutton.display     = *itsDisplay;
				xEvent.xbutton.window      = xWindow;
				xEvent.xbutton.root        = rootWindow;
				xEvent.xbutton.subwindow   = None;
				xEvent.xbutton.time        = itsFakeButtonPressTime;
				xEvent.xbutton.x           = ptG.x;
				xEvent.xbutton.y           = ptG.y;
				xEvent.xbutton.x_root      = ptR.x;
				xEvent.xbutton.y_root      = ptR.y;
				xEvent.xbutton.state       = 0;
				xEvent.xbutton.button      = Button2;
				xEvent.xbutton.same_screen = True;

				itsDisplay->SendXEvent(xWindow, &xEvent);

				xEvent.xbutton.type  = ButtonRelease;
				xEvent.xbutton.time  = itsFakeButtonReleaseTime;
				xEvent.xbutton.state = Button2Mask;

				itsDisplay->SendXEvent(xWindow, &xEvent);

				#if JXDND_DEBUG_MSGS
				std::cout << "Sent fake middle click, time=" << xEvent.xbutton.time << std::endl;
				#endif
			}
		}
	}
}

/******************************************************************************
 SendDNDDrop (private)

 ******************************************************************************/

void
JXDNDManager::SendDNDDrop()
{
	assert( itsWillAcceptDropFlag );

	PrepareForDrop(itsMouseContainer);

	if (itsMouseContainer != nullptr)
	{
		itsMouseContainer->DNDDrop(itsPrevMousePt, *itsDraggerTypeList,
								   itsPrevStatusAction, CurrentTime, itsDragger);
	}
	else if (itsMouseWindowIsAware)
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "Sent XdndDrop" << std::endl;
		#endif

		#if JXDND_SOURCE_DELAY > 0
		JWait(JXDND_SOURCE_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsMouseWindow;
		message.message_type = itsAtoms[ kDNDDropAtomIndex ];
		message.format       = 32;

		message.data.l[ kDNDDropWindow    ] = itsDraggerWindow;
		message.data.l[ kDNDDropFlags     ] = 0;
		message.data.l[ kDNDDropTimeStamp ] = itsDisplay->GetLastEventTime();

		itsDisplay->SendXEvent(itsMsgWindow, &xEvent);
	}
}

/******************************************************************************
 PrepareForDrop (private)

 ******************************************************************************/

void
JXDNDManager::PrepareForDrop
	(
	const JXContainer* target
	)
{
	const JXSelectionData* data = nullptr;
	if ((itsDisplay->GetSelectionManager())->
			GetData(itsAtoms[ kDNDSelectionAtomIndex ], CurrentTime, &data))
	{
		data->Resolve();
	}

	itsDragger->DNDFinish(true, target);
}

/******************************************************************************
 SendDNDStatus (private)

	This is sent to let the source know that we are ready for another
	DNDHere message.

 ******************************************************************************/

void
JXDNDManager::SendDNDStatus
	(
	const bool	willAcceptDrop,
	const Atom		action
	)
{
	if (itsDraggerWindow != None)
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "Sent XdndStatus: " << willAcceptDrop << std::endl;
		#endif

		#if JXDND_TARGET_DELAY > 0
		JWait(JXDND_TARGET_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsDraggerWindow;
		message.message_type = itsAtoms[ kDNDStatusAtomIndex ];
		message.format       = 32;

		message.data.l[ kDNDStatusWindow ] = itsMouseWindow;
		message.data.l[ kDNDStatusFlags  ] = 0;
		message.data.l[ kDNDStatusPt     ] = 0;		// empty rectangle
		message.data.l[ kDNDStatusArea   ] = 0;
		message.data.l[ kDNDStatusAction ] = action;

		if (willAcceptDrop)
		{
			message.data.l[ kDNDStatusFlags ] |= kDNDStatusAcceptDropFlag;
		}

		itsDisplay->SendXEvent(itsDraggerWindow, &xEvent);
	}
}

/******************************************************************************
 SendDNDFinished (private)

	This is sent to let the source know that we are done with the data.

 ******************************************************************************/

void
JXDNDManager::SendDNDFinished()
{
	if (itsDraggerWindow != None)
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "Sent XdndFinished" << std::endl;
		#endif

		#if JXDND_TARGET_DELAY > 0
		JWait(JXDND_TARGET_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsDraggerWindow;
		message.message_type = itsAtoms[ kDNDFinishedAtomIndex ];
		message.format       = 32;

		message.data.l[ kDNDFinishedWindow ] = itsMouseWindow;
		message.data.l[ kDNDFinishedFlags ]  = 0;

		itsDisplay->SendXEvent(itsDraggerWindow, &xEvent);
	}
}

/******************************************************************************
 HandleClientMessage 

 ******************************************************************************/

bool
JXDNDManager::HandleClientMessage
	(
	const XClientMessageEvent& clientMessage
	)
{
	// target:  receive and process window-level enter and leave

	if (clientMessage.message_type == itsAtoms[ kDNDEnterAtomIndex ])
	{
		HandleDNDEnter(clientMessage);
		return true;
	}

	else if (clientMessage.message_type == itsAtoms[ kDNDHereAtomIndex ])
	{
		HandleDNDHere(clientMessage);
		return true;
	}

	else if (clientMessage.message_type == itsAtoms[ kDNDLeaveAtomIndex ])
	{
		HandleDNDLeave(clientMessage);
		return true;
	}

	else if (clientMessage.message_type == itsAtoms[ kDNDDropAtomIndex ])
	{
		HandleDNDDrop(clientMessage);
		return true;
	}

	// source:  clear our flag when we receive status message

	else if (clientMessage.message_type == itsAtoms[ kDNDStatusAtomIndex ])
	{
		HandleDNDStatus(clientMessage);
		return true;
	}

	// source:  ignore finished message
	//			because JXSelectionManager rejects outdated requests

	else if (clientMessage.message_type == itsAtoms[ kDNDFinishedAtomIndex ])
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "Received XdndFinished" << std::endl;
		#endif

		return true;
	}

	// not for us

	else
	{
		return false;
	}
}

/******************************************************************************
 HandleDNDEnter (private)

 ******************************************************************************/

void
JXDNDManager::HandleDNDEnter
	(
	const XClientMessageEvent& clientMessage
	)
{
	assert( clientMessage.message_type == itsAtoms[ kDNDEnterAtomIndex ] );

	itsDNDVersion =
		(clientMessage.data.l[ kDNDEnterFlags ] >> kDNDEnterVersionRShift) &
		kDNDEnterVersionMask;

	if (itsDraggerWindow == None &&
		kMinDNDVersion <= itsDNDVersion && itsDNDVersion <= kCurrentDNDVersion &&
		clientMessage.data.l[ kDNDEnterType1 ] != None)
	{
		#if JXDND_TARGET_DELAY > 0
			#if JXDND_DEBUG_MSGS
			std::cout << "XdndEnter will arrive" << std::endl;
			#endif
		JWait(JXDND_TARGET_DELAY);
		#endif

		#if JXDND_DEBUG_MSGS
		std::cout << "Received XdndEnter" << std::endl;
		#endif

		if (itsDragger != nullptr)
		{
			StopListening(itsDragger);
		}
		if (itsMouseContainer != nullptr)
		{
			StopListening(itsMouseContainer);
		}

		itsIsDraggingFlag = false;
		itsDragger        = nullptr;
		itsDraggerWindow  = clientMessage.data.l[ kDNDEnterWindow ];
		itsMouseWindow    = clientMessage.window;
		itsMouseContainer = nullptr;
		itsPrevHereAction = None;

		XSelectInput(*itsDisplay, itsDraggerWindow, StructureNotifyMask);
		ListenTo(itsDisplay);

		itsDraggerTypeList->RemoveAll();
		if ((clientMessage.data.l[ kDNDEnterFlags ] & kDNDEnterMoreTypesFlag) != 0)
		{
			Atom actualType;
			int actualFormat;
			unsigned long itemCount, remainingBytes;
			unsigned char* rawData = nullptr;
			XGetWindowProperty(*itsDisplay, itsDraggerWindow, itsAtoms[ kDNDTypeListAtomIndex ],
							   0, LONG_MAX, False, XA_ATOM,
							   &actualType, &actualFormat,
							   &itemCount, &remainingBytes, &rawData);

			if (actualType == XA_ATOM && actualFormat == 32 && itemCount > 0)
			{
				Atom* data = reinterpret_cast<Atom*>(rawData);
				for (JUnsignedOffset i=0; i<itemCount; i++)
				{
					itsDraggerTypeList->AppendElement(data[i]);
				}
			}

			XFree(rawData);
		}

		if (itsDraggerTypeList->IsEmpty())		// in case window property was empty
		{
			for (JIndex i=1; i<=kDNDEnterTypeCount; i++)
			{
				const Atom type = clientMessage.data.l[ kDNDEnterType1 + i-1 ];
				if (type == None)
				{
					assert( i > 1 );
					break;
				}
				itsDraggerTypeList->AppendElement(type);
			}
		}
	}
}

/******************************************************************************
 HandleDNDHere (private)

 ******************************************************************************/

void
JXDNDManager::HandleDNDHere
	(
	const XClientMessageEvent& clientMessage
	)
{
	assert( clientMessage.message_type == itsAtoms[ kDNDHereAtomIndex ] );

	if (itsDraggerWindow != (Window) clientMessage.data.l[ kDNDHereWindow ])
	{
		return;
	}

	#if JXDND_TARGET_DELAY > 0
		#if JXDND_DEBUG_MSGS
		std::cout << "XdndPosition will arrive" << std::endl;
		#endif
	JWait(JXDND_TARGET_DELAY);
	#endif

	#if JXDND_DEBUG_MSGS
	std::cout << "Received XdndPosition" << std::endl;
	#endif

	const JPoint ptR = UnpackPoint(clientMessage.data.l[ kDNDHerePt ]);
	JPoint ptG, pt;

	const Time time       = clientMessage.data.l[ kDNDHereTimeStamp ];
	Atom action           = clientMessage.data.l[ kDNDHereAction ];
	const Atom origAction = action;

	JXWindow* window;
	if (itsMouseContainer != nullptr)
	{
		window = itsMouseContainer->GetWindow();
		ptG    = window->RootToGlobal(ptR);
		JXContainer* newMouseContainer;
		const bool found = itsDisplay->FindMouseContainer(window, ptG, &newMouseContainer);
		if (found && newMouseContainer != itsMouseContainer)
		{
			if (itsWillAcceptDropFlag)
			{
				itsMouseContainer->DNDLeave();
			}
			StopListening(itsMouseContainer);
			itsMouseContainer = newMouseContainer;
			ListenTo(itsMouseContainer);
			pt = itsMouseContainer->GlobalToLocal(ptG);
			InvokeDNDScroll(clientMessage, pt);
			itsWillAcceptDropFlag =
				itsMouseContainer->WillAcceptDrop(*itsDraggerTypeList, &action, pt, time, nullptr);
			if (itsWillAcceptDropFlag)
			{
				itsMouseContainer->DNDEnter();
			}
		}
		else if (found)
		{
			const bool savedAccept = itsWillAcceptDropFlag;
			pt = itsMouseContainer->GlobalToLocal(ptG);
			InvokeDNDScroll(clientMessage, pt);
			itsWillAcceptDropFlag =
				itsMouseContainer->WillAcceptDrop(*itsDraggerTypeList, &action, pt, time, nullptr);
			if ((action != itsPrevHereAction || !savedAccept) && itsWillAcceptDropFlag)
			{
				itsMouseContainer->DNDEnter();
			}
			else if ((action != itsPrevHereAction || savedAccept) && !itsWillAcceptDropFlag)
			{
				itsMouseContainer->DNDLeave();
			}
		}
	}
	else if (itsDisplay->FindXWindow(itsMouseWindow, &window))
	{
		ptG = window->RootToGlobal(ptR);
		if (itsDisplay->FindMouseContainer(window, ptG, &itsMouseContainer))
		{
			ListenTo(itsMouseContainer);
			pt = itsMouseContainer->GlobalToLocal(ptG);
			itsWillAcceptDropFlag =
				itsMouseContainer->WillAcceptDrop(*itsDraggerTypeList, &action, pt, time, nullptr);
			if (itsWillAcceptDropFlag)
			{
				itsMouseContainer->DNDEnter();
			}
		}
	}

	itsPrevHereAction = origAction;

	if (itsMouseContainer != nullptr && itsWillAcceptDropFlag)
	{
		itsPrevMousePt      = pt;
		itsPrevStatusAction = action;
		itsMouseContainer->DNDHere(itsPrevMousePt, nullptr);
		SendDNDStatus(true, action);
	}
	else
	{
		SendDNDStatus(false, None);
	}
}

void
JXDNDManager::InvokeDNDScroll
	(
	const XClientMessageEvent&	clientMessage,
	const JPoint&				pt
	)
{
	const auto scrollButton = (JXMouseButton)
		(((clientMessage.data.l[ kDNDHereFlags ] & kDNDScrollButtonMask) >> kDNDScrollButtonShift) + 4);
	const JXKeyModifiers modifiers(itsDisplay, clientMessage.data.l[ kDNDHereFlags ] & kDNDScrollModsMask);

	itsMouseContainer->DNDScroll(
		pt,
		(clientMessage.data.l[ kDNDHereFlags ] & kDNDScrollTargetMask) ? scrollButton : (JXMouseButton) 0,
		modifiers);
}

/******************************************************************************
 HandleDNDLeave (private)

 ******************************************************************************/

void
JXDNDManager::HandleDNDLeave
	(
	const XClientMessageEvent& clientMessage
	)
{
	assert( clientMessage.message_type == itsAtoms[ kDNDLeaveAtomIndex ] );

	if (itsDraggerWindow == (Window) clientMessage.data.l[ kDNDLeaveWindow ])
	{
		#if JXDND_TARGET_DELAY > 0
			#if JXDND_DEBUG_MSGS
			std::cout << "XdndLeave will arrive" << std::endl;
			#endif
		JWait(JXDND_TARGET_DELAY);
		#endif

		#if JXDND_DEBUG_MSGS
		std::cout << "Received XdndLeave" << std::endl;
		#endif

		CleanUpAfterDNDLeave();
	}
}

/******************************************************************************
 CleanUpAfterDNDLeave (private)

 ******************************************************************************/

void
JXDNDManager::CleanUpAfterDNDLeave()
{
	if (itsMouseContainer != nullptr && itsWillAcceptDropFlag)
	{
		itsMouseContainer->DNDLeave();
	}

	HandleDNDFinished();
}

/******************************************************************************
 HandleDNDDrop (private)

 ******************************************************************************/

void
JXDNDManager::HandleDNDDrop
	(
	const XClientMessageEvent& clientMessage
	)
{
	assert( clientMessage.message_type == itsAtoms[ kDNDDropAtomIndex ] );

	if (itsDraggerWindow == (Window) clientMessage.data.l[ kDNDDropWindow ])
	{
		#if JXDND_TARGET_DELAY > 0
			#if JXDND_DEBUG_MSGS
			std::cout << "XdndDrop will arrive" << std::endl;
			#endif
		JWait(JXDND_TARGET_DELAY);
		#endif

		#if JXDND_DEBUG_MSGS
		std::cout << "Received XdndDrop" << std::endl;
		#endif

		const Time time = clientMessage.data.l[ kDNDDropTimeStamp ];

		if (itsMouseContainer != nullptr && itsWillAcceptDropFlag)
		{
			itsMouseContainer->DNDDrop(itsPrevMousePt, *itsDraggerTypeList,
									   itsPrevStatusAction, time, nullptr);
		}

		SendDNDFinished();		// do it here because not when HandleDNDLeave()
		HandleDNDFinished();
	}
}

/******************************************************************************
 HandleDNDFinished (private)

 ******************************************************************************/

void
JXDNDManager::HandleDNDFinished()
{
	if (itsDraggerWindow != None)
	{
		XSelectInput(*itsDisplay, itsDraggerWindow, NoEventMask);
	}
	StopListening(itsDisplay);

	if (itsDragger != nullptr)
	{
		StopListening(itsDragger);
	}
	if (itsMouseContainer != nullptr)
	{
		StopListening(itsMouseContainer);
	}

	itsIsDraggingFlag = false;
	itsDragger        = nullptr;
	itsDraggerWindow  = None;
	itsMouseWindow    = None;
	itsMouseContainer = nullptr;
}

/******************************************************************************
 HandleDNDStatus (private)

 ******************************************************************************/

void
JXDNDManager::HandleDNDStatus
	(
	const XClientMessageEvent& clientMessage
	)
{
	assert( clientMessage.message_type == itsAtoms[ kDNDStatusAtomIndex ] );

	if (itsDragger != nullptr &&
		itsMouseWindow == (Window) clientMessage.data.l[ kDNDStatusWindow ])
	{
		#if JXDND_SOURCE_DELAY > 0
			#if JXDND_DEBUG_MSGS
			std::cout << "XdndStatus will arrive" << std::endl;
			#endif
		JWait(JXDND_SOURCE_DELAY);
		#endif

		const bool savedAccept = itsWillAcceptDropFlag;

		itsWaitForStatusFlag  = false;
		itsReceivedStatusFlag = true;

		itsWillAcceptDropFlag = clientMessage.data.l[ kDNDStatusFlags ] & kDNDStatusAcceptDropFlag;

		itsUseMouseRectFlag =
			!( clientMessage.data.l[ kDNDStatusFlags ] & kDNDStatusSendHereFlag );

		itsMouseRectR = UnpackRect(clientMessage.data.l[ kDNDStatusPt ],
								   clientMessage.data.l[ kDNDStatusArea ]);

		#if JXDND_DEBUG_MSGS
		std::cout << "Received XdndStatus: " << itsWillAcceptDropFlag;
		std::cout << ' ' << itsUseMouseRectFlag;
		std::cout << ' ' << itsMouseRectR.top << ' ' << itsMouseRectR.left;
		std::cout << ' ' << itsMouseRectR.bottom << ' ' << itsMouseRectR.right << std::endl;
		#endif

		Atom action = clientMessage.data.l[ kDNDStatusAction ];
		if (!itsWillAcceptDropFlag)
		{
			action = None;
		}

		if (itsWillAcceptDropFlag != savedAccept ||
			itsPrevStatusAction != action)
		{
			itsDragger->HandleDNDResponse(nullptr, itsWillAcceptDropFlag, action);
			itsPrevStatusAction = action;
		}

		if (itsSendHereMsgFlag)
		{
			SendDNDHere(itsPrevHandleDNDPt, itsPrevHandleDNDAction,
						itsPrevHandleDNDScrollButton, itsPrevHandleDNDModifiers);
		}
	}
}

/******************************************************************************
 WaitForLastStatusMsg (private)

	Returns true if it receives XdndStatus message with accept==true.

 ******************************************************************************/

bool
JXDNDManager::WaitForLastStatusMsg()
{
	if (!itsReceivedStatusFlag)
	{
		return false;
	}
	else if (!itsWaitForStatusFlag)
	{
		return itsWillAcceptDropFlag;
	}

	itsSendHereMsgFlag = false;	// don't send any more messages

	JXSelectionManager* selMgr = itsDisplay->GetSelectionManager();

	Atom messageList[] = { 1, itsAtoms[ kDNDStatusAtomIndex ] };

	XEvent xEvent;
	clock_t endTime = clock() + kWaitForLastStatusTime;
	while (clock() < endTime)
	{
		if (XCheckIfEvent(*itsDisplay, &xEvent, GetNextStatusEvent,
						  reinterpret_cast<char*>(messageList)))
		{
			if (xEvent.type == ClientMessage)
			{
				const bool ok = HandleClientMessage(xEvent.xclient);
				assert( ok );
				if (xEvent.xclient.message_type == itsAtoms[ kDNDStatusAtomIndex ])
				{
					return itsWillAcceptDropFlag;
				}
			}
			else if (xEvent.type == SelectionRequest)
			{
				selMgr->HandleSelectionRequest(xEvent.xselectionrequest);
			}

			// reset timeout

			endTime = clock() + kWaitForLastStatusTime;
		}
	}

	return false;
}

// static

Bool
JXDNDManager::GetNextStatusEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	Atom* messageList     = reinterpret_cast<Atom*>(arg);
	const JSize atomCount = messageList[0];

	if (event->type == ClientMessage)
	{
		for (JIndex i=1; i<=atomCount; i++)
		{
			if ((event->xclient).message_type == messageList[i])
			{
				return True;
			}
		}
	}
	else if (event->type == SelectionRequest)
	{
		return True;
	}

	return False;
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
	if (sender == itsDisplay && message->Is(JXDisplay::kXError))
	{
		auto* err = dynamic_cast<JXDisplay::XError*>(message);
		assert( err != nullptr );

		// source: target crashed -- nothing to do

		// target: source crashed

		if (itsDragger == nullptr && itsDraggerWindow != None &&
			err->GetType() == BadWindow &&
			err->GetXID()  == itsDraggerWindow)
		{
			#if JXDND_DEBUG_MSGS
			std::cout << "JXDND: Source crashed via XError" << std::endl;
			#endif

			itsDraggerWindow = None;
			CleanUpAfterDNDLeave();
			err->SetCaught();
		}

		// target: residual from source crash -- nothing to do
	}

	else
	{
		JBroadcaster::ReceiveWithFeedback(sender, message);
	}
}

/******************************************************************************
 HandleDestroyNotify

	This catches source crashes while the source is processing XdndStatus.

 ******************************************************************************/

bool
JXDNDManager::HandleDestroyNotify
	(
	const XDestroyWindowEvent& xEvent
	)
{
	if (itsDragger == nullptr && itsDraggerWindow != None &&
		xEvent.window == itsDraggerWindow)
	{
		#if JXDND_DEBUG_MSGS
		std::cout << "JXDND: Source crashed via DestroyNotify" << std::endl;
		#endif

		itsDraggerWindow = None;
		CleanUpAfterDNDLeave();
		return true;
	}
	else
	{
		return false;
	}
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
	if (sender == itsDragger)
	{
		itsDragger = nullptr;
		CancelDND();
	}
	else if (sender == itsMouseContainer)
	{
		itsMouseContainer = nullptr;
	}
	else
	{
		JBroadcaster::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 Packing/unpacking (private)

 ******************************************************************************/

inline long
JXDNDManager::PackPoint
	(
	const JPoint& pt
	)
	const
{
	return ((pt.x << 16) | pt.y);
}

inline JPoint
JXDNDManager::UnpackPoint
	(
	const long data
	)
	const
{
	return JPoint((data >> 16) & 0xFFFF, data & 0xFFFF);
}

inline JRect
JXDNDManager::UnpackRect
	(
	const long pt,
	const long area
	)
	const
{
	const JPoint topLeft = UnpackPoint(pt);
	return JRect(topLeft.y, topLeft.x,
				 topLeft.y + (area & 0xFFFF),
				 topLeft.x + ((area >> 16) & 0xFFFF));
}

/******************************************************************************
 Default cursors

 ******************************************************************************/

static const JUtf8Byte* kDefaultDNDCursorName[] =
{
	"JXDNDCopyCursor",
	"JXDNDMoveCursor",
	"JXDNDLinkCursor",
	"JXDNDAskCursor",

	"JXDNDCopyFileCursor",
	"JXDNDMoveFileCursor",
	"JXDNDLinkFileCursor",
	"JXDNDAskFileCursor",

	"JXDNDCopyDirCursor",
	"JXDNDMoveDirCursor",
	"JXDNDLinkDirCursor",
	"JXDNDAskDirCursor",

	"JXDNDCopyMixedCursor",
	"JXDNDMoveMixedCursor",
	"JXDNDLinkMixedCursor",
	"JXDNDAskMixedCursor"
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

// generic

#include <jx-af/image/jx/jx_drag_object_cursor.xbm>
#include <jx-af/image/jx/jx_drag_object_cursor_mask.xbm>
#include <jx-af/image/jx/jx_drag_copy_object_cursor.xbm>
#include <jx-af/image/jx/jx_drag_link_object_cursor.xbm>
#include <jx-af/image/jx/jx_drag_ask_object_cursor.xbm>
#include <jx-af/image/jx/jx_drag_mod_object_cursor_mask.xbm>

// file

#include <jx-af/image/jx/jx_drag_file_cursor.xbm>
#include <jx-af/image/jx/jx_drag_file_cursor_mask.xbm>
#include <jx-af/image/jx/jx_drag_copy_file_cursor.xbm>
#include <jx-af/image/jx/jx_drag_link_file_cursor.xbm>
#include <jx-af/image/jx/jx_drag_ask_file_cursor.xbm>
#include <jx-af/image/jx/jx_drag_mod_file_cursor_mask.xbm>

// directory

#include <jx-af/image/jx/jx_drag_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_directory_cursor_mask.xbm>
#include <jx-af/image/jx/jx_drag_copy_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_link_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_ask_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_mod_directory_cursor_mask.xbm>

// mixed

#include <jx-af/image/jx/jx_drag_file_and_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_file_and_directory_cursor_mask.xbm>
#include <jx-af/image/jx/jx_drag_copy_file_and_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_link_file_and_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_ask_file_and_directory_cursor.xbm>
#include <jx-af/image/jx/jx_drag_mod_file_and_directory_cursor_mask.xbm>

#pragma GCC diagnostic pop

static const JXCursor kDefaultDNDCursor[] =
{
{ jx_drag_copy_object_cursor_width, jx_drag_copy_object_cursor_height, 10,10,
	  jx_drag_copy_object_cursor_bits, jx_drag_mod_object_cursor_mask_bits },
{ jx_drag_object_cursor_width, jx_drag_object_cursor_height, 10,10,
	  jx_drag_object_cursor_bits, jx_drag_object_cursor_mask_bits },
{ jx_drag_link_object_cursor_width, jx_drag_link_object_cursor_height, 10,10,
	  jx_drag_link_object_cursor_bits, jx_drag_mod_object_cursor_mask_bits },
{ jx_drag_ask_object_cursor_width, jx_drag_ask_object_cursor_height, 10,10,
	  jx_drag_ask_object_cursor_bits, jx_drag_mod_object_cursor_mask_bits },

{ jx_drag_copy_file_cursor_width, jx_drag_copy_file_cursor_height, 6,7,
	  jx_drag_copy_file_cursor_bits, jx_drag_mod_file_cursor_mask_bits },
{ jx_drag_file_cursor_width, jx_drag_file_cursor_height, 6,7,
	  jx_drag_file_cursor_bits, jx_drag_file_cursor_mask_bits },
{ jx_drag_link_file_cursor_width, jx_drag_link_file_cursor_height, 6,7,
	  jx_drag_link_file_cursor_bits, jx_drag_mod_file_cursor_mask_bits },
{ jx_drag_ask_file_cursor_width, jx_drag_ask_file_cursor_height, 6,7,
	  jx_drag_ask_file_cursor_bits, jx_drag_mod_file_cursor_mask_bits },

{ jx_drag_copy_directory_cursor_width, jx_drag_copy_directory_cursor_height, 6,7,
	  jx_drag_copy_directory_cursor_bits, jx_drag_mod_directory_cursor_mask_bits },
{ jx_drag_directory_cursor_width, jx_drag_directory_cursor_height, 6,7,
	  jx_drag_directory_cursor_bits, jx_drag_directory_cursor_mask_bits },
{ jx_drag_link_directory_cursor_width, jx_drag_link_directory_cursor_height, 6,7,
	  jx_drag_link_directory_cursor_bits, jx_drag_mod_directory_cursor_mask_bits },
{ jx_drag_ask_directory_cursor_width, jx_drag_ask_directory_cursor_height, 6,7,
	  jx_drag_ask_directory_cursor_bits, jx_drag_mod_directory_cursor_mask_bits },

{ jx_drag_copy_file_and_directory_cursor_width, jx_drag_copy_file_and_directory_cursor_height, 8,12,
	  jx_drag_copy_file_and_directory_cursor_bits, jx_drag_mod_file_and_directory_cursor_mask_bits },
{ jx_drag_file_and_directory_cursor_width, jx_drag_file_and_directory_cursor_height, 8,12,
	  jx_drag_file_and_directory_cursor_bits, jx_drag_file_and_directory_cursor_mask_bits },
{ jx_drag_link_file_and_directory_cursor_width, jx_drag_link_file_and_directory_cursor_height, 8,12,
	  jx_drag_link_file_and_directory_cursor_bits, jx_drag_mod_file_and_directory_cursor_mask_bits },
{ jx_drag_ask_file_and_directory_cursor_width, jx_drag_ask_file_and_directory_cursor_height, 8,12,
	  jx_drag_ask_file_and_directory_cursor_bits,  jx_drag_mod_file_and_directory_cursor_mask_bits }
};	

/******************************************************************************
 InitCursors (private)

 ******************************************************************************/

void
JXDNDManager::InitCursors()
{
	assert( sizeof(kDefaultDNDCursorName)/sizeof(JUtf8Byte*) == kDefDNDCursorCount );

	for (JUnsignedOffset i=0; i<kDefDNDCursorCount; i++)
	{
		itsDefDNDCursor[i] =
			itsDisplay->CreateCustomCursor(kDefaultDNDCursorName[i], kDefaultDNDCursor[i]);
	}
}

/******************************************************************************
 GetDNDCursor

	Returns the appropriate cursor for the given response.

	cursor must be an array of 4 cursors:  copy, move, link, ask

 ******************************************************************************/

JCursorIndex
JXDNDManager::GetDNDCursor
	(
	const bool		dropAccepted,
	const Atom			action,
	const JCursorIndex*	cursor
	)
	const
{
	if (!dropAccepted || action == GetDNDActionMoveXAtom())
	{
		return cursor [ kMoveCursorIndex ];
	}
	else if (action == GetDNDActionCopyXAtom())
	{
		return cursor [ kCopyCursorIndex ];
	}
	else if (action == GetDNDActionLinkXAtom())
	{
		return cursor [ kLinkCursorIndex ];
	}
	else if (action == GetDNDActionAskXAtom())
	{
		return cursor [ kAskCursorIndex ];
	}
	else
	{
		return cursor [ kMoveCursorIndex ];
	}
}

/******************************************************************************
 JXDNDManager::TargetFinder

	This defines the interface for all objects that can locate a DND target.

	Derived classes must override FindTarget().  The return values are:

		return bool		target supports XDND
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
