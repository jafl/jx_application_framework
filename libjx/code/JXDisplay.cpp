/******************************************************************************
 JXDisplay.cpp

	Represents a single X Display.  We consider a display to be an X server
	plus a particular screen.

	Since cursors should be the same on every display, but each X server will
	provide different cursor id's, we hide cursors behind JCursorIndex.

	Cursors are bulky and difficult to compare, so we require that each
	one be given a unique name.  This way, every client that needs a particular
	cursor can use GetCursor() with only the name.  Only if GetCursor()
	fails does one of the cursor creation routines need to be called.  There
	are two such routines because X makes a distinction between the default
	cursor font and custom cursors.

	Cursor names can be anything, as long as they don't conflict.  All objects
	that use a particular cursor must know the name.  Since the X cursor font
	can be used by all objects, it is suggested that one uses the name of
	the associated XC_* constant as the cursor name.

	We cannot sort the cursor array because the indices that we give out must
	remain valid.  Appending new items is the only option.  This is not much
	of a problem, however, both because the most common cursors have predefined
	indices, and because looking up cursors only has to be done once in each
	constructor.

	Because we are receiving events over a network which introduces an arbitrary
	delay between the occurrence of an event and our processing of it,
	mouse and keyboard events can be reported for the wrong (original) window
	while we are switching a grab from one window to another.  (e.g. The user
	releases the mouse so soon after we switch the grab that the server hasn't
	received our message and therefore thinks that the original window is still
	grabbing.  We therefore get the ButtonRelease for the original window
	instead of for the new window.)  Our solution is to tell JXDisplay about
	who has grabbed what so JXDisplay can re-route the message to the current
	grabber.  This is what itsMouseGrabber and itsKeyboardGrabber are used for.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXDisplay.h"
#include "JXGC.h"
#include "JXColorManager.h"
#include "JXWindow.h"
#include "JXWindowDirector.h"
#include "JXFontManager.h"
#include "JXSelectionManager.h"
#include "JXDNDManager.h"
#include "JXMenuManager.h"
#include "JXWDManager.h"
#include "JXImageCache.h"
#include "JXCursor.h"
#include "jXGlobals.h"
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xproto.h>		// for error request codes
#ifdef _J_HAS_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JSize kMaxSleepTime = 50;		// 0.05 seconds (in milliseconds)

static const JUtf8Byte* kStandardXAtomNames[ JXDisplay::kStandardXAtomCount ] =
{
	"WM_STATE",
	"WM_CLIENT_MACHINE",
	"WM_PROTOCOLS",
	"WM_DELETE_WINDOW",
	"WM_SAVE_YOURSELF",
	"_NET_WM_PING",
	"_NET_WM_PID",
	"_NET_WM_USER_TIME",
	"_NET_ACTIVE_WINDOW",
	"_NET_WM_DESKTOP",
	"_NET_CURRENT_DESKTOP",

	// http://standards.freedesktop.org/wm-spec/latest/ar01s05.html

	"_NET_WM_WINDOW_TYPE",
	"_NET_WM_WINDOW_TYPE_NORMAL",
	"_NET_WM_WINDOW_TYPE_DIALOG",
	"_NET_WM_WINDOW_TYPE_DROPDOWN_MENU",
	"_NET_WM_WINDOW_TYPE_POPUP_MENU",
	"_NET_WM_WINDOW_TYPE_TOOLTIP",

	// private

	"JXWindowManagerBehaviorV0"
};

// prototypes

static int JXDebugAfterFunction(Display* xDisplay);

// JBroadcaster message types

const JUtf8Byte* JXDisplay::kXEventMessage = "XEventMessage::JXDisplay";
const JUtf8Byte* JXDisplay::kXError        = "XError::JXDisplay";

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about XOpenDisplay succeeding within the class itself.

	Note that this prevents one from creating derived classes.

 ******************************************************************************/

