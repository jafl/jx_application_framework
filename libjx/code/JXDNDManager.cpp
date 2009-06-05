/******************************************************************************
 JXDNDManager.cpp

	Global object to manage Drag-And-Drop interactions.

	When the drop is intra-application, we simply call the Widget directly.
	When the drop is inter-application, we send ClientMessages.

	Refer to http://www.newplanetsoftware.com/xdnd/ for the complete protocol.

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

	Copyright © 1997-2009 by John Lindal. All rights reserved.

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

const Atom kCurrentDNDVersion = 4;
const Atom kMinDNDVersion     = 3;

const JCoordinate kHysteresisBorderWidth = 50;
const clock_t kWaitForLastStatusTime     = 10 * CLOCKS_PER_SEC;

// atom names

static const JCharacter* kDNDSelectionXAtomName = "XdndSelection";

static const JCharacter* kDNDProxyXAtomName    = "XdndProxy";
static const JCharacter* kDNDAwareXAtomName    = "XdndAware";
static const JCharacter* kDNDTypeListXAtomName = "XdndTypeList";

static const JCharacter* kDNDEnterXAtomName    = "XdndEnter";
static const JCharacter* kDNDHereXAtomName     = "XdndPosition";
static const JCharacter* kDNDStatusXAtomName   = "XdndStatus";
static const JCharacter* kDNDLeaveXAtomName    = "XdndLeave";
static const JCharacter* kDNDDropXAtomName     = "XdndDrop";
static const JCharacter* kDNDFinishedXAtomName = "XdndFinished";

static const JCharacter* kDNDActionCopyXAtomName       = "XdndActionCopy";
static const JCharacter* kDNDActionMoveXAtomName       = "XdndActionMove";
static const JCharacter* kDNDActionLinkXAtomName       = "XdndActionLink";
static const JCharacter* kDNDActionAskXAtomName        = "XdndActionAsk";
static const JCharacter* kDNDActionPrivateXAtomName    = "XdndActionPrivate";
static const JCharacter* kDNDActionDirectSaveXAtomName = "XdndActionDirectSave";

static const JCharacter* kDNDActionListXAtomName        = "XdndActionList";
static const JCharacter* kDNDActionDescriptionXAtomName = "XdndActionDescription";

static const JCharacter* kDNDDirectSave0XAtomName = "XdndDirectSave0";

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

const JUInt32 kDNDScrollTargetMask     = 0x00000200;
const JUInt32 kDNDScrollTargetDownMask = 0x00000100;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDNDManager::JXDNDManager
	(	
	JXDisplay* display
	)
	:
	itsPrevHandleDNDModifiers(0)
{
	itsDisplay            = display;
	itsIsDraggingFlag     = kJFalse;
	itsDragger            = NULL;
	itsDraggerWindow      = None;
	itsTargetFinder       = NULL;
	itsMouseWindow        = None;
	itsMouseWindowIsAware = kJFalse;
	itsMouseContainer     = NULL;
	itsMsgWindow          = None;

	itsDraggerTypeList = new JArray<Atom>;
	assert( itsDraggerTypeList != NULL );

	itsDraggerAskActionList = new JArray<Atom>;
	assert( itsDraggerAskActionList != NULL );

	itsDraggerAskDescripList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsDraggerAskDescripList != NULL );

	itsChooseDropActionDialog = NULL;
	itsUserDropAction         = NULL;

	itsSentFakePasteFlag = kJFalse;

	InitCursors();

	// create required X atoms

	itsDNDSelectionName = itsDisplay->RegisterXAtom(kDNDSelectionXAtomName);

	itsDNDProxyXAtom    = itsDisplay->RegisterXAtom(kDNDProxyXAtomName);
	itsDNDAwareXAtom    = itsDisplay->RegisterXAtom(kDNDAwareXAtomName);
	itsDNDTypeListXAtom = itsDisplay->RegisterXAtom(kDNDTypeListXAtomName);

	itsDNDEnterXAtom    = itsDisplay->RegisterXAtom(kDNDEnterXAtomName);
	itsDNDHereXAtom     = itsDisplay->RegisterXAtom(kDNDHereXAtomName);
	itsDNDStatusXAtom   = itsDisplay->RegisterXAtom(kDNDStatusXAtomName);
	itsDNDLeaveXAtom    = itsDisplay->RegisterXAtom(kDNDLeaveXAtomName);
	itsDNDDropXAtom     = itsDisplay->RegisterXAtom(kDNDDropXAtomName);
	itsDNDFinishedXAtom = itsDisplay->RegisterXAtom(kDNDFinishedXAtomName);

	itsDNDActionCopyXAtom       = itsDisplay->RegisterXAtom(kDNDActionCopyXAtomName);
	itsDNDActionMoveXAtom       = itsDisplay->RegisterXAtom(kDNDActionMoveXAtomName);
	itsDNDActionLinkXAtom       = itsDisplay->RegisterXAtom(kDNDActionLinkXAtomName);
	itsDNDActionAskXAtom        = itsDisplay->RegisterXAtom(kDNDActionAskXAtomName);
	itsDNDActionPrivateXAtom    = itsDisplay->RegisterXAtom(kDNDActionPrivateXAtomName);
	itsDNDActionDirectSaveXAtom = itsDisplay->RegisterXAtom(kDNDActionDirectSaveXAtomName);

	itsDNDActionListXAtom        = itsDisplay->RegisterXAtom(kDNDActionListXAtomName);
	itsDNDActionDescriptionXAtom = itsDisplay->RegisterXAtom(kDNDActionDescriptionXAtomName);

	itsDNDDirectSave0XAtom = itsDisplay->RegisterXAtom(kDNDDirectSave0XAtomName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDNDManager::~JXDNDManager()
{
	delete itsDraggerTypeList;
	delete itsDraggerAskActionList;
	delete itsDraggerAskDescripList;
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
	if (itsDragger != NULL)
		{
		return kJFalse;
		}

	assert( (widget->GetWindow())->IsDragging() );

	if (itsMouseContainer != NULL)
		{
		StopListening(itsMouseContainer);
		}

	itsMouseWindow                  = None;
	itsMouseWindowIsAware           = kJFalse;
	itsMouseContainer               = NULL;
	itsMsgWindow                    = None;
	itsPrevHandleDNDAction          = None;
	itsPrevHandleDNDScrollDirection = 0;
	itsPrevHandleDNDModifiers.Clear();

	if ((itsDisplay->GetSelectionManager())->SetData(itsDNDSelectionName, data))
		{
		itsIsDraggingFlag   = kJTrue;
		itsDragger          = widget;
		itsDraggerWindow    = (itsDragger->GetWindow())->GetXWindow();
		*itsDraggerTypeList = data->GetTypeList();
		itsTargetFinder     = targetFinder;

		ListenTo(itsDragger);
		itsDragger->BecomeDNDSource();
		itsDragger->DNDInit(pt, buttonStates, modifiers);
		itsDragger->HandleDNDResponse(NULL, kJFalse, None);		// set initial cursor

		AnnounceTypeList(itsDraggerWindow, *itsDraggerTypeList);

		HandleDND(pt, buttonStates, modifiers, 0);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HandleDND

	scrollDirection can be -1, 0, +1

 ******************************************************************************/