bool
JXDisplay::Create
	(
	const JString&	displayName,
	JXDisplay**		display
	)
{
	const JUtf8Byte* name = displayName.GetBytes();
	if (JString::IsEmpty(name))
	{
		name = nullptr;
	}

	Display* xDisplay = XOpenDisplay(name);
	if (xDisplay != nullptr)
	{
		*display = jnew JXDisplay(JString(XDisplayName(name), JString::kNoCopy), xDisplay);
		return *display != nullptr;
	}
	else
	{
		*display = nullptr;
		return false;
	}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXDisplay::JXDisplay
	(
	const JString&	displayName,
	Display*		xDisplay
	)
	:
	itsName(displayName),
	itsMaxStringByteCount( 4*(XMaxRequestSize(xDisplay)-3) * 4/5 ),
	itsLatestKeyModifiers(this)
{
	itsXDisplay             = xDisplay;
	itsColorManager         = JXColorManager::Create(this);
	itsLastEventTime        = 0;
	itsLastIdleTime         = 0;
	itsLastMotionNotifyTime = 0;

	itsBounds                        = nullptr;
	itsShrinkDisplayToScreenRefCount = 0;

	itsModifierKeymap = nullptr;
	UpdateModifierMapping();

	itsCursorList = jnew JArray<CursorInfo>;
	assert( itsCursorList != nullptr );

	itsWindowList = jnew JArray<WindowInfo>(10);
	assert( itsWindowList != nullptr );
	itsWindowList->SetCompareFunction(CompareXWindows);

	itsDefaultGC = jnew JXGC(this, GetRootWindow());
	assert( itsDefaultGC != nullptr );

	XSetLocaleModifiers("");				// loads the XMODIFIERS environment variable
	itsXIM = XOpenIM(itsXDisplay, nullptr, nullptr, nullptr);
	if (!itsXIM)
	{
		XSetLocaleModifiers("@im=none");	// fallback to internal input method
		itsXIM = XOpenIM(itsXDisplay, nullptr, nullptr, nullptr);
	}
	assert( itsXIM != nullptr );

	itsNeedsUpdateFlag = false;
	itsMouseContainer  = nullptr;
	itsMouseGrabber    = nullptr;
	itsKeyboardGrabber = nullptr;

	assert( kStandardXAtomCount == (sizeof(kStandardXAtomNames) / sizeof(JUtf8Byte*)) );
	RegisterXAtoms(kStandardXAtomCount, kStandardXAtomNames, itsStandardXAtoms);

	CreateBuiltInCursor("XC_left_ptr", XC_left_ptr);
	CreateBuiltInCursor("XC_xterm",    XC_xterm);
	CreateBuiltInCursor("XC_tcross",   XC_tcross);
	CreateBuiltInCursor("XC_watch",    XC_watch);
	CreateBuiltInCursor("XC_X_cursor", XC_X_cursor);

	itsFontManager = jnew JXFontManager(this);

	itsSelectionManager = jnew JXSelectionManager(this);

	itsDNDManager = jnew JXDNDManager(this);

	itsMenuManager = jnew JXMenuManager;

	itsWDManager = nullptr;

	itsImageCache = jnew JXImageCache(this);

	int major_opcode, first_event, first_error;
	itsIsMacOSFlag = XQueryExtension(itsXDisplay, "Apple-WM",
									 &major_opcode, &first_event, &first_error);

	JXGetApplication()->DisplayOpened(this);

	if (_Xdebug)
	{
		XSetAfterFunction(itsXDisplay, JXDebugAfterFunction);
	}

	if (!itsWMBehavior.Load(this))
	{
		JXWindow::AnalyzeWindowManager(this);
		itsWMBehavior.Save(this);
	}
}

/******************************************************************************
 Destructor

	We should only be deleted after all windows have been closed.

 ******************************************************************************/

JXDisplay::~JXDisplay()
{
	assert( itsWindowList->IsEmpty() );

	JXGetApplication()->DisplayClosed(this);

	jdelete itsWDManager;
	jdelete itsMenuManager;
	jdelete itsSelectionManager;
	jdelete itsDNDManager;
	jdelete itsFontManager;
	jdelete itsImageCache;

	jdelete itsWindowList;
	jdelete itsDefaultGC;
	jdelete itsColorManager;
	jdelete itsBounds;

	for (const auto& info : *itsCursorList)
	{
		jdelete (info.name);
		XFreeCursor(itsXDisplay, info.xid);
	}
	jdelete itsCursorList;

	XFreeModifiermap(itsModifierKeymap);
	XCloseIM(itsXIM);

	XCloseDisplay(itsXDisplay);
}

/******************************************************************************
 Close

	This is the safe way to delete a JXDisplay.

 ******************************************************************************/

bool
JXDisplay::Close()
{
	while (!itsWindowList->IsEmpty())
	{
		WindowInfo info = itsWindowList->GetLastItem();
		if (!info.window->GetDirector()->Close())
		{
			return false;
		}
	}

	jdelete this;
	return true;
}

/******************************************************************************
 RaiseAllWindows

	Raises all visible, non-iconified windows and maintains the stacking order.

 ******************************************************************************/

void
JXDisplay::RaiseAllWindows()
{
	// get list of all top level windows

	Window root, parent, *childList;
	unsigned int childCount;
	if (!XQueryTree(itsXDisplay, GetRootWindow(),
				   &root, &parent, &childList, &childCount))
	{
		return;
	}

	// initialize the mapping of X windows to JXWindows

	JPtrArray<JXWindow> childMapping(JPtrArrayT::kForgetAll, childCount);
	for (JIndex i=1; i<=childCount; i++)
	{
		childMapping.Append(nullptr);
	}

	// fill in the mapping of X windows to JXWindows

	for (const auto& info : *itsWindowList)
	{
		Window rootChild;
		if ((info.window)->GetRootChild(&rootChild))
		{
			// find the ancestor in the list of top level windows

			for (JUnsignedOffset j=0; j<childCount; j++)
			{
				if (childList[j] == rootChild)
				{
					childMapping.SetItem(j+1, info.window, JPtrArrayT::kForget);
					break;
				}
			}
		}
	}

	// raise the windows (first one is on the bottom)

	for (auto* w : childMapping)
	{
		if (w != nullptr && w->IsVisible() && !w->IsIconified())
		{
			w->Raise();
		}
	}

	XFree(childList);
}

/******************************************************************************
 HideAllWindows

	Hide all windows on this display.  Mainly useful when quitting.

 ******************************************************************************/

void
JXDisplay::HideAllWindows()
{
	for (const auto& info : *itsWindowList)
	{
		info.window->Hide();
	}
}

/******************************************************************************
 UndockAllWindows

 ******************************************************************************/

void
JXDisplay::UndockAllWindows()
{
	for (const auto& info : *itsWindowList)
	{
		info.window->Undock();
	}
}

/******************************************************************************
 CloseAllOtherWindows

	Calls Close() on all windows except the given one.

 ******************************************************************************/

void
JXDisplay::CloseAllOtherWindows
	(
	JXWindow* window
	)
{
	JArrayIterator iter(*itsWindowList);

	WindowInfo info;
	while (iter.Next(&info))
	{
		if (info.window != window)
		{
			info.window->Close();
		}
	}
}

/******************************************************************************
 WindowExists (static)

	Returns true if the given window hasn't been deleted.  Since we
	cannot assume that anything exists, everything that we need must
	be passed in.

 ******************************************************************************/

bool
JXDisplay::WindowExists
	(
	JXDisplay*		display,
	const Display*	xDisplay,
	const Window	xWindow
	)
{
	JXApplication* app;
	JXWindow* window;
	return JXGetApplication(&app) && app->DisplayExists(xDisplay) &&
			display->FindXWindow(xWindow, &window);
}

/******************************************************************************
 GetWDManager

 ******************************************************************************/

JXWDManager*
JXDisplay::GetWDManager()
	const
{
	assert( itsWDManager != nullptr );
	return itsWDManager;
}

/******************************************************************************
 SetWDManager

 ******************************************************************************/

void
JXDisplay::SetWDManager
	(
	JXWDManager* mgr
	)
{
	assert( itsWDManager == nullptr );
	itsWDManager = mgr;
}

/******************************************************************************
 GetFontManager

	Not inline to avoid including <JXFontManager.h> in header file.

 ******************************************************************************/

JFontManager*
JXDisplay::GetFontManager()
	const
{
	return itsFontManager;
}

/******************************************************************************
 Get resolution

	Screen resolution in pixels/inch (25.4 mm = 1 inch)

 ******************************************************************************/

JSize
JXDisplay::GetHorizResolution()
	const
{
	const int screen = GetScreen();
	return JRound(DisplayWidth(itsXDisplay, screen) /
				  (DisplayWidthMM(itsXDisplay, screen)/25.4));
}

JSize
JXDisplay::GetVertResolution()
	const
{
	const int screen = GetScreen();
	return JRound(DisplayHeight(itsXDisplay, screen) /
				  (DisplayHeightMM(itsXDisplay, screen)/25.4));
}

/******************************************************************************
 GetBounds

 ******************************************************************************/

JRect
JXDisplay::GetBounds()
	const
{
	#ifdef _J_HAS_XINERAMA

	if (itsBounds == nullptr && itsShrinkDisplayToScreenRefCount > 0)
	{
		int event_base, error_base;
		if (XineramaQueryExtension(itsXDisplay, &event_base, &error_base) &&
			XineramaIsActive(itsXDisplay))
		{
			int count;
			XineramaScreenInfo* info = XineramaQueryScreens(itsXDisplay, &count);

			itsBounds = jnew JArray<JRect>(count);
			assert( itsBounds != nullptr );

			for (int i=0; i<count; i++)
			{
				JRect r(info[i].y_org,
						info[i].x_org,
						info[i].y_org + info[i].height,
						info[i].x_org + info[i].width);

				if (info[i].x_org == 0 && info[i].y_org == 0)
				{
					itsBounds->PrependItem(r);
				}
				else
				{
					itsBounds->AppendItem(r);
				}
			}

			XFree(info);
		}
	}

	#endif

	if (itsBounds == nullptr)
	{
		int x,y;
		unsigned int width, height, borderWidth, depth;
		Window rootWindow;
		const Status ok =
			XGetGeometry(itsXDisplay, GetRootWindow(), &rootWindow, &x, &y,
						 &width, &height, &borderWidth, &depth);
		assert( ok );

		itsBounds = jnew JArray<JRect>(1);
		assert( itsBounds != nullptr );
		itsBounds->AppendItem(JRect(y, x, y+height, x+width));
	}

	// The location of the mouse is a good guess as to the location of the
	// user's attention.  Treat the screen that contains this location as
	// the root "window"

	for (const auto& r : *itsBounds)
	{
		if (r.Contains(itsLatestMouseLocation))
		{
			return r;
		}
	}

	return itsBounds->GetFirstItem();
}

/******************************************************************************
 Placing windows on active screen

 ******************************************************************************/

void
JXDisplay::ShrinkDisplayBoundsToActiveScreen()
{
	itsShrinkDisplayToScreenRefCount++;
	jdelete itsBounds;
	itsBounds = nullptr;
}

void
JXDisplay::RestoreDisplayBounds()
{
	assert( itsShrinkDisplayToScreenRefCount > 0 );

	itsShrinkDisplayToScreenRefCount--;
	if (itsShrinkDisplayToScreenRefCount == 0)
	{
		jdelete itsBounds;
		itsBounds = nullptr;
	}
}

/******************************************************************************
 GetCurrentButtonKeyState

	Call these if you need to get information more current than the latest
	event.  This is useful if you need the state of the Shift key because
	pressing Shift doesn't generate an event.

	buttonStates and/or modifiers can be nullptr.

 ******************************************************************************/

void
JXDisplay::GetCurrentButtonKeyState
	(
	JXButtonStates* buttonStates,
	JXKeyModifiers* modifiers
	)
	const
{
	Window root, child;
	int root_x, root_y, win_x, win_y;
	unsigned int state;
	XQueryPointer(itsXDisplay, GetRootWindow(), &root, &child, &root_x, &root_y,
				  &win_x, &win_y, &state);

	if (buttonStates != nullptr)
	{
		buttonStates->SetState(state);
	}

	if (modifiers != nullptr)
	{
		modifiers->SetState(this, state);
	}
}

/******************************************************************************
 UpdateModifierMapping (private)

 ******************************************************************************/

// same order as JXModifierKey enum in JXKeyModifiers.h

static const KeySym kModifierKeySymList[] =
{
	XK_Num_Lock,    0,
	XK_Scroll_Lock, 0,
	XK_Meta_L,      XK_Meta_R,
	XK_Alt_L,       XK_Alt_R,
	XK_Super_L,     XK_Super_R,
	XK_Hyper_L,     XK_Hyper_R
};

const JSize kModifierKeySymCount = sizeof(kModifierKeySymList)/(2*sizeof(KeySym));

void
JXDisplay::UpdateModifierMapping()
{
JIndex i;

	if (itsModifierKeymap != nullptr)
	{
		XFreeModifiermap(itsModifierKeymap);
	}
	itsModifierKeymap = XGetModifierMapping(itsXDisplay);

	itsJXKeyModifierMapping[0] = 0;		// might as well initialize

	for (i=1; i<=kXModifierCount; i++)
	{
		itsJXKeyModifierMapping[i] = i;
	}

	assert( kModifierKeySymCount == kJXKeyModifierMapCount-kXModifierCount );

	for (i=1; i<=kModifierKeySymCount; i++)
	{
		JIndex j;
		if (!KeysymToModifier(kModifierKeySymList[2*i-2], &j) &&
			(kModifierKeySymList[2*i-1] == 0 ||
			 !KeysymToModifier(kModifierKeySymList[2*i-1], &j)))
		{
			j = 0;
		}
		itsJXKeyModifierMapping [ kXModifierCount+i ] = j;

//		std::cout << "modifier " << kXModifierCount+i << " mapped to " << j << std::endl;
	}
//	std::cout << std::endl;

	// some Linux distributions (Mandrake) map only Alt modifier, not Meta

	if (itsJXKeyModifierMapping[kJXMetaKeyIndex] == 0)
	{
		itsJXKeyModifierMapping [ kJXMetaKeyIndex ] =
			itsJXKeyModifierMapping [ kJXAltKeyIndex ];
	}
}

/******************************************************************************
 KeysymToModifier

	Converts an X keysym into an X modifier index.

 ******************************************************************************/

bool
JXDisplay::KeysymToModifier
	(
	const KeySym	keysym,
	JIndex*			modifierIndex
	)
	const
{
	return KeycodeToModifier(XKeysymToKeycode(itsXDisplay, keysym), modifierIndex);
}

/******************************************************************************
 KeycodeToModifier

	Converts an X keycode into an X modifier index.

 ******************************************************************************/

bool
JXDisplay::KeycodeToModifier
	(
	const KeyCode	keycode,
	JIndex*			modifierIndex
	)
	const
{
	*modifierIndex = 0;
	if (keycode == 0)
	{
		return false;
	}

	const JSize maxKeyPerMod = itsModifierKeymap->max_keypermod;
	for (JUnsignedOffset i=0; i<kXModifierCount; i++)
	{
		const KeyCode* modifier =
			itsModifierKeymap->modifiermap + i * maxKeyPerMod;

		for (JUnsignedOffset j=0; j<maxKeyPerMod; j++)
		{
			if (modifier[j] == keycode)
			{
				*modifierIndex = i+1;
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 CreateBuiltInCursor

 ******************************************************************************/

JCursorIndex
JXDisplay::CreateBuiltInCursor
	(
	const JUtf8Byte*	name,
	const unsigned int	shape
	)
{
	JCursorIndex index;
	if (GetCursor(name, &index))
	{
		return index;
	}

	CursorInfo info;

	info.name = jnew JString(name);
	assert( info.name != nullptr );

	info.xid = XCreateFontCursor(itsXDisplay, shape);

	itsCursorList->AppendItem(info);
	return itsCursorList->GetItemCount();
}

/******************************************************************************
 CreateCustomCursor

 ******************************************************************************/

JCursorIndex
JXDisplay::CreateCustomCursor
	(
	const JUtf8Byte*	name,
	const JXCursor&		cursor
	)
{
	JCursorIndex index;
	if (GetCursor(name, &index))
	{
		return index;
	}

	CursorInfo info;

	info.name = jnew JString(name);
	assert( info.name != nullptr );

	info.xid = CreateCustomXCursor(cursor);

	itsCursorList->AppendItem(info);
	return itsCursorList->GetItemCount();
}

/******************************************************************************
 CreateCustomXCursor (private)

 ******************************************************************************/

Cursor
JXDisplay::CreateCustomXCursor
	(
	const JXCursor& cursor
	)
	const
{
	XColor foreColor;
	foreColor.pixel = itsColorManager->GetXColor(JColorManager::GetBlackColor());
	foreColor.red = foreColor.green = foreColor.blue = 0;
	foreColor.flags = DoRed | DoGreen | DoBlue;

	XColor backColor;
	backColor.pixel = itsColorManager->GetXColor(JColorManager::GetWhiteColor());
	backColor.red = backColor.green = backColor.blue = 65535;
	backColor.flags = DoRed | DoGreen | DoBlue;

	Pixmap sourcePixmap =
		XCreateBitmapFromData(itsXDisplay, GetRootWindow(),
							  cursor.data, cursor.w, cursor.h);
	assert( sourcePixmap != 0 );

	Pixmap maskPixmap =
		XCreateBitmapFromData(itsXDisplay, GetRootWindow(),
							  cursor.mask, cursor.w, cursor.h);
	assert( maskPixmap != 0 );

	const Cursor cursorID =
		XCreatePixmapCursor(itsXDisplay, sourcePixmap, maskPixmap,
							&foreColor, &backColor, cursor.hotx, cursor.hoty);

	XFreePixmap(itsXDisplay, sourcePixmap);
	XFreePixmap(itsXDisplay, maskPixmap);

	return cursorID;
}

/******************************************************************************
 GetCursor

	Returns true if a cursor with the given name has been created.

 ******************************************************************************/

bool
JXDisplay::GetCursor
	(
	const JUtf8Byte*	name,
	JCursorIndex*		index
	)
	const
{
	const JSize count = itsCursorList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const CursorInfo info = itsCursorList->GetItem(i);
		if (*info.name == name)
		{
			*index = i;
			return true;
		}
	}

	return false;
}

/******************************************************************************
 DisplayCursorInAllWindows

	Displays the specified cursor in all windows on this display.
	Mainly useful for displaying the watch cursor or the inactive cursor.

 ******************************************************************************/

void
JXDisplay::DisplayCursorInAllWindows
	(
	const JCursorIndex index
	)
{
	for (const auto& info : *itsWindowList)
	{
		info.window->DisplayXCursor(index);
	}

	// Since this is usually called before a blocking process,
	// we need to make sure that the X server gets the messages.

	Flush();
}

/******************************************************************************
 HandleEvent

	JXSelectionManager handles all the selection events.
	All other events go to the appropriate window.

 ******************************************************************************/

void
JXDisplay::HandleEvent
	(
	const XEvent&	xEvent,
	const Time		currentTime
	)
{
	// update event time (mainly for JXSelectionManager::BecomeOwner)

	Time time;
	if (JXGetEventTime(xEvent, &time))
	{
		itsLastEventTime = time;
	}

	if (xEvent.type == MotionNotify)
	{
		itsLastMotionNotifyTime = currentTime;
	}
//	else if (xEvent.type == Expose)
//		{
//		itsLastIdleTime = 0;
//		}

	// save button and key modifier states

	unsigned int state;
	if (JXGetButtonAndModifierStates(xEvent, this, &state))
	{
		itsLatestButtonStates.SetState(state);
		itsLatestKeyModifiers.SetState(this, state);
	}

	JPoint mousePt;
	if (JXGetMouseLocation(xEvent, this, &mousePt))
	{
		itsLatestMouseLocation = mousePt;
	}

	// handle event

	if (xEvent.type == MappingNotify)
	{
		if (xEvent.xmapping.request == MappingModifier)
		{
			UpdateModifierMapping();
		}
		else if (xEvent.xmapping.request == MappingKeyboard)
		{
			XRefreshKeyboardMapping(const_cast<XMappingEvent*>(&(xEvent.xmapping)));
		}
	}

	else if (xEvent.type == SelectionRequest)
	{
		itsSelectionManager->HandleSelectionRequest(xEvent.xselectionrequest);
	}
	else if (xEvent.type == SelectionClear)
	{
		itsSelectionManager->ClearData(xEvent.xselectionclear.selection,
									   xEvent.xselectionclear.time);
	}
	else if (xEvent.type == SelectionNotify)
	{
		// JXSelectionManager catches these events
	}

	else if (xEvent.type == ClientMessage &&
			 itsDNDManager->HandleClientMessage(xEvent.xclient))
	{
		// JXDNDManager handled it if it returns true
	}
	else if (xEvent.type == DestroyNotify &&
			 itsDNDManager->HandleDestroyNotify(xEvent.xdestroywindow))
	{
		// JXDNDManager handled it if it returns true
	}

	else if ((xEvent.type == ButtonPress || xEvent.type == ButtonRelease) &&
			 itsMouseGrabber != nullptr)
	{
		XEvent fixedEvent         = xEvent;
		XButtonEvent& buttonEvent = fixedEvent.xbutton;
		if (buttonEvent.window != itsMouseGrabber->GetXWindow())
		{
			const JPoint ptG =
				itsMouseGrabber->RootToGlobal(buttonEvent.x_root, buttonEvent.y_root);
			buttonEvent.window    = itsMouseGrabber->GetXWindow();
			buttonEvent.subwindow = None;
			buttonEvent.x         = ptG.x;
			buttonEvent.y         = ptG.y;
		}
		itsMouseGrabber->HandleEvent(fixedEvent);
	}
	else if (xEvent.type == MotionNotify && itsMouseGrabber != nullptr)
	{
		// calls XQueryPointer() for itsXWindow so we don't care which window
		// it is reported relative to

		itsMouseGrabber->HandleEvent(xEvent);
	}
	else if ((xEvent.type == KeyPress || xEvent.type == KeyRelease) &&
			 itsKeyboardGrabber != nullptr)
	{
		// ignores mouse coordinates so we don't care which window
		// it is reported relative to

		itsKeyboardGrabber->HandleEvent(xEvent);
	}

	else
	{
		JXWindow* window;
		if (FindXWindow(xEvent.xany.window, &window))
		{
			window->HandleEvent(xEvent);
		}
		else
		{
			XEventMessage msg(xEvent);
			BroadcastWithFeedback(&msg);
		}
	}
}

/******************************************************************************
 Idle

	Redraw our windows and send a fake motion event.

	If the mouse is not pressed, we send idle events relatively slowly so
	that we don't hog CPU time.

	When the mouse is pressed, we send one at every opportunity because
	the user presumably doesn't care how much CPU time we use.

 ******************************************************************************/

void
JXDisplay::Idle
	(
	const Time currentTime
	)
{
	if (currentTime - itsLastIdleTime > kMaxSleepTime ||
		!itsLatestButtonStates.AllOff())
	{
		Update();
		itsLastIdleTime = currentTime;

		if (currentTime - itsLastMotionNotifyTime > kMaxSleepTime)
		{
			DispatchMouse();
			itsLastMotionNotifyTime = currentTime;
		}
	}
}

/******************************************************************************
 Update

	Give each window a chance to redraw itself.  Since this routine is
	an idle task, we use a single flag to make sure that it doesn't do
	more work than it has to.

 ******************************************************************************/

void
JXDisplay::Update()
{
	if (itsNeedsUpdateFlag)
	{
		itsNeedsUpdateFlag = false;	// clear first, in case redraw triggers update

		for (const auto& info : *itsWindowList)
		{
			info.window->Update();
		}
	}
}

/******************************************************************************
 DispatchMouse

 ******************************************************************************/

void
JXDisplay::DispatchMouse()
{
	if (itsMouseContainer != nullptr)
	{
		itsMouseContainer->DispatchMouse();
	}
}

/******************************************************************************
 DispatchCursor

 ******************************************************************************/

void
JXDisplay::DispatchCursor()
{
	if (itsMouseContainer != nullptr)
	{
		itsMouseContainer->DispatchCursor();
	}
}

/******************************************************************************
 FindMouseContainer

	Returns true if the mouse is in one of our windows.

	Regardless of the return value, if xWindow is not nullptr, it is set to the
	id of the X window that the cursor is in.  ptG is set to the mouse
	coordinates relative to xWindow, and ptR contains the mouse coordinates
	relative to the root window.

 ******************************************************************************/

bool
JXDisplay::FindMouseContainer
	(
	JXContainer**	obj,
	Window*			xWindow,
	JPoint*			ptG,
	JPoint*			ptR
	)
	const
{
	Window rootWindow, childWindow;
	int root_x, root_y, x,y;
	unsigned int state;
	if (XQueryPointer(itsXDisplay, GetRootWindow(), &rootWindow, &childWindow,
					  &root_x, &root_y, &x, &y, &state) &&
		childWindow != None)
	{
		return FindMouseContainer(rootWindow, root_x, root_y,
								  childWindow, obj, xWindow, ptG, ptR);
	}
	else
	{
		if (xWindow != nullptr)
		{
			*xWindow = rootWindow;
		}
		if (ptG != nullptr)
		{
			ptG->x = x;
			ptG->y = y;
		}
		if (ptR != nullptr)
		{
			ptR->x = root_x;
			ptR->y = root_y;
		}
		*obj = nullptr;
		return false;
	}
}

bool
JXDisplay::FindMouseContainer
	(
	const JXContainer*	coordOwner,
	const JPoint&		pt,
	JXContainer**		obj,
	Window*				xWindow,
	JPoint*				resultPtG,
	JPoint*				resultPtR
	)
	const
{
	const Window startWindow = (coordOwner->GetWindow())->GetXWindow();
	const JPoint ptG         = coordOwner->LocalToGlobal(pt);

	const Window rootWindow = GetRootWindow();
	Window childWindow;
	int x,y;
	if (XTranslateCoordinates(itsXDisplay, startWindow, rootWindow,
							  ptG.x, ptG.y, &x, &y, &childWindow) &&
		childWindow != None)
	{
		return FindMouseContainer(rootWindow, x,y, childWindow,
								  obj, xWindow, resultPtG, resultPtR);
	}
	else
	{
		if (xWindow != nullptr)
		{
			*xWindow = rootWindow;
		}
		if (resultPtG != nullptr)
		{
			resultPtG->x = x;
			resultPtG->y = y;
		}
		if (resultPtR != nullptr)
		{
			resultPtR->x = x;
			resultPtR->y = y;
		}
		*obj = nullptr;
		return false;
	}
}

// private

bool
JXDisplay::FindMouseContainer
	(
	const Window		rootWindow,
	const JCoordinate	xRoot,
	const JCoordinate	yRoot,
	const Window		origChildWindow,
	JXContainer**		obj,
	Window*				xWindow,
	JPoint*				ptG,
	JPoint*				ptR
	)
	const
{
	Window window1 = rootWindow;
	Window window2 = origChildWindow;
	Window childWindow;
	int x1 = xRoot, y1 = yRoot, x2,y2;
	while (XTranslateCoordinates(itsXDisplay, window1, window2,
								 x1, y1, &x2, &y2, &childWindow) &&
		   childWindow != None)
	{
		window1 = window2;
		window2 = childWindow;
		x1      = x2;
		y1      = y2;
	}

	if (xWindow != nullptr)
	{
		*xWindow = window2;
	}
	if (ptG != nullptr)
	{
		ptG->x = x2;
		ptG->y = y2;
	}
	if (ptR != nullptr)
	{
		ptR->x = xRoot;
		ptR->y = yRoot;
	}

	JXWindow* window;
	if (FindXWindow(window2, &window))
	{
		return window->FindContainer(JPoint(x2,y2), obj);
	}
	else
	{
		*obj = nullptr;
		return false;
	}
}

/******************************************************************************
 SwitchDrag

	Switch the current mouse drag messages from one widget to another.

 ******************************************************************************/

bool
JXDisplay::SwitchDrag
	(
	JXContainer*			fromObj,
	const JPoint&			fromObjPt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JXContainer*			toObj
	)
{
	JXWindow* fromWindow = fromObj->GetWindow();
	const JPoint fromPtG = fromObj->LocalToGlobal(fromObjPt);

	fromWindow->EndDrag(fromObj, fromPtG, buttonStates, modifiers);

	JXWindow* toWindow = toObj->GetWindow();
	JPoint toPtG;
	if (toWindow != fromWindow)
	{
		const JPoint ptR = fromWindow->GlobalToRoot(fromPtG);
		toPtG = toWindow->RootToGlobal(ptR);
	}
	else
	{
		toPtG = fromPtG;
	}

	return toWindow->BeginDrag(toObj, toPtG, buttonStates, modifiers);
}

/******************************************************************************
 WindowCreated

	Insert the new window so that the list remains sorted by xWindow values.

 ******************************************************************************/

void
JXDisplay::WindowCreated
	(
	JXWindow*		window,
	const Window	xWindow
	)
{
	const WindowInfo newInfo(window, xWindow);
	JIndex i;
	if (itsWindowList->InsertSorted(newInfo, false, &i) && i == 1)
	{
		window->AcceptSaveYourself(true);
		if (itsWindowList->GetItemCount() > 1)
		{
			itsWindowList->GetItem(2).window->AcceptSaveYourself(false);
		}
	}
}

/******************************************************************************
 WindowDeleted

 ******************************************************************************/

void
JXDisplay::WindowDeleted
	(
	JXWindow* window
	)
{
	if (itsMouseContainer == window)
	{
		itsMouseContainer = nullptr;
	}
	if (itsMouseGrabber == window)
	{
		itsMouseGrabber = nullptr;
	}
	if (itsKeyboardGrabber == window)
	{
		itsKeyboardGrabber = nullptr;
	}

	const JSize count = itsWindowList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const WindowInfo info = itsWindowList->GetItem(i);
		if (info.window == window)
		{
			itsWindowList->RemoveItem(i);
			if (i == 1 && count > 1)
			{
				((itsWindowList->GetItem(1)).window)->AcceptSaveYourself(true);
			}
			break;
		}
	}
}

/******************************************************************************
 FindXWindow

	Returns true if the given xWindow is registered.
	Since the window list is sorted by xWindow, we use an O(log(N)) binary search.

	We can't call it FindWindow() because Windows #define's FindWindow to
	FindWindowA.

 ******************************************************************************/

bool
JXDisplay::FindXWindow
	(
	const Window	xWindow,
	JXWindow**		window
	)
	const
{
	WindowInfo target(nullptr, xWindow);
	JIndex i;
	if (itsWindowList->SearchSorted(target, JListT::kAnyMatch, &i))
	{
		target  = itsWindowList->GetItem(i);
		*window = target.window;
		return true;
	}
	else
	{
		*window = nullptr;
		return false;
	}
}

/******************************************************************************
 CompareXWindows (static private)

 ******************************************************************************/

std::weak_ordering
JXDisplay::CompareXWindows
	(
	const WindowInfo& info1,
	const WindowInfo& info2
	)
{
	return info1.xWindow <=> info2.xWindow;
}

/******************************************************************************
 XErrorHandler (static)

	Errors reported by the X Server are translated into XError messages
	and BroadcastWithFeedback() by the appropriate JXDisplay to anybody who
	cares to listen.  If somebody catches the error and deals with it, they
	*must* call SetCaught() to notify us.  If nobody calls SetCaught(), we
	abort, causing a core dump.

	The messages are broadcast as an urgent task to avoid reentrancy problems.

	The one exception is BadWindow errors for windows that we do not
	own.  These errors must be ignored to avoid horrendous amounts of
	error checking in JXSelectionManager and JXDNDManager.

	*** X restricts the actions that can be taken while handling an error:
		You cannot contact the X Server.

 ******************************************************************************/

static JArray<XErrorEvent> theXErrorList;

int
JXDisplay::JXErrorHandler
	(
	Display*		xDisplay,
	XErrorEvent*	error
	)
{
	theXErrorList.AppendItem(*error);
	return Success;
}

void
JXDisplay::CheckForXErrors()
{
	if (theXErrorList.IsEmpty())
	{
		return;
	}

	// we need to get count every time since extra XErrors could be generated

	for (const auto& error : theXErrorList)
	{
		JXDisplay* display;
		const bool found = JXGetApplication()->FindDisplay(error.display, &display);
		assert( found );

		XError msg(error);
		display->BroadcastWithFeedback(&msg);
		if (!msg.WasCaught())
		{
			JXWindow* w = nullptr;
			if (msg.GetType() == BadWindow && msg.GetXID() != None &&
				!display->FindXWindow(msg.GetXID(), &w))
			{
				// not our window -- probably residual from selection or DND
				continue;
			}
			else if (msg.GetType() == BadMatch &&
					 error.request_code == X_SetInputFocus)
			{
				// Never die on anything as silly as an XSetInputFocus() error,
				// but still useful to know if it happens.
//				std::cerr << "Illegal call to XSetInputFocus()" << std::endl;
				continue;
			}

			std::cerr << "An unexpected XError occurred!" << std::endl;

			char str[80];
			XGetErrorText(error.display, error.error_code, str, 80);
			std::cerr << "Error code: " << str << std::endl;

			JString reqCodeStr(error.request_code);
			XGetErrorDatabaseText(error.display, "XRequest", reqCodeStr.GetBytes(), "unknown", str, 80);
			std::cerr << "Offending request: " << str << std::endl;

			assert_msg( 0, "unexpected XError" );
		}
	}

	theXErrorList.RemoveAll();
}

/******************************************************************************
 JXDebugAfterFunction (static)

	This generates a core dump immediately when an X error ocurrs, instead
	of at some unspecified time in the future.

 ******************************************************************************/

static int
JXDebugAfterFunction
	(
	Display* xDisplay
	)
{
	XSync(xDisplay, False);
	JXDisplay::CheckForXErrors();
	return True;
}

/******************************************************************************
 WMBehavior::Load

 ******************************************************************************/

bool
JXDisplay::WMBehavior::Load
	(
	JXDisplay* display
	)
{
	bool success = false;

	const Atom atom = display->itsStandardXAtoms[ kJXWMBehaviorV0XAtomIndex ];

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* data = nullptr;
	XGetWindowProperty(*display, display->GetRootWindow(), atom,
					   0, LONG_MAX, False, atom,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &data);

	if (actualType == atom && actualFormat == 8 && itemCount > 0)
	{
		const std::string s(reinterpret_cast<char*>(data), itemCount);
		std::istringstream input(s);
		success = Read(input, 0);
	}

	XFree(data);
	return success;
}

/******************************************************************************
 WMBehavior::Save

 ******************************************************************************/

void
JXDisplay::WMBehavior::Save
	(
	JXDisplay* display
	)
	const
{
	// version 0

	std::ostringstream dataV0;
	WriteV0(dataV0);
	const std::string s = dataV0.str();

	const Atom atom = display->itsStandardXAtoms[ kJXWMBehaviorV0XAtomIndex ];
	XChangeProperty(*display, display->GetRootWindow(),
					atom, atom, 8, PropModeReplace,
					(unsigned char*) s.c_str(), s.length());
}

/******************************************************************************
 WMBehavior::Read (private)

 ******************************************************************************/

bool
JXDisplay::WMBehavior::Read
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	if (vers > 0)
	{
		return false;
	}

	input >> JBoolFromString(desktopMapsWindowsFlag)
		  >> JBoolFromString(frameCompensateFlag);
	input >> reshowOffset;

	return true;
}

/******************************************************************************
 WMBehavior::WriteV0 (private)

 ******************************************************************************/

void
JXDisplay::WMBehavior::WriteV0
	(
	std::ostream& output
	)
	const
{
	output << JBoolToString(desktopMapsWindowsFlag)
		   << JBoolToString(frameCompensateFlag);
	output << ' ' << reshowOffset;
}