void
JXDNDManager::HandleDND
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	const JInteger			scrollDirection
	)
{
	assert( itsDragger != NULL );

	JXContainer* dropWidget;
	Window xWindow, msgWindow;
	Atom dndVers;
	const JBoolean isDNDAware =
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
	if (dropAction == itsDNDActionAskXAtom &&
		itsPrevHandleDNDAction != itsDNDActionAskXAtom)
		{
		AnnounceAskActions(buttonStates, modifiers);
		}

	// contact the target

	itsPrevHandleDNDPt              = pt;
	itsPrevHandleDNDAction          = dropAction;
	itsPrevHandleDNDScrollDirection = scrollDirection;
	itsPrevHandleDNDModifiers       = modifiers;
	SendDNDHere(pt, dropAction, scrollDirection, modifiers);

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

JBoolean
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
	JBoolean isAware = kJFalse;
	*xWindow         = None;
	*msgWindow       = None;
	*target          = NULL;
	*vers            = 0;

	if (itsTargetFinder != NULL)
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
			JBoolean isDock = kJFalse;		// not required for reference impl

			XClassHint hint;
			if (XGetClassHint(*itsDisplay, xWindow2, &hint))
				{
				if (hint.res_name != NULL &&
					strcmp(hint.res_name, JXGetDockWindowClass()) == 0)
					{
					isDock = kJTrue;
					}

				XFree(hint.res_name);
				XFree(hint.res_class);
				}

			if (!isDock)
				{
				*xWindow = xWindow2;
				isAware  = kJTrue;
				ptG.Set(x2, y2);
				break;
				}
			}

		if (childWindow == None)
			{
			*xWindow   = xWindow2;
			*msgWindow = xWindow2;
			isAware    = kJFalse;
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
		// If a local window is deactivated, FindContainer() returns kJFalse.
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
	if (itsDragger != NULL)
		{
		itsIsDraggingFlag = kJFalse;		// don't grab ESC any longer

		if (WaitForLastStatusMsg())
			{
			SendDNDDrop();
			}
		else
			{
			SendDNDLeave(kJTrue);
			}

		FinishDND1();
		}
}

/******************************************************************************
 CancelDND

	Returns kJTrue if the cancel was during a drag.

 ******************************************************************************/

JBoolean
JXDNDManager::CancelDND()
{
	if (itsIsDraggingFlag)	// can't use IsDragging() because called if dragger deleted
		{
		if (itsDragger != NULL)
			{
			itsDragger->DNDFinish(kJFalse, NULL);
			}
		SendDNDLeave();
		FinishDND1();
		return kJTrue;
		}

	return kJFalse;
}

/******************************************************************************
 FinishDND1 (private)

 ******************************************************************************/

void
JXDNDManager::FinishDND1()
{
	// need to check itsMouseContainer because if drop in other app,
	// the data has not yet been transferred

	if (itsDragger != NULL && itsMouseContainer != NULL)
		{
		itsDragger->DNDCompletelyFinished();
		}
	if (itsDragger != NULL)
		{
		itsDragger->FinishDNDSource();
		StopListening(itsDragger);
		}
	if (itsMouseContainer != NULL)
		{
		StopListening(itsMouseContainer);
		}

	itsIsDraggingFlag     = kJFalse;
	itsDragger            = NULL;
	itsDraggerWindow      = None;
	itsTargetFinder       = NULL;
	itsMouseWindow        = None;
	itsMouseWindowIsAware = kJFalse;
	itsMouseContainer     = NULL;
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
					itsDNDAwareXAtom, XA_ATOM, 32,
					PropModeReplace,
					(unsigned char*) &kCurrentDNDVersion, 1);
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
	JBoolean result = kJFalse;
	*proxy          = xWindow;
	*vers           = 0;

	// check XdndProxy

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* rawData = NULL;
	XGetWindowProperty(*itsDisplay, xWindow, itsDNDProxyXAtom,
					   0, LONG_MAX, False, XA_WINDOW,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &rawData);

	if (actualType == XA_WINDOW && actualFormat == 32 && itemCount > 0)
		{
		*proxy = *(reinterpret_cast<Window*>(rawData));

		XFree(rawData);
		rawData = NULL;

		// check XdndProxy on proxy window -- must point to itself

		XGetWindowProperty(*itsDisplay, *proxy, itsDNDProxyXAtom,
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
	rawData = NULL;

	// check XdndAware

	XGetWindowProperty(*itsDisplay, *proxy, itsDNDAwareXAtom,
					   0, LONG_MAX, False, XA_ATOM,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &rawData);

	if (actualType == XA_ATOM && actualFormat == 32 && itemCount > 0)
		{
		Atom* data = reinterpret_cast<Atom*>(rawData);
		if (data[0] >= kMinDNDVersion)
			{
			result = kJTrue;
			*vers  = JMin(kCurrentDNDVersion, data[0]);

			#if JXDND_DEBUG_MSGS
			cout << "Using XDND version " << *vers << endl;
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
						itsDNDTypeListXAtom, XA_ATOM, 32,
						PropModeReplace,
						(unsigned char*) list.GetCArray(), typeCount);
		}
}

/******************************************************************************
 DraggerCanProvideText (private)

	Returns kJTrue if text/plain is in itsDraggerTypeList.

 ******************************************************************************/

JBoolean
JXDNDManager::DraggerCanProvideText()
	const
{
	if (itsDragger == NULL)
		{
		return kJFalse;
		}

	const Atom textAtom = (itsDisplay->GetSelectionManager())->GetMimePlainTextXAtom();

	const JSize count = itsDraggerTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (itsDraggerTypeList->GetElement(i) == textAtom)
			{
			return kJTrue;
			}
		}

	return kJFalse;
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
	assert( itsDragger != NULL );

	itsDraggerAskActionList->RemoveAll();
	itsDraggerAskDescripList->CleanOut();

	itsDragger->GetDNDAskActions(buttonStates, modifiers,
								 itsDraggerAskActionList, itsDraggerAskDescripList);

	const JSize count = itsDraggerAskActionList->GetElementCount();
	assert( count >= 2 && count == itsDraggerAskDescripList->GetElementCount() );

	XChangeProperty(*itsDisplay, itsDraggerWindow,
					itsDNDActionListXAtom, XA_ATOM, 32,
					PropModeReplace,
					(unsigned char*) itsDraggerAskActionList->GetCArray(), count);

	const JString descripData = JXPackStrings(*itsDraggerAskDescripList);
	XChangeProperty(*itsDisplay, itsDraggerWindow,
					itsDNDActionDescriptionXAtom, XA_STRING, 8,
					PropModeReplace,
					(unsigned char*) descripData.GetCString(),
					descripData.GetLength());
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
	if (itsDragger != NULL)
		{
		*actionList = *itsDraggerAskActionList;
		descriptionList->CopyObjects(*itsDraggerAskDescripList,
									 descriptionList->GetCleanUpAction(), kJFalse);
		}
	else
		{
		actionList->RemoveAll();
		descriptionList->CleanOut();

		Atom actualType;
		int actualFormat;
		unsigned long itemCount, remainingBytes;
		unsigned char* rawData = NULL;

		// get action atoms

		XGetWindowProperty(*itsDisplay, itsDraggerWindow, itsDNDActionListXAtom,
						   0, LONG_MAX, False, XA_ATOM,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &rawData);

		if (actualType == XA_ATOM && actualFormat == 32 && itemCount > 0)
			{
			Atom* data = reinterpret_cast<Atom*>(rawData);
			for (JIndex i=0; i<itemCount; i++)
				{
				actionList->AppendElement(data[i]);
				}
			}

		XFree(rawData);

		// get action descriptions

		XGetWindowProperty(*itsDisplay, itsDraggerWindow, itsDNDActionDescriptionXAtom,
						   0, LONG_MAX, False, XA_STRING,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &rawData);

		if (actualType == XA_STRING && actualFormat == 8 && itemCount > 0)
			{
			JXUnpackStrings(reinterpret_cast<JCharacter*>(rawData), itemCount,
							descriptionList);
			}

		XFree(rawData);
		}

	// return status

	if (actionList->GetElementCount() != descriptionList->GetElementCount())
		{
		actionList->RemoveAll();
		descriptionList->CleanOut();
		return kJFalse;
		}
	else
		{
		return !actionList->IsEmpty();
		}
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
	assert( itsChooseDropActionDialog == NULL );

	JXApplication* app = JXGetApplication();
	app->PrepareForBlockingWindow();

	itsChooseDropActionDialog =
		new JXDNDChooseDropActionDialog(actionList, descriptionList, *action);
	assert( itsChooseDropActionDialog != NULL );

	ListenTo(itsChooseDropActionDialog);
	itsChooseDropActionDialog->BeginDialog();

	// display the inactive cursor in all the other windows

	app->DisplayInactiveCursor();

	// block with event loop running until we get a response

	itsUserDropAction = action;

	JXWindow* window = itsChooseDropActionDialog->GetWindow();
	while (itsChooseDropActionDialog != NULL)
		{
		app->HandleOneEventForWindow(window);
		}

	app->BlockingWindowFinished();

	itsUserDropAction = NULL;
	return JI2B( *action != None );
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
 SendDNDEnter (private)

 ******************************************************************************/

void
JXDNDManager::SendDNDEnter
	(
	const Window	xWindow,
	const Window	msgWindow,
	JXContainer*	widget,
	const JBoolean	isAware,
	const Atom		vers
	)
{
	assert( xWindow != None );

	itsDNDVersion         = vers;
	itsMouseWindow        = xWindow;
	itsMouseWindowIsAware = isAware;
	itsMouseContainer     = widget;
	itsMsgWindow          = msgWindow;

	if (itsMouseContainer != NULL)
		{
		ListenTo(itsMouseContainer);
		}

	itsWillAcceptDropFlag = kJFalse;
	itsWaitForStatusFlag  = kJFalse;
	itsSendHereMsgFlag    = kJFalse;
	itsReceivedStatusFlag = kJFalse;
	itsUseMouseRectFlag   = kJFalse;
	itsMouseRectR         = JRect(0,0,0,0);
	itsPrevStatusAction   = None;

	itsDragger->HandleDNDResponse(NULL, kJFalse, None);		// reset cursor

	if (itsMouseContainer != NULL)
		{
		itsPrevHereAction = None;
		}
	else if (itsMouseWindowIsAware)
		{
		#if JXDND_DEBUG_MSGS
		cout << "Sent XdndEnter" << endl;
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
		message.message_type = itsDNDEnterXAtom;
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
	const JInteger			scrollDirection,
	const JXKeyModifiers&	modifiers
	)
{
	const JPoint ptG1 = itsDragger->JXContainer::LocalToGlobal(pt1);
	const JPoint ptR  = (itsDragger->GetWindow())->GlobalToRoot(ptG1);

	const JBoolean shouldSendMessage = JI2B(
						itsMouseWindowIsAware &&
						(!itsUseMouseRectFlag || !itsMouseRectR.Contains(ptR) ||
						 scrollDirection != 0) );

	if (itsMouseContainer != NULL)
		{
		// Scroll first, because it affects drop location.
		// Always call it, in case mouse is inside a scroll zone.

		const JPoint ptG2 = (itsMouseContainer->GetWindow())->RootToGlobal(ptR);
		const JPoint pt2  = itsMouseContainer->GlobalToLocal(ptG2);
		itsMouseContainer->DNDScroll(pt2, scrollDirection, modifiers);

		const JBoolean savedAccept = itsWillAcceptDropFlag;
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

		itsReceivedStatusFlag = kJTrue;
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
		cout << "Sent XdndPosition: " << ptR.x << ' ' << ptR.y << endl;
		#endif

		#if JXDND_SOURCE_DELAY > 0
		JWait(JXDND_SOURCE_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsMouseWindow;
		message.message_type = itsDNDHereXAtom;
		message.format       = 32;

		message.data.l[ kDNDHereWindow    ] = itsDraggerWindow;
		message.data.l[ kDNDHereFlags     ] = 0;
		message.data.l[ kDNDHerePt        ] = PackPoint(ptR);
		message.data.l[ kDNDHereTimeStamp ] = itsDisplay->GetLastEventTime();
		message.data.l[ kDNDHereAction    ] = action;

		if (scrollDirection != 0)
			{
			message.data.l[ kDNDHereFlags ] |= kDNDScrollTargetMask;
			if (scrollDirection > 0)
				{
				message.data.l[ kDNDHereFlags ] |= kDNDScrollTargetDownMask;
				}

			message.data.l[ kDNDHereFlags ] |= (modifiers.GetState() & 0x00FF);
			}

		itsDisplay->SendXEvent(itsMsgWindow, &xEvent);

		itsWaitForStatusFlag = kJTrue;
		itsSendHereMsgFlag   = kJFalse;
		}

	else if (itsWaitForStatusFlag && shouldSendMessage)
		{
		itsSendHereMsgFlag = kJTrue;
		}
}

/******************************************************************************
 SendDNDLeave (private)

 ******************************************************************************/

void
JXDNDManager::SendDNDLeave
	(
	const JBoolean sendPasteClick
	)
{
	if (itsMouseContainer != NULL)
		{
		if (itsWillAcceptDropFlag)
			{
			itsMouseContainer->DNDLeave();
			}
		}
	else if (itsMouseWindowIsAware)
		{
		#if JXDND_DEBUG_MSGS
		cout << "Sent XdndLeave" << endl;
		#endif

		#if JXDND_SOURCE_DELAY > 0
		JWait(JXDND_SOURCE_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsMouseWindow;
		message.message_type = itsDNDLeaveXAtom;
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
		if (dropWidget == NULL && xWindow != None && xWindow != rootWindow)
			{
			// this isn't necessary -- our clipboard continues to work as usual
			// (itsDisplay->GetSelectionManager())->ClearData(kJXClipboardName);

			const Window xferWindow  = (itsDisplay->GetSelectionManager())->GetDataTransferWindow();
			const Time lastEventTime = itsDisplay->GetLastEventTime();
			XSetSelectionOwner(*itsDisplay, kJXClipboardName, xferWindow, CurrentTime);
			if (XGetSelectionOwner(*itsDisplay, kJXClipboardName) == xferWindow)
				{
				PrepareForDrop(NULL);

				itsSentFakePasteFlag     = kJTrue;
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
				cout << "Sent fake middle click, time=" << xEvent.xbutton.time << endl;
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

	if (itsMouseContainer != NULL)
		{
		itsMouseContainer->DNDDrop(itsPrevMousePt, *itsDraggerTypeList,
								   itsPrevStatusAction, CurrentTime, itsDragger);
		}
	else if (itsMouseWindowIsAware)
		{
		#if JXDND_DEBUG_MSGS
		cout << "Sent XdndDrop" << endl;
		#endif

		#if JXDND_SOURCE_DELAY > 0
		JWait(JXDND_SOURCE_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsMouseWindow;
		message.message_type = itsDNDDropXAtom;
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
	const JXSelectionData* data = NULL;
	if ((itsDisplay->GetSelectionManager())->
			GetData(itsDNDSelectionName, CurrentTime, &data))
		{
		data->Resolve();
		}

	itsDragger->DNDFinish(kJTrue, target);
}

/******************************************************************************
 SendDNDStatus (private)

	This is sent to let the source know that we are ready for another
	DNDHere message.

 ******************************************************************************/

void
JXDNDManager::SendDNDStatus
	(
	const JBoolean	willAcceptDrop,
	const Atom		action
	)
{
	if (itsDraggerWindow != None)
		{
		#if JXDND_DEBUG_MSGS
		cout << "Sent XdndStatus: " << willAcceptDrop << endl;
		#endif

		#if JXDND_TARGET_DELAY > 0
		JWait(JXDND_TARGET_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsDraggerWindow;
		message.message_type = itsDNDStatusXAtom;
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
		cout << "Sent XdndFinished" << endl;
		#endif

		#if JXDND_TARGET_DELAY > 0
		JWait(JXDND_TARGET_DELAY);
		#endif

		XEvent xEvent;
		XClientMessageEvent& message = xEvent.xclient;

		message.type         = ClientMessage;
		message.display      = *itsDisplay;
		message.window       = itsDraggerWindow;
		message.message_type = itsDNDFinishedXAtom;
		message.format       = 32;

		message.data.l[ kDNDFinishedWindow ] = itsMouseWindow;
		message.data.l[ kDNDFinishedFlags ]  = 0;

		itsDisplay->SendXEvent(itsDraggerWindow, &xEvent);
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
	// target:  receive and process window-level enter and leave

	if (clientMessage.message_type == itsDNDEnterXAtom)
		{
		HandleDNDEnter(clientMessage);
		return kJTrue;
		}

	else if (clientMessage.message_type == itsDNDHereXAtom)
		{
		HandleDNDHere(clientMessage);
		return kJTrue;
		}

	else if (clientMessage.message_type == itsDNDLeaveXAtom)
		{
		HandleDNDLeave(clientMessage);
		return kJTrue;
		}

	else if (clientMessage.message_type == itsDNDDropXAtom)
		{
		HandleDNDDrop(clientMessage);
		return kJTrue;
		}

	// source:  clear our flag when we receive status message

	else if (clientMessage.message_type == itsDNDStatusXAtom)
		{
		HandleDNDStatus(clientMessage);
		return kJTrue;
		}

	// source:  ignore finished message
	//			because JXSelectionManager rejects outdated requests

	else if (clientMessage.message_type == itsDNDFinishedXAtom)
		{
		#if JXDND_DEBUG_MSGS
		cout << "Received XdndFinished" << endl;
		#endif

		return kJTrue;
		}

	// not for us

	else
		{
		return kJFalse;
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
	assert( clientMessage.message_type == itsDNDEnterXAtom );

	itsDNDVersion =
		(clientMessage.data.l[ kDNDEnterFlags ] >> kDNDEnterVersionRShift) &
		kDNDEnterVersionMask;

	if (itsDraggerWindow == None &&
		kMinDNDVersion <= itsDNDVersion && itsDNDVersion <= kCurrentDNDVersion &&
		clientMessage.data.l[ kDNDEnterType1 ] != None)
		{
		#if JXDND_TARGET_DELAY > 0
			#if JXDND_DEBUG_MSGS
			cout << "XdndEnter will arrive" << endl;
			#endif
		JWait(JXDND_TARGET_DELAY);
		#endif

		#if JXDND_DEBUG_MSGS
		cout << "Received XdndEnter" << endl;
		#endif

		if (itsDragger != NULL)
			{
			StopListening(itsDragger);
			}
		if (itsMouseContainer != NULL)
			{
			StopListening(itsMouseContainer);
			}

		itsIsDraggingFlag = kJFalse;
		itsDragger        = NULL;
		itsDraggerWindow  = clientMessage.data.l[ kDNDEnterWindow ];
		itsMouseWindow    = clientMessage.window;
		itsMouseContainer = NULL;
		itsPrevHereAction = None;

		XSelectInput(*itsDisplay, itsDraggerWindow, StructureNotifyMask);
		ListenTo(itsDisplay);

		itsDraggerTypeList->RemoveAll();
		if (clientMessage.data.l[ kDNDEnterFlags ] & kDNDEnterMoreTypesFlag != 0)
			{
			Atom actualType;
			int actualFormat;
			unsigned long itemCount, remainingBytes;
			unsigned char* rawData = NULL;
			XGetWindowProperty(*itsDisplay, itsDraggerWindow, itsDNDTypeListXAtom,
							   0, LONG_MAX, False, XA_ATOM,
							   &actualType, &actualFormat,
							   &itemCount, &remainingBytes, &rawData);

			if (actualType == XA_ATOM && actualFormat == 32 && itemCount > 0)
				{
				Atom* data = reinterpret_cast<Atom*>(rawData);
				for (JIndex i=0; i<itemCount; i++)
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
	assert( clientMessage.message_type == itsDNDHereXAtom );

	if (itsDraggerWindow != (Window) clientMessage.data.l[ kDNDHereWindow ])
		{
		return;
		}

	#if JXDND_TARGET_DELAY > 0
		#if JXDND_DEBUG_MSGS
		cout << "XdndPosition will arrive" << endl;
		#endif
	JWait(JXDND_TARGET_DELAY);
	#endif

	#if JXDND_DEBUG_MSGS
	cout << "Received XdndPosition" << endl;
	#endif

	const JPoint ptR = UnpackPoint(clientMessage.data.l[ kDNDHerePt ]);
	JPoint ptG, pt;

	const Time time       = clientMessage.data.l[ kDNDHereTimeStamp ];
	Atom action           = clientMessage.data.l[ kDNDHereAction ];
	const Atom origAction = action;

	JXWindow* window;
	if (itsMouseContainer != NULL)
		{
		window = itsMouseContainer->GetWindow();
		ptG    = window->RootToGlobal(ptR);
		JXContainer* newMouseContainer;
		const JBoolean found = itsDisplay->FindMouseContainer(window, ptG, &newMouseContainer);
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
				itsMouseContainer->WillAcceptDrop(*itsDraggerTypeList, &action, pt, time, NULL);
			if (itsWillAcceptDropFlag)
				{
				itsMouseContainer->DNDEnter();
				}
			}
		else if (found)
			{
			const JBoolean savedAccept = itsWillAcceptDropFlag;
			pt = itsMouseContainer->GlobalToLocal(ptG);
			InvokeDNDScroll(clientMessage, pt);
			itsWillAcceptDropFlag =
				itsMouseContainer->WillAcceptDrop(*itsDraggerTypeList, &action, pt, time, NULL);
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
				itsMouseContainer->WillAcceptDrop(*itsDraggerTypeList, &action, pt, time, NULL);
			if (itsWillAcceptDropFlag)
				{
				itsMouseContainer->DNDEnter();
				}
			}
		}

	itsPrevHereAction = origAction;

	if (itsMouseContainer != NULL && itsWillAcceptDropFlag)
		{
		itsPrevMousePt      = pt;
		itsPrevStatusAction = action;
		itsMouseContainer->DNDHere(itsPrevMousePt, NULL);
		SendDNDStatus(kJTrue, action);
		}
	else
		{
		SendDNDStatus(kJFalse, None);
		}
}

void
JXDNDManager::InvokeDNDScroll
	(
	const XClientMessageEvent&	clientMessage,
	const JPoint&				pt
	)
{
	const JInteger direction =
		clientMessage.data.l[ kDNDHereFlags ] & kDNDScrollTargetDownMask ? +1 : -1;
	const JXKeyModifiers modifiers(itsDisplay, clientMessage.data.l[ kDNDHereFlags ] & 0x000000FF);

	itsMouseContainer->DNDScroll(
		pt,
		(clientMessage.data.l[ kDNDHereFlags ] & kDNDScrollTargetMask) ? direction : 0,
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
	assert( clientMessage.message_type == itsDNDLeaveXAtom );

	if (itsDraggerWindow == (Window) clientMessage.data.l[ kDNDLeaveWindow ])
		{
		#if JXDND_TARGET_DELAY > 0
			#if JXDND_DEBUG_MSGS
			cout << "XdndLeave will arrive" << endl;
			#endif
		JWait(JXDND_TARGET_DELAY);
		#endif

		#if JXDND_DEBUG_MSGS
		cout << "Received XdndLeave" << endl;
		#endif

		HandleDNDLeave1();
		}
}

/******************************************************************************
 HandleDNDLeave1 (private)

 ******************************************************************************/

void
JXDNDManager::HandleDNDLeave1()
{
	if (itsMouseContainer != NULL && itsWillAcceptDropFlag)
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
	assert( clientMessage.message_type == itsDNDDropXAtom );

	if (itsDraggerWindow == (Window) clientMessage.data.l[ kDNDDropWindow ])
		{
		#if JXDND_TARGET_DELAY > 0
			#if JXDND_DEBUG_MSGS
			cout << "XdndDrop will arrive" << endl;
			#endif
		JWait(JXDND_TARGET_DELAY);
		#endif

		#if JXDND_DEBUG_MSGS
		cout << "Received XdndDrop" << endl;
		#endif

		const Time time = clientMessage.data.l[ kDNDDropTimeStamp ];

		if (itsMouseContainer != NULL && itsWillAcceptDropFlag)
			{
			itsMouseContainer->DNDDrop(itsPrevMousePt, *itsDraggerTypeList,
									   itsPrevStatusAction, time, NULL);
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

	if (itsDragger != NULL)
		{
		StopListening(itsDragger);
		}
	if (itsMouseContainer != NULL)
		{
		StopListening(itsMouseContainer);
		}

	itsIsDraggingFlag = kJFalse;
	itsDragger        = NULL;
	itsDraggerWindow  = None;
	itsMouseWindow    = None;
	itsMouseContainer = NULL;
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
	assert( clientMessage.message_type == itsDNDStatusXAtom );

	if (itsDragger != NULL &&
		itsMouseWindow == (Window) clientMessage.data.l[ kDNDStatusWindow ])
		{
		#if JXDND_SOURCE_DELAY > 0
			#if JXDND_DEBUG_MSGS
			cout << "XdndStatus will arrive" << endl;
			#endif
		JWait(JXDND_SOURCE_DELAY);
		#endif

		const JBoolean savedAccept = itsWillAcceptDropFlag;

		itsWaitForStatusFlag  = kJFalse;
		itsReceivedStatusFlag = kJTrue;

		itsWillAcceptDropFlag = JConvertToBoolean(
			clientMessage.data.l[ kDNDStatusFlags ] & kDNDStatusAcceptDropFlag );

		itsUseMouseRectFlag = JNegate(
			clientMessage.data.l[ kDNDStatusFlags ] & kDNDStatusSendHereFlag );

		itsMouseRectR = UnpackRect(clientMessage.data.l[ kDNDStatusPt ],
								   clientMessage.data.l[ kDNDStatusArea ]);

		#if JXDND_DEBUG_MSGS
		cout << "Received XdndStatus: " << itsWillAcceptDropFlag;
		cout << ' ' << itsUseMouseRectFlag;
		cout << ' ' << itsMouseRectR.top << ' ' << itsMouseRectR.left;
		cout << ' ' << itsMouseRectR.bottom << ' ' << itsMouseRectR.right << endl;
		#endif

		Atom action = clientMessage.data.l[ kDNDStatusAction ];
		if (!itsWillAcceptDropFlag)
			{
			action = None;
			}

		if (itsWillAcceptDropFlag != savedAccept ||
			itsPrevStatusAction != action)
			{
			itsDragger->HandleDNDResponse(NULL, itsWillAcceptDropFlag, action);
			itsPrevStatusAction = action;
			}

		if (itsSendHereMsgFlag)
			{
			SendDNDHere(itsPrevHandleDNDPt, itsPrevHandleDNDAction,
						itsPrevHandleDNDScrollDirection, itsPrevHandleDNDModifiers);
			}
		}
}

/******************************************************************************
 WaitForLastStatusMsg (private)

	Returns kJTrue if it receives XdndStatus message with accept==kJTrue.

 ******************************************************************************/

JBoolean
JXDNDManager::WaitForLastStatusMsg()
{
	if (!itsReceivedStatusFlag)
		{
		return kJFalse;
		}
	else if (!itsWaitForStatusFlag)
		{
		return itsWillAcceptDropFlag;
		}

	itsSendHereMsgFlag = kJFalse;	// don't send any more messages

	JXSelectionManager* selMgr = itsDisplay->GetSelectionManager();

	Atom messageList[] = { 1, itsDNDStatusXAtom };

	XEvent xEvent;
	clock_t endTime = clock() + kWaitForLastStatusTime;
	while (clock() < endTime)
		{
		if (XCheckIfEvent(*itsDisplay, &xEvent, GetNextStatusEvent,
						  reinterpret_cast<char*>(messageList)))
			{
			if (xEvent.type == ClientMessage)
				{
				const JBoolean ok = HandleClientMessage(xEvent.xclient);
				assert( ok );
				if (xEvent.xclient.message_type == itsDNDStatusXAtom)
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

	return kJFalse;
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
		JXDisplay::XError* err = dynamic_cast(JXDisplay::XError*, message);
		assert( err != NULL );

		// source: target crashed -- nothing to do

		// target: source crashed

		if (itsDragger == NULL && itsDraggerWindow != None &&
			err->GetType() == BadWindow &&
			err->GetXID()  == itsDraggerWindow)
			{
			#if JXDND_DEBUG_MSGS
			cout << "JXDND: Source crashed via XError" << endl;
			#endif

			itsDraggerWindow = None;
			HandleDNDLeave1();
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

JBoolean
JXDNDManager::HandleDestroyNotify
	(
	const XDestroyWindowEvent& xEvent
	)
{
	if (itsDragger == NULL && itsDraggerWindow != None &&
		xEvent.window == itsDraggerWindow)
		{
		#if JXDND_DEBUG_MSGS
		cout << "JXDND: Source crashed via DestroyNotify" << endl;
		#endif

		itsDraggerWindow = None;
		HandleDNDLeave1();
		return kJTrue;
		}
	else
		{
		return kJFalse;
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
		itsDragger = NULL;
		CancelDND();
		}
	else if (sender == itsMouseContainer)
		{
		itsMouseContainer = NULL;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 Default cursors

 ******************************************************************************/

static const JCharacter* kDefaultDNDCursorName[] =
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

// generic

#include <jx_drag_object_cursor.xbm>
#include <jx_drag_object_cursor_mask.xbm>
#include <jx_drag_copy_object_cursor.xbm>
#include <jx_drag_link_object_cursor.xbm>
#include <jx_drag_ask_object_cursor.xbm>
#include <jx_drag_mod_object_cursor_mask.xbm>

// file

#include <jx_drag_file_cursor.xbm>
#include <jx_drag_file_cursor_mask.xbm>
#include <jx_drag_copy_file_cursor.xbm>
#include <jx_drag_link_file_cursor.xbm>
#include <jx_drag_ask_file_cursor.xbm>
#include <jx_drag_mod_file_cursor_mask.xbm>

// directory

#include <jx_drag_directory_cursor.xbm>
#include <jx_drag_directory_cursor_mask.xbm>
#include <jx_drag_copy_directory_cursor.xbm>
#include <jx_drag_link_directory_cursor.xbm>
#include <jx_drag_ask_directory_cursor.xbm>
#include <jx_drag_mod_directory_cursor_mask.xbm>

// mixed

#include <jx_drag_file_and_directory_cursor.xbm>
#include <jx_drag_file_and_directory_cursor_mask.xbm>
#include <jx_drag_copy_file_and_directory_cursor.xbm>
#include <jx_drag_link_file_and_directory_cursor.xbm>
#include <jx_drag_ask_file_and_directory_cursor.xbm>
#include <jx_drag_mod_file_and_directory_cursor_mask.xbm>

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
	assert( sizeof(kDefaultDNDCursorName)/sizeof(JCharacter*) == kDefDNDCursorCount );

	for (JIndex i=0; i<kDefDNDCursorCount; i++)
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
	const JBoolean		dropAccepted,
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
