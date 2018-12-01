/******************************************************************************
 JXWindow.cpp

	If we ever allow a constructor that takes a JXDisplay*, we have to
	rewrite JXGetCurrFontMgr.

	BASE CLASS = JXContainer

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXWindow.h"
#include "JXWindowDirector.h"
#include "JXMenuManager.h"
#include "JXDNDManager.h"
#include "JXHintManager.h"
#include "JXRaiseWindowTask.h"
#include "JXExpandWindowToFitContentTask.h"
#include "JXTextMenu.h"
#include "JXDisplay.h"
#include "JXGC.h"
#include "JXColorManager.h"
#include "JXWindowPainter.h"
#include "JXIconDirector.h"
#include "JXWindowIcon.h"
#include "JXImageMask.h"
#include "JXDockManager.h"
#include "JXDockDirector.h"
#include "JXDockWidget.h"
#include "JXDockWindowTask.h"
#include "jXUtil.h"
#include "jXGlobals.h"

#include <X11/Xatom.h>

#include <JThisProcess.h>
#include <JStringIterator.h>
#include <jASCIIConstants.h>
#include <jStreamUtil.h>
#include <jTime.h>
#include <jAssert.h>

static JBoolean theAnalyzeWMFlag = kJFalse;
const JCoordinate kWMFrameSlop   = 2;		// pixels
const JFloat kSyncExtraDelay     = 0.3;		// seconds

JBoolean JXWindow::theAutoDockNewWindowFlag        = kJTrue;
JBoolean JXWindow::theFocusFollowsCursorInDockFlag = kJFalse;

// JXSelectionManager needs PropertyNotify

const unsigned int kEventMask =
	FocusChangeMask | KeyPressMask | KeyReleaseMask |
	ButtonPressMask | ButtonReleaseMask |
	PointerMotionMask | PointerMotionHintMask |
	EnterWindowMask | LeaveWindowMask |
	ExposureMask | StructureNotifyMask | SubstructureNotifyMask |
	PropertyChangeMask;

const unsigned int kPointerGrabMask =
	ButtonPressMask | ButtonReleaseMask |
	PointerMotionMask | PointerMotionHintMask |
	PropertyChangeMask;

const JFileVersion kCurrentGeometryDataVersion = 2;
const JUtf8Byte kGeometryDataEndDelimiter      = '\1';

	// version  2 saves existence of docks
	// version  1 saves dock ID

// JBroadcaster message types

const JUtf8Byte* JXWindow::kIconified      = "Iconified::JXWindow";
const JUtf8Byte* JXWindow::kDeiconified    = "Deiconified::JXWindow";
const JUtf8Byte* JXWindow::kMapped         = "Mapped::JXWindow";
const JUtf8Byte* JXWindow::kUnmapped       = "Unmapped::JXWindow";
const JUtf8Byte* JXWindow::kRaised         = "Raised::JXWindow";
const JUtf8Byte* JXWindow::kDocked         = "Docked::JXWindow";
const JUtf8Byte* JXWindow::kUndocked       = "Undocked::JXWindow";
const JUtf8Byte* JXWindow::kMinSizeChanged = "MinSizeChanged::JXWindow";
const JUtf8Byte* JXWindow::kTitleChanged   = "TitleChanged::JXWindow";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWindow::JXWindow
	(
	JXWindowDirector*	director,
	const JCoordinate	w,
	const JCoordinate	h,
	const JString&		title,
	const JBoolean		isOverlay
	)
	:
	JXContainer((JXGetApplication())->GetCurrentDisplay(), this, nullptr),
	itsDirector(director),
	itsIconDir(nullptr),
	itsMainWindow(nullptr),
	itsIsOverlayFlag(isOverlay),
	itsBufferPixmap(None),
	itsTitle(title),
	itsIcon(nullptr),
	itsBounds(0, 0, h, w),
	itsHasBeenVisibleFlag(kJFalse),
	itsUpdateRegion(XCreateRegion()),
	itsIsMappedFlag(kJFalse),
	itsIsIconifiedFlag(kJFalse),
	itsHasFocusFlag(kJFalse),
	itsFocusWhenShowFlag(kJFalse),
	itsBufferDrawingFlag(isOverlay),
	itsKeepBufferPixmapFlag(isOverlay),
	itsUseBkgdPixmapFlag(isOverlay),
	itsIsDestructingFlag(kJFalse),
	itsHasMinSizeFlag(kJFalse),
	itsHasMaxSizeFlag(kJFalse),
	itsCloseAction(kCloseDirector),
	itsCursorIndex(kJXDefaultCursor),
	itsMouseContainer(nullptr),
	itsIsDraggingFlag(kJFalse),
	itsProcessDragFlag(kJFalse),
	itsCursorLeftFlag(kJFalse),
	itsCleanAfterBlockFlag(kJFalse),
	itsButtonPressReceiver(this),
	itsPointerGrabbedFlag(kJFalse),
	itsBPRChangedFlag(kJFalse),
	itsFocusWidget(nullptr),
	itsCurrHintMgr(nullptr),
	itsRootChild(None),
	itsPrevMouseContainer(nullptr),
	itsFirstClick(kJXNoButton, 0, JPoint(-1,-1)),
	itsSecondClick(kJXNoButton, 0, JPoint(-1,-1)),
	itsClickCount(1),
	itsIsDockedFlag(kJFalse),
	itsDockXWindow(None),
	itsDockWidget(nullptr),
	itsDockingTask(nullptr),
	itsChildWindowList(nullptr),
	itsExpandTask(nullptr)
{
	assert( director != nullptr );

	AdjustTitle();

	itsShortcuts = jnew JArray<Shortcut>;
	assert( itsShortcuts != nullptr );
	itsShortcuts->SetCompareFunction(CompareShortcuts);

	itsFocusList = jnew JPtrArray<JXWidget>(JPtrArrayT::kForgetAll);
	assert( itsFocusList != nullptr );

	// get display/colormap for this window

	itsDisplay   = (JXGetApplication())->GetCurrentDisplay();
	itsBackColor = JColorManager::GetDefaultBackColor();

	// create window

	const unsigned long valueMask =
		CWBackPixel | CWBorderPixel | CWColormap | CWCursor |
		CWSaveUnder | CWOverrideRedirect | CWEventMask;

	JXColorManager* colorManager = itsDisplay->GetColorManager();

	XSetWindowAttributes attr;
	attr.background_pixel  = itsBackColor;
	attr.border_pixel      = colorManager->GetXColor(JColorManager::GetBlackColor());
	attr.colormap          = *colorManager;
	attr.cursor            = itsDisplay->GetXCursorID(itsCursorIndex);
	attr.save_under        = itsIsOverlayFlag;
	attr.override_redirect = itsIsOverlayFlag;
	attr.event_mask        = kEventMask;

	itsXWindow =
		XCreateWindow(*itsDisplay, itsDisplay->GetRootWindow(), 0,0, w,h,
					  0, CopyFromParent, InputOutput, colorManager->GetVisual(),
					  valueMask, &attr);

	// set window properties

	const JUtf8Byte* t = itsTitle.GetBytes();

	XTextProperty windowName;
	const int ok = XStringListToTextProperty((char**) &t, 1, &windowName);
	assert( ok );

	XSizeHints sizeHints;
	sizeHints.flags  = PSize;
	sizeHints.width  = w;
	sizeHints.height = h;

	XWMHints wmHints;
	wmHints.flags         = StateHint | InputHint;
	wmHints.initial_state = NormalState;
	wmHints.input         = True;

	XSetWMProperties(*itsDisplay, itsXWindow, &windowName, &windowName,
					 nullptr, 0, &sizeHints, &wmHints, nullptr);

	XFree(windowName.value);

	// tell window manager about our process

	const JUInt32 pid = (JThisProcess::Instance())->GetPID();
	XChangeProperty(*itsDisplay, itsXWindow, itsDisplay->GetWMPidXAtom(),
					XA_CARDINAL, 32, PropModeReplace, (unsigned char*) &pid, 1);

	// create JXDNDAware property for Drag-And-Drop

	GetDNDManager()->EnableDND(itsXWindow);

	// trap window manager's delete message

	AcceptSaveYourself(kJFalse);

	// tell window manager what kind of window we are

	if (isOverlay)
		{
		SetWMWindowType(kWMPulldownMenuType);
		}

	// create GC to use when drawing

	itsGC = jnew JXGC(itsDisplay, itsXWindow);
	assert( itsGC != nullptr );

	// Create IC to use when typing

	itsXIC = XCreateIC(itsDisplay->GetXIM(),
					   XNInputStyle,   XIMPreeditNothing | XIMStatusNothing,
					   XNClientWindow, itsXWindow,
					   XNFocusWindow,  itsXWindow,
					   nullptr);

	long icEventMask;
	XGetICValues(itsXIC, XNFilterEvents, &icEventMask, nullptr);
	if ((kEventMask | icEventMask) != kEventMask)
		{
		XSelectInput(*itsDisplay, itsXWindow, kEventMask | icEventMask);
		}

	// notify the display that we exist

	itsDisplay->WindowCreated(this, itsXWindow);
	itsDirector->SetWindow(this);

	// expand window after creator's BuildWindow() finishes

	if (!isOverlay)
		{
		itsExpandTask = jnew JXExpandWindowToFitContentTask(this);
		assert( itsExpandTask != nullptr );
		itsExpandTask->Go();

		const JXKeyModifiers& mod = itsDisplay->GetLatestKeyModifiers();
		if (mod.meta() && mod.control() && mod.hyper())
			{
			JXContainer::DebugExpandToFitContent(!mod.shift());
			}
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWindow::~JXWindow()
{
	itsIsDestructingFlag = kJTrue;

	DeleteEnclosedObjects();	// widgets talk to us when deleted

	itsDisplay->WindowDeleted(this);

	if (itsIsDockedFlag && itsDockWidget != nullptr)
		{
		(itsDockWidget->GetDockDirector())->ClearFocusWindow(this);
		}
	jdelete itsDockingTask;

	jdelete itsIcon;
	// itsIconDir deleted by itsDirector
	jdelete itsShortcuts;
	jdelete itsFocusList;
	jdelete itsChildWindowList;

	if (itsBufferPixmap != None)
		{
		XFreePixmap(*itsDisplay, itsBufferPixmap);
		}

	XDestroyIC(itsXIC);

	jdelete itsGC;
	XDestroyRegion(itsUpdateRegion);
	if (itsXWindow != None)				// plug-in: might be destroyed by Netscape
		{
		XDestroyWindow(*itsDisplay, itsXWindow);
		}
	itsDisplay->Flush();
}

/******************************************************************************
 SetTitle

 ******************************************************************************/

void
JXWindow::SetTitle
	(
	const JString& title
	)
{
	itsTitle = title;
	AdjustTitle();

	const JUtf8Byte* t = itsTitle.GetBytes();

	XTextProperty windowName;
	const int ok = Xutf8TextListToTextProperty(*itsDisplay, (char**) &t, 1, XTextStyle, &windowName);
	assert( ok == Success );

	XSetWMName(*itsDisplay, itsXWindow, &windowName);
	XSetWMIconName(*itsDisplay, itsXWindow, &windowName);

	XFree(windowName.value);

	Broadcast(TitleChanged());
}

/******************************************************************************
 AdjustTitle (private)

 ******************************************************************************/

void
JXWindow::AdjustTitle()
{
	// avoid loony feature of fvwm

	if (itsTitle.EndsWith("lock"))
		{
		itsTitle.Append(" ");
		}
}

/******************************************************************************
 SetTransientFor

	Provide information to the window manager about which window we
	are subsidiary to.

 ******************************************************************************/

void
JXWindow::SetTransientFor
	(
	const JXWindowDirector* director
	)
{
	XSetTransientForHint(*itsDisplay, itsXWindow,
						 (director->GetWindow())->itsXWindow);
}

/******************************************************************************
 SetWMClass

	Provide information to the window manager that can be used to identify
	the window's type.  This is useful when excluding certain types of
	windows from a task bar.

	Call this before SetIcon() to avoid losing the icon's mask.

 ******************************************************************************/

void
JXWindow::SetWMClass
	(
	const JUtf8Byte* c_class,
	const JUtf8Byte* instance
	)
{
	XClassHint hint = { const_cast<char*>(instance), const_cast<char*>(c_class) };
	XSetClassHint(*itsDisplay, itsXWindow, &hint);

	itsWindowType = instance;

	JXDockManager* mgr;
	JXDockWidget* dock;
	if (theAutoDockNewWindowFlag &&
		JXGetDockManager(&mgr) && mgr->GetDefaultDock(instance, &dock))
		{
		dock->Dock(this);
		}
}

/******************************************************************************
 HideFromTaskbar

	Exclude this window from a task bar.

	Call this before SetIcon() to avoid losing the icon's mask.

 ******************************************************************************/

void
JXWindow::HideFromTaskbar()
{
	SetWMClass("Miscellaneous", JXGetInvisibleWindowClass());
}

/******************************************************************************
 DispatchMouse

 ******************************************************************************/

void
JXWindow::DispatchMouse()
{
	Window rootWindow, childWindow;
	int root_x, root_y, x,y;
	unsigned int state;
	JXWindow* window;
	if (IsVisible() &&
		XQueryPointer(*itsDisplay, itsXWindow, &rootWindow, &childWindow,
					  &root_x, &root_y, &x, &y, &state) &&
		itsDisplay->GetMouseContainer(&window) &&
		window == this)
		{
		XMotionEvent xEvent;
		xEvent.type      = MotionNotify;
		xEvent.display   = *itsDisplay;
		xEvent.window    = itsXWindow;
		xEvent.root      = rootWindow;
		xEvent.subwindow = None;
		xEvent.x         = x;
		xEvent.y         = y;
		xEvent.x_root    = root_x;
		xEvent.y_root    = root_y;
		xEvent.state     = state;

		HandleMotionNotify(xEvent);
		}
}

/******************************************************************************
 DispatchCursor

 ******************************************************************************/

void
JXWindow::DispatchCursor()
{
	Window rootWindow, childWindow;
	int root_x, root_y, x,y;
	unsigned int state;
	JXWindow* window;
	if (IsVisible() &&
		XQueryPointer(*itsDisplay, itsXWindow, &rootWindow, &childWindow,
					  &root_x, &root_y, &x, &y, &state) &&
		itsDisplay->GetMouseContainer(&window) &&
		window == this)
		{
		itsButtonPressReceiver->
			DispatchCursor(JPoint(x,y), JXKeyModifiers(itsDisplay, state));
		}
}

/******************************************************************************
 DisplayXCursor

 ******************************************************************************/

void
JXWindow::DisplayXCursor
	(
	const JCursorIndex index
	)
{
	if (itsCursorIndex != index)
		{
		itsCursorIndex = index;
		XDefineCursor(*itsDisplay, itsXWindow, itsDisplay->GetXCursorID(index));
		}
}

/******************************************************************************
 Close

 ******************************************************************************/

JBoolean
JXWindow::Close()
{
	if (itsCloseAction == kDeactivateDirector)
		{
		return (itsIsDockedFlag ? kJTrue : itsDirector->Deactivate());
		}
	else if (itsCloseAction == kCloseDirector)
		{
		return itsDirector->Close();
		}
	else if (itsCloseAction == kCloseDisplay)
		{
		return itsDisplay->Close();
		}
	else
		{
		assert( itsCloseAction == kQuitApp );
		(JXGetApplication())->Quit();
		return kJTrue;
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXWindow::Activate()
{
	if (!WouldBeActive())
		{
		JXContainer::Activate();
		if (itsIsDockedFlag)
			{
			Raise();
			}
		if (itsFocusWidget == nullptr)
			{
			SwitchFocus(kJFalse);
			}
		}
}

/******************************************************************************
 Resume (virtual)

 ******************************************************************************/

void
JXWindow::Resume()
{
	JXContainer::Resume();
	if (IsActive() && itsFocusWidget == nullptr)
		{
		SwitchFocus(kJFalse);
		}
}

/******************************************************************************
 Show (virtual)

 ******************************************************************************/

void
JXWindow::Show()
{
	if (itsExpandTask != nullptr)
		{
		itsExpandTask->ShowAfterFTC();
		return;
		}

	if (!IsVisible() && (itsDockingTask == nullptr || itsDockingTask->IsDone()))
		{
		JXContainer::Show();
		if (itsFocusWidget == nullptr)
			{
			SwitchFocus(kJFalse);
			}
		if (itsUseBkgdPixmapFlag)
			{
			Redraw();
			}

		const JXDisplay::WMBehavior& b = itsDisplay->GetWMBehavior();
		if (!theAnalyzeWMFlag && itsHasBeenVisibleFlag &&
			!itsIsIconifiedFlag && !itsIsDockedFlag &&
			(b.reshowOffset.x != 0 || b.reshowOffset.y != 0))
			{
			Move(- b.reshowOffset.x, - b.reshowOffset.y);
			}
		itsHasBeenVisibleFlag = kJTrue;

		XMapWindow(*itsDisplay, itsXWindow);
		// input focus set by HandleMapNotify()

		JXWindow* parent;
		if (GetDockWindow(&parent))
			{
			parent->Show();
			}
		}
}

/******************************************************************************
 Hide (virtual)

	PrivateHide() is called during docking, after itsIsDockedFlag is set.

 ******************************************************************************/

void
JXWindow::Hide()
{
	if (IsVisible() && !itsIsDockedFlag)
		{
		// We have to deiconify the window first because otherwise,
		// the icon won't disappear, at least under fvwm and fvwm2.

		if (itsIsIconifiedFlag)
			{
			Deiconify();

			// toss the MapNotify event -- avoids error from XSetInputFocus()

			XEvent xEvent;
			XIfEvent(*itsDisplay, &xEvent, GetNextMapNotifyEvent, nullptr);
			itsIsIconifiedFlag = kJFalse;
			Broadcast(Deiconified());
			}

		PrivateHide();
		}
}

// private

void
JXWindow::PrivateHide()
{
	if (IsVisible())
		{
		XUnmapWindow(*itsDisplay, itsXWindow);
		itsIsMappedFlag = kJFalse;
		JXContainer::Hide();
		}
}

// static private

Bool
JXWindow::GetNextMapNotifyEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	return (event->type == MapNotify ? True : False);
}

/******************************************************************************
 Raise

	Make this the top window.

 ******************************************************************************/

void
JXWindow::Raise
	(
	const JBoolean grabKeyboardFocus
	)
{
	const JBoolean wasVisible = IsVisible();
	const JBoolean wasIcon    = itsIsIconifiedFlag;

	Show();
	Activate();		// in case it wasn't already

	if (itsIsIconifiedFlag)
		{
		Deiconify();
		}
	else if (grabKeyboardFocus)
		{
		RequestFocus();
		}

	if (!itsIsDockedFlag &&
		(!(wasVisible && !itsIsMappedFlag) || wasIcon))
		{
		// Don't call Place() if window director is being activated
		// a second time before XMapWindow event arrives.

		// By calling Place() we ensure that the window is visible on
		// the current virtual desktop.

		Move(0,0);
		}

	// move window to current desktop
	// http://standards.freedesktop.org/wm-spec/latest/ar01s05.html

	Window root = itsDisplay->GetRootWindow();

	const JXDisplay::WMBehavior& b = itsDisplay->GetWMBehavior();
	if (b.desktopMapsWindowsFlag || itsIsMappedFlag)
		{
		Atom actualType;
		int actualFormat;
		unsigned long itemCount, remainingBytes;
		unsigned char* xdata;
		XGetWindowProperty(*itsDisplay, root, itsDisplay->GetWMCurrentDesktopXAtom(),
						   0, LONG_MAX, False, XA_CARDINAL,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &xdata);
		if (actualType == XA_CARDINAL && actualFormat == 32 && itemCount > 0)
			{
			const JUInt32 desktop = *reinterpret_cast<JUInt32*>(xdata);

			XEvent e;
			bzero(&e, sizeof(e));
			e.type                 = ClientMessage;
			e.xclient.display      = *itsDisplay;
			e.xclient.window       = itsXWindow;
			e.xclient.message_type = itsDisplay->GetWMDesktopXAtom();
			e.xclient.format       = 32;
			e.xclient.data.l[0]    = desktop;
			e.xclient.data.l[1]    = 1;	// normal app
			itsDisplay->SendXEvent(root, &e, SubstructureNotifyMask|SubstructureRedirectMask);
			}

		XFree(xdata);
		}

	// force window to be raised
	// http://standards.freedesktop.org/wm-spec/wm-spec-latest.html

	Time t = itsDisplay->GetLastEventTime();
	XChangeProperty(*itsDisplay, itsXWindow, itsDisplay->GetWMUserTimeXAtom(),
					XA_CARDINAL, 32, PropModeReplace, (unsigned char*) &t, 1);

	if (grabKeyboardFocus)
		{
		XEvent e;
		bzero(&e, sizeof(e));
		e.type                 = ClientMessage;
		e.xclient.display      = *itsDisplay;
		e.xclient.window       = itsXWindow;
		e.xclient.message_type = itsDisplay->GetWMActiveWindowXAtom();
		e.xclient.format       = 32;
		e.xclient.data.l[0]    = 1;	// normal app
		e.xclient.data.l[1]    = t;
		itsDisplay->SendXEvent(root, &e, SubstructureNotifyMask|SubstructureRedirectMask);
		}

	XRaiseWindow(*itsDisplay, itsXWindow);

	Broadcast(Raised());
}

/******************************************************************************
 Lower

	Make this the bottom window.

 ******************************************************************************/

void
JXWindow::Lower()
{
	XLowerWindow(*itsDisplay, itsXWindow);
}

/******************************************************************************
 RequestFocus

	Ask X for keyboard input focus.  This uses XSetInputFocus() rather
	than XGrabKeyboard().

 ******************************************************************************/

void
JXWindow::RequestFocus()
{
	if (itsIsMappedFlag)
		{
		XSetInputFocus(*itsDisplay, itsXWindow, RevertToPointerRoot, CurrentTime);
		}
}

/******************************************************************************
 Iconify

 ******************************************************************************/

void
JXWindow::Iconify()
{
	if (itsIsDockedFlag)
		{
		return;
		}

	// change the initial_state hint

	SetWindowStateHint(IconicState);

	// take other necessary actions

	if (IsVisible())
		{
		// We don't modify itsIsIconifiedFlag here because the procedure might
		// fail.  We wait for an UnmapNotify event.

		XIconifyWindow(*itsDisplay, itsXWindow, itsDisplay->GetScreen());
		}
	else if (!itsIsIconifiedFlag)
		{
		itsIsIconifiedFlag = kJTrue;
		Broadcast(Iconified());
		}
}

/******************************************************************************
 Deiconify

 ******************************************************************************/

void
JXWindow::Deiconify()
{
	// change the initial_state hint

	SetWindowStateHint(NormalState);

	// take other necessary actions

	if (IsVisible())
		{
		// We don't modify itsIsIconifiedFlag here because we already deal
		// with it in HandleMapNotify().

		XMapWindow(*itsDisplay, itsXWindow);
		}
	else if (itsIsIconifiedFlag)
		{
		itsIsIconifiedFlag = kJFalse;
		Broadcast(Deiconified());
		}
}

/******************************************************************************
 SetWindowStateHint (private)

	Sets the initial_state hint.

 ******************************************************************************/

void
JXWindow::SetWindowStateHint
	(
	const int initial_state
	)
{
	XWMHints wmHints;

	XWMHints* origHints = XGetWMHints(*itsDisplay, itsXWindow);
	if (origHints != nullptr)
		{
		wmHints = *origHints;
		XFree(origHints);
		}
	else
		{
		wmHints.flags = 0;
		}

	wmHints.flags        |= StateHint;
	wmHints.initial_state = initial_state;
	XSetWMHints(*itsDisplay, itsXWindow, &wmHints);
}

/******************************************************************************
 Refresh (virtual)

 ******************************************************************************/

void
JXWindow::Refresh()
	const
{
	RefreshRect(itsBounds);
}

/******************************************************************************
 RefreshRect

 ******************************************************************************/

void
JXWindow::RefreshRect
	(
	const JRect& rect
	)
	const
{
	XRectangle xrect = JXJToXRect(rect);
	XUnionRectWithRegion(&xrect, itsUpdateRegion, itsUpdateRegion);
	itsDisplay->WindowNeedsUpdate(const_cast<JXWindow*>(this));
}

/******************************************************************************
 Redraw (virtual)

 ******************************************************************************/

void
JXWindow::Redraw()
	const
{
	Refresh();
	(const_cast<JXWindow*>(this))->Update();
}

/******************************************************************************
 RedrawRect

 ******************************************************************************/

void
JXWindow::RedrawRect
	(
	const JRect& rect
	)
	const
{
	RefreshRect(rect);
	(const_cast<JXWindow*>(this))->Update();
}

/******************************************************************************
 BufferDrawing

 ******************************************************************************/

void
JXWindow::BufferDrawing
	(
	const JBoolean bufferDrawing
	)
{
	itsBufferDrawingFlag = JI2B(bufferDrawing || itsUseBkgdPixmapFlag);
	if (!itsBufferDrawingFlag && itsBufferPixmap != None)
		{
		XFreePixmap(*itsDisplay, itsBufferPixmap);
		itsBufferPixmap = None;
		}
}

/******************************************************************************
 Update

	It is a bad idea to keep itsBufferPixmap between updates because this
	wastes an enormous amount of server memory.  It is only done under
	special circumstances that warrant optimization.

	jafl 11/12/97:
		Creating the JXWindowPainter each time we are called is easier
		than keeping it because we don't have to worry about what
		Reset() doesn't clear and because the cost is negligible compared
		with creating the buffer pixmap.

 ******************************************************************************/

void
JXWindow::Update()
{
	if (XEmptyRegion(itsUpdateRegion))
		{
		return;
		}
	else if ((!itsIsMappedFlag || itsIsIconifiedFlag) && !itsUseBkgdPixmapFlag)
		{
		XDestroyRegion(itsUpdateRegion);
		itsUpdateRegion = XCreateRegion();
		return;
		}

	assert( !itsUseBkgdPixmapFlag || itsBufferDrawingFlag );

	// We clear itsUpdateRegion first so widgets call call Refresh() inside Draw()

	Region updateRegion = JXCopyRegion(itsUpdateRegion);
	XDestroyRegion(itsUpdateRegion);
	itsUpdateRegion = XCreateRegion();

	const Drawable drawable = PrepareForUpdate();
	const JRect rect        = JXGetRegionBounds(updateRegion);
	JXWindowPainter p(itsGC, drawable, itsBounds, updateRegion);
	DrawAll(p, rect);

	FinishUpdate(rect, updateRegion);
}

/******************************************************************************
 UpdateForScroll

	Before this function can be used, it must account for areas of the
	window that are obscured by other windows.  In addition, it must also
	deal with issues such as:

		JTextEditor caret is visible, but it doesn't move like the text.

		Enclosed widgets.  The input field in a table may trigger scrolling,
		and then it doesn't move like the cells.  Even when it does,
		updating needs to be smoother so it doesn't look so ugly.

 ******************************************************************************/

void
JXWindow::UpdateForScroll
	(
	const JRect& ap,
	const JRect& src,
	const JRect& dest
	)
{
	assert( src.width() == dest.width() && src.height() == dest.height() );
	assert( ap.Contains(src) && ap.Contains(dest) );

	if ((!itsIsMappedFlag || itsIsIconifiedFlag) && !itsUseBkgdPixmapFlag)
		{
		return;
		}
	Update();	// scrollbars, etc.

	assert( !itsUseBkgdPixmapFlag || itsBufferDrawingFlag );

	// move the pixels that remain visible

	const Drawable scrollD = (itsUseBkgdPixmapFlag && itsBufferPixmap != None) ?
							 itsBufferPixmap : itsXWindow;
	itsGC->SetClipRect(ap);
	itsGC->CopyPixels(scrollD, src.left, src.top, src.width(), src.height(),
					  scrollD, dest.left, dest.top);

	// redraw the strips that have moved into view

	const Drawable drawable = PrepareForUpdate();
	Region updateRegion     = XCreateRegion();

	JRect r = ap;
	if (dest.top > ap.top)
		{
		r.bottom = dest.top;
		JXWindowPainter p(itsGC, drawable, r, nullptr);
		DrawAll(p, r);
		}
	else if (dest.bottom < ap.bottom)
		{
		r.top = dest.bottom;
		JXWindowPainter p(itsGC, drawable, r, nullptr);
		DrawAll(p, r);
		}
	if (r.height() != ap.height())
		{
		XRectangle xrect = JXJToXRect(r);
		XUnionRectWithRegion(&xrect, updateRegion, updateRegion);
		}

	r        = ap;
	r.top    = dest.top;
	r.bottom = dest.bottom;
	if (dest.left > ap.left)
		{
		r.right = dest.left;
		JXWindowPainter p(itsGC, drawable, r, nullptr);
		DrawAll(p, r);
		}
	else if (dest.right < ap.right)
		{
		r.left = dest.right;
		JXWindowPainter p(itsGC, drawable, r, nullptr);
		DrawAll(p, r);
		}
	if (r.width() != ap.width())
		{
		XRectangle xrect = JXJToXRect(r);
		XUnionRectWithRegion(&xrect, updateRegion, updateRegion);
		}

	FinishUpdate(JXGetRegionBounds(updateRegion), updateRegion);
}

/******************************************************************************
 PrepareForUpdate (private)

 ******************************************************************************/

Drawable
JXWindow::PrepareForUpdate()
{
	Drawable drawable   = itsXWindow;
	const JCoordinate w = itsBounds.width();
	const JCoordinate h = itsBounds.height();

	if (itsBufferDrawingFlag && itsBufferPixmap == None)
		{
		itsBufferPixmap = XCreatePixmap(*itsDisplay, itsXWindow, w,h,
										itsDisplay->GetDepth());
		if (itsBufferPixmap != None)
			{
			drawable = itsBufferPixmap;
			}
		}
	else if (itsBufferDrawingFlag)
		{
		drawable = itsBufferPixmap;
		}

	return drawable;
}

/******************************************************************************
 FinishUpdate (private)

	region is deleted.

 ******************************************************************************/

void
JXWindow::FinishUpdate
	(
	const JRect&	rect,
	Region			region
	)
{
	if (itsUseBkgdPixmapFlag && itsBufferPixmap != None)
		{
		XSetWindowBackgroundPixmap(*itsDisplay, itsXWindow, itsBufferPixmap);
		XClearWindow(*itsDisplay, itsXWindow);
		}
	else if (itsUseBkgdPixmapFlag)
		{
		XSetWindowBackground(*itsDisplay, itsXWindow, itsBackColor);
		}
	else if (itsBufferPixmap != None)
		{
		itsGC->SetClipRegion(region);
		itsGC->CopyPixels(itsBufferPixmap,
						  rect.left, rect.top, rect.width(), rect.height(),
						  itsXWindow, rect.left, rect.top);
		}

	// Under normal conditions, we have to toss the pixmap because
	// it uses an enormous amount of server memory.

	if (!itsKeepBufferPixmapFlag && itsBufferPixmap != None &&
		!(itsIsDraggingFlag && itsProcessDragFlag))
		{
		XFreePixmap(*itsDisplay, itsBufferPixmap);
		itsBufferPixmap = None;
		}

	XDestroyRegion(region);

	itsDisplay->Flush();
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXWindow::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXWindow::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 DrawBackground (virtual protected)

 ******************************************************************************/

void
JXWindow::DrawBackground
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.SetPenColor(itsBackColor);
	p.SetFilling(kJTrue);
	p.JPainter::Rect(itsBounds);
}

/******************************************************************************
 GlobalToLocal (virtual)

 ******************************************************************************/

JPoint
JXWindow::GlobalToLocal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x,y);
}

/******************************************************************************
 LocalToGlobal (virtual)

 ******************************************************************************/

JPoint
JXWindow::LocalToGlobal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x,y);
}

/******************************************************************************
 GlobalToRoot

 ******************************************************************************/

JRect
JXWindow::GlobalToRoot
	(
	const JRect& r
	)
	const
{
	const JPoint topLeft  = GlobalToRoot(r.left, r.top);
	const JPoint botRight = GlobalToRoot(r.right, r.bottom);
	return JRect(topLeft.y, topLeft.x, botRight.y, botRight.x);
}

/******************************************************************************
 RootToGlobal

 ******************************************************************************/

JRect
JXWindow::RootToGlobal
	(
	const JRect& r
	)
	const
{
	const JPoint topLeft  = RootToGlobal(r.left, r.top);
	const JPoint botRight = RootToGlobal(r.right, r.bottom);
	return JRect(topLeft.y, topLeft.x, botRight.y, botRight.x);
}

/******************************************************************************
 AnalyzeWindowManager (static)

	Figure out how to compensate for quirky window manager behavior.

	After we call XSync(), we wait a bit longer because the window manager
	gets the placement request, and XSync() doesn't wait for a reply from
	the WM, only from the X server.  We don't have to wait very long,
	however, since XSync() already took care of the network delay.

	This is a member of JXWindow because it needs access to private data.

 ******************************************************************************/

inline void
JXWindow::WaitForWM
	(
	JXDisplay*	d,
	JXWindow*	w
	)
{
	const time_t start = time(nullptr);
	while (XPending(*d) > 0)
		{
		(JXGetApplication())->HandleOneEventForWindow(w);
		if (time(nullptr) - start > 2)	// sometimes we get events for other windows
			{
			break;
			}
		}
}

inline void
Sync
	(
	JXDisplay* d
	)
{
	d->Synchronize();		// make sure server has processed all requests
	JWait(kSyncExtraDelay);	// give window manager some extra time to catch up (not very long, since it runs locally on server)
}

void
JXWindow::AnalyzeWindowManager
	(
	JXDisplay* d
	)
{
	const JCoordinate p = 100;

	JXDisplay::WMBehavior behavior;
	theAnalyzeWMFlag = kJTrue;

	// init wm virtual desktop style
	// Modern window managers map/unmap windows when switching desktops.
	// This completely destroys the usefulness of itsFocusWhenShowFlag.
	{
	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* xdata;
	XGetWindowProperty(*d, d->GetRootWindow(), d->GetWMCurrentDesktopXAtom(),
					   0, LONG_MAX, False, XA_CARDINAL,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &xdata);

	behavior.desktopMapsWindowsFlag =
		JI2B(actualType == XA_CARDINAL && actualFormat == 32 && itemCount > 0);

	XFree(xdata);
	}
	d->SetWMBehavior(behavior);

	// block off everything until we get some answers about window placement

	JXApplication* app = JXGetApplication();
	app->DisplayInactiveCursor();
	app->PrepareForBlockingWindow();

	// create a test window

	JXWindowDirector* dir = jnew JXWindowDirector(JXGetApplication());
	assert( dir != nullptr );

	JXWindow* w = jnew JXWindow(dir, 100, 100, JString("Testing Window Manager", kJFalse));
	assert( w != nullptr );
	jdelete w->itsExpandTask;
	w->itsExpandTask = nullptr;

	// test placing visible window (fvwm2)

	w->Show();
	Sync(d);
	w->Place(p, p);

	for (JIndex i=1; i<=2; i++)
		{
		Sync(d);

		WaitForWM(d, w);
		assert( w->itsIsMappedFlag );

		if (JLAbs(w->itsWMFrameLoc.x - p) > kWMFrameSlop ||
			JLAbs(w->itsWMFrameLoc.y - p) > kWMFrameSlop)
			{
			behavior.frameCompensateFlag = !behavior.frameCompensateFlag;
			d->SetWMBehavior(behavior);
			w->Place(p, p);
			}
		else
			{
			break;
			}
		}

	// test hiding and then showing (XQuartz)

	const JPoint pt1 = w->GetDesktopLocation();

	w->Hide();
	Sync(d);
	w->Show();
	Sync(d);

	WaitForWM(d, w);
	assert( w->itsIsMappedFlag );

	const JPoint pt2 = w->GetDesktopLocation();

	if (pt2 != pt1)
		{
		behavior.reshowOffset = pt2 - pt1;
		d->SetWMBehavior(behavior);
		}

	// done

	dir->Close();
	app->BlockingWindowFinished();

	theAnalyzeWMFlag = kJFalse;
}

/******************************************************************************
 CalcDesktopLocation (private)

	Shift the given point to cancel the window manager border.

	direction should be +1 or -1.
		+1 => WMFrame    -> itsXWindow
		-1 => itsXWindow -> WMFrame

	If anything fails, the window manager must be having convulsions
	(e.g. restarting).  In this case, we will get ConfigureNotify after
	things calm down, so we simply return the input values.

	Some window managers are smart enough to automatically compensate for
	the window border when we call XMoveWindow().  We like this behavior,
	so we assume it by default by setting compensate=kJFalse.  It won't
	always work, however, so we check the result in Place(), and if it
	didn't work, we change our behavior and try again.  Once we find a
	method that works, we set method=kJTrue so we ignore further failures
	as network latency problems.

 ******************************************************************************/

JPoint
JXWindow::CalcDesktopLocation
	(
	const JCoordinate origX,
	const JCoordinate origY,
	const JCoordinate origDirection
	)
	const
{
	assert( !itsIsDockedFlag );

	Window rootChild;
	if (!GetRootChild(&rootChild))
		{
		return JPoint(origX, origY);
		}

	JCoordinate desktopX = origX;
	JCoordinate desktopY = origY;

	int x,y;
	Window childWindow;
	if (XTranslateCoordinates(*itsDisplay, itsXWindow, rootChild,
							  0,0, &x,&y, &childWindow))
		{
		const JCoordinate direction = JSign(origDirection);
		desktopX += x * direction;
		desktopY += y * direction;
		}

	return JPoint(desktopX, desktopY);
}

/******************************************************************************
 GetRootChild (private)

	If possible, returns the X window that is our ancestor and a child
	of the root window.

 ******************************************************************************/

JBoolean
JXWindow::GetRootChild
	(
	Window* rootChild
	)
	const
{
	if (itsRootChild != None)
		{
		*rootChild = itsRootChild;
		return JI2B(itsRootChild != itsXWindow);
		}

	*rootChild = itsRootChild = None;

	Window currWindow = itsXWindow;
	while (1)
		{
		Window rootWindow, parentWindow;
		Window* childList;
		unsigned int childCount;
		if (!XQueryTree(*itsDisplay, currWindow, &rootWindow,
						&parentWindow, &childList, &childCount))
			{
			return kJFalse;
			}
		XFree(childList);

		if (parentWindow == rootWindow)
			{
			*rootChild = itsRootChild = currWindow;
			return JI2B(currWindow != itsXWindow);
			}
		else
			{
			currWindow = parentWindow;
			}
		}
}

/******************************************************************************
 Place (virtual)

	We can't short-circuit if the size won't change because there might be
	geometry events waiting in the queue.

 ******************************************************************************/

void
JXWindow::Place
	(
	const JCoordinate enclX,
	const JCoordinate enclY
	)
{
	if (itsIsDockedFlag)
		{
		itsUndockedWMFrameLoc.Set(enclX, enclY);
		}
	else
		{
		UndockedPlace(enclX, enclY);
		}
}

void
JXWindow::UndockedPlace
	(
	const JCoordinate origEnclX,
	const JCoordinate origEnclY
	)
{
	JCoordinate enclX = origEnclX;
	JCoordinate enclY = origEnclY;

	const JRect desktopBounds      = itsDisplay->GetBounds();
	const JXDisplay::WMBehavior& b = itsDisplay->GetWMBehavior();

	JPoint pt(enclX, enclY);
	if (!itsIsDockedFlag)
		{
		// adjust position so at least part of window is visible
		// (important for virtual desktops)

		while (enclX + itsBounds.width() <= desktopBounds.left)
			{
			enclX += desktopBounds.width();
			}
		while (enclX >= desktopBounds.right)
			{
			enclX -= desktopBounds.width();
			}

		while (enclY + itsBounds.height() <= desktopBounds.top)
			{
			enclY += desktopBounds.height();
			}
		while (enclY >= desktopBounds.bottom)
			{
			enclY -= desktopBounds.height();
			}

		// compensate for width of window manager frame

		if (b.frameCompensateFlag)
			{
			pt = CalcDesktopLocation(enclX, enclY, +1);
			}
		else
			{
			pt = JPoint(enclX, enclY);
			}
		}

	// tell the window manager to move us

	XMoveWindow(*itsDisplay, itsXWindow, pt.x, pt.y);

	if (!itsIsDockedFlag)
		{
		long supplied;
		XSizeHints sizeHints;
		if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
			{
			sizeHints.flags = 0;
			}

		sizeHints.flags |= PPosition;
		sizeHints.x      = pt.x;
		sizeHints.y      = pt.y;

		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		}

	// OverrideRedirect means the Window Manager doesn't interfere.  For
	// all others, Window Manager moves the window, and we don't know how
	// long that will take.  CalcDesktopLocation() doesn't care where the
	// window actually is.  It only cares about the position relative to
	// the Window Manager frame.

	if (itsIsOverlayFlag || itsIsDockedFlag)
		{
		UpdateFrame();
		}
	else if (b.frameCompensateFlag)
		{
		// same as UpdateFrame()

		itsDesktopLoc = pt;
		itsWMFrameLoc = CalcDesktopLocation(pt.x, pt.y, -1);
		}
	else
		{
		// same as UpdateFrame()

		itsDesktopLoc = CalcDesktopLocation(pt.x, pt.y, +1);
		itsWMFrameLoc = pt;
		}
}

/******************************************************************************
 Move (virtual)

 ******************************************************************************/

void
JXWindow::Move
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	Place(itsWMFrameLoc.x + dx, itsWMFrameLoc.y + dy);
}

void
JXWindow::UndockedMove
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	UndockedPlace(itsWMFrameLoc.x + dx, itsWMFrameLoc.y + dy);
}

/******************************************************************************
 SetSize (virtual)

	We can't short-circuit if the size won't change because there might be
	geometry events waiting in the queue.

 ******************************************************************************/

void
JXWindow::SetSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	if (itsIsDockedFlag)
		{
		itsUndockedGeom.SetSize(w, h);
		}
	else
		{
		UndockedSetSize(w, h);
		}
}

void
JXWindow::UndockedSetSize
	(
	const JCoordinate	origW,
	const JCoordinate	origH,
	const JBoolean		ftc
	)
{
	JCoordinate w = origW;
	JCoordinate h = origH;
	if (itsHasMinSizeFlag)
		{
		w = JMax(w, itsMinSize.x);
		h = JMax(h, itsMinSize.y);
		}
	if (itsHasMaxSizeFlag)
		{
		w = JMin(w, itsMaxSize.x);
		h = JMin(h, itsMaxSize.y);
		}

	XResizeWindow(*itsDisplay, itsXWindow, w, h);

	if (!itsIsDockedFlag)
		{
		long supplied;
		XSizeHints sizeHints;
		if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
			{
			sizeHints.flags = 0;
			}

		sizeHints.flags  |= PSize;
		sizeHints.width   = w;
		sizeHints.height  = h;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		}

	UpdateBounds(w, h, ftc);
}

/******************************************************************************
 AdjustSize (virtual)

 ******************************************************************************/

void
JXWindow::AdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	SetSize(itsBounds.width() + dw, itsBounds.height() + dh);
}

/******************************************************************************
 FTCAdjustSize (virtual protected)

	Adjust our size without affecting enclosed widgets, because they have
	already been resized.  To avoid size drift when storing prefs, save the
	delta so we can subtract it before saving our geometry.

 ******************************************************************************/

void
JXWindow::FTCAdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (itsHasMinSizeFlag || itsHasMaxSizeFlag)
		{
		long supplied;
		XSizeHints sizeHints;
		if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
			{
			sizeHints.flags = 0;
			}

		if (itsHasMinSizeFlag)
			{
			itsMinSize.x += dw;
			itsMinSize.y += dh;

			sizeHints.min_width  = itsMinSize.x;
			sizeHints.min_height = itsMinSize.y;
			}

		if (itsHasMaxSizeFlag)
			{
			itsMaxSize.x += dw;
			itsMaxSize.y += dh;

			sizeHints.max_width  = itsMaxSize.x;
			sizeHints.max_height = itsMaxSize.y;
			}

		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		itsDisplay->Flush();
		}

	// adjust layout to fit original size

	const JCoordinate origW = itsBounds.width(),
					  origH = itsBounds.height();

	JCoordinate w = origW,
				h = origH;

	if (itsHasMinSizeFlag)
		{
		w = JMax(w, itsMinSize.x);
		h = JMax(h, itsMinSize.y);
		}

	if (itsHasMaxSizeFlag)
		{
		w = JMin(w, itsMaxSize.x);
		h = JMin(h, itsMaxSize.y);
		}

	if (theDebugFTCFlag)
		{
		GetFTCLog() << "Resizing window contents by dw=" << w - (origW + dw)
					<< " dh=" << h - (origH + dh) << std::endl;
		}
	NotifyBoundsResized(w - (origW + dw), h - (origH + dh));

	itsFTCDelta.x += w - origW;		// save difference created by min/max
	itsFTCDelta.y += h - origH;

	if (itsIsDockedFlag)
		{
		itsUndockedGeom.SetSize(w, h);
		}
	else
		{
		UndockedSetSize(w, h, kJTrue);
		}
}

/******************************************************************************
 UpdateFrame (private)

	We can't pass in the new frame information because ConfigureNotify
	caused by resize window gives x=0, y=0.

 ******************************************************************************/

void
JXWindow::UpdateFrame()
{
	Window rootWindow;
	int x,y;
	unsigned int w,h, bw, depth;
	const Status ok1 = XGetGeometry(*itsDisplay, itsXWindow, &rootWindow,
									&x, &y, &w, &h, &bw, &depth);
	assert( ok1 );

	const int origX = x, origY = y;

	Window childWindow;
	const Bool ok2 = XTranslateCoordinates(*itsDisplay, itsXWindow, rootWindow,
										   0,0, &x, &y, &childWindow);
	assert( ok2 );

	itsDesktopLoc.Set(x,y);		// also at end of Place()
	itsWMFrameLoc =
		itsIsDockedFlag ? JPoint(origX, origY) : CalcDesktopLocation(x,y, -1);
	if (itsIsMappedFlag &&
		!(itsHasMinSizeFlag && (w < itsMinSize.x || h < itsMinSize.y)))
		{
		UpdateBounds(w, h, kJFalse);
		}
}

/******************************************************************************
 UpdateBounds (private)

 ******************************************************************************/

void
JXWindow::UpdateBounds
	(
	const JCoordinate	w,
	const JCoordinate	h,
	const JBoolean		ftc
	)
{
	const JCoordinate dw = w - itsBounds.width();
	const JCoordinate dh = h - itsBounds.height();
	itsBounds.top    = 0;
	itsBounds.left   = 0;
	itsBounds.bottom = h;
	itsBounds.right  = w;

	if (dw != 0 || dh != 0)
		{
		if (!ftc)
			{
			NotifyBoundsResized(dw,dh);
			}

		if (itsBufferPixmap != None)
			{
			XFreePixmap(*itsDisplay, itsBufferPixmap);
			itsBufferPixmap = None;
			}
		}
}

/******************************************************************************
 CenterOnScreen

 ******************************************************************************/

void
JXWindow::CenterOnScreen()
{
	if (!itsIsDockedFlag)
		{
		itsDisplay->ShrinkDisplayBoundsToActiveScreen();

		const JRect r = itsDisplay->GetBounds();
		Place((r.left + r.right - itsBounds.width())/2,
			  (r.top + r.bottom - itsBounds.height())/2);

		itsDisplay->RestoreDisplayBounds();
		}
}

/******************************************************************************
 PlaceAsDialogWindow

	Following Macintosh Human Interface Guidelines, dialog windows
	have 1/3 blank space above and 2/3 blank space below.

 ******************************************************************************/

void
JXWindow::PlaceAsDialogWindow()
{
	if (!itsIsDockedFlag)
		{
		itsDisplay->ShrinkDisplayBoundsToActiveScreen();

		const JRect r = itsDisplay->GetBounds();
		Place((r.left + r.right - itsBounds.width())/2,
			  (r.top + r.bottom - itsBounds.height())/3);

		itsDisplay->RestoreDisplayBounds();
		}
}

/******************************************************************************
 SetMinSize

 ******************************************************************************/

void
JXWindow::SetMinSize
	(
	const JCoordinate origW,
	const JCoordinate origH
	)
{
	long supplied;
	XSizeHints sizeHints;
	if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
		{
		sizeHints.flags = 0;
		}

	JCoordinate w = origW;
	JCoordinate h = origH;
	if ((sizeHints.flags & PMaxSize) != 0)
		{
		w = JMin(w, (JCoordinate) sizeHints.max_width);
		h = JMin(h, (JCoordinate) sizeHints.max_height);
		}

	sizeHints.flags     |= PMinSize;
	sizeHints.min_width  = w;
	sizeHints.min_height = h;
	XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
	itsDisplay->Flush();

	itsHasMinSizeFlag = kJTrue;
	itsMinSize.x      = w;
	itsMinSize.y      = h;

	if (itsBounds.width() < w || itsBounds.height() < h)
		{
		SetSize(JMax(w,itsBounds.width()), JMax(h,itsBounds.height()));
		}

	Broadcast(MinSizeChanged());
}

/******************************************************************************
 ClearMinSize

 ******************************************************************************/

void
JXWindow::ClearMinSize()
{
	long supplied;
	XSizeHints sizeHints;
	if (XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied) &&
		(sizeHints.flags & PMinSize) != 0)
		{
		sizeHints.flags     -= PMinSize;
		sizeHints.min_width  = 0;
		sizeHints.min_height = 0;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		itsDisplay->Flush();
		}

	itsHasMinSizeFlag = kJFalse;
}

/******************************************************************************
 SetMaxSize

 ******************************************************************************/

void
JXWindow::SetMaxSize
	(
	const JCoordinate origW,
	const JCoordinate origH
	)
{
	long supplied;
	XSizeHints sizeHints;
	if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
		{
		sizeHints.flags = 0;
		}

	JCoordinate w = origW;
	JCoordinate h = origH;
	if ((sizeHints.flags & PMinSize) != 0)
		{
		w = JMax(w, (JCoordinate) sizeHints.min_width);
		h = JMax(h, (JCoordinate) sizeHints.min_height);
		}

	sizeHints.flags     |= PMaxSize;
	sizeHints.max_width  = w;
	sizeHints.max_height = h;
	XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
	itsDisplay->Flush();

	itsHasMaxSizeFlag = kJTrue;
	itsMaxSize.x      = w;
	itsMaxSize.y      = h;

	if (itsBounds.width() > w || itsBounds.height() > h)
		{
		SetSize(JMin(w,itsBounds.width()), JMin(h,itsBounds.height()));
		}
}

/******************************************************************************
 ClearMaxSize

 ******************************************************************************/

void
JXWindow::ClearMaxSize()
{
	long supplied;
	XSizeHints sizeHints;
	if (XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied) &&
		(sizeHints.flags & PMaxSize) != 0)
		{
		sizeHints.flags     -= PMaxSize;
		sizeHints.max_width  = INT_MAX;
		sizeHints.max_height = INT_MAX;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		itsDisplay->Flush();
		}

	itsHasMaxSizeFlag = kJFalse;
}

/******************************************************************************
 SetStepSize

 ******************************************************************************/

void
JXWindow::SetStepSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	long supplied;
	XSizeHints sizeHints;
	if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
		{
		sizeHints.flags = 0;
		}

	sizeHints.flags     |= PResizeInc;
	sizeHints.width_inc  = dw;
	sizeHints.height_inc = dh;
	XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
	itsDisplay->Flush();
}

/******************************************************************************
 ClearStepSize

 ******************************************************************************/

void
JXWindow::ClearStepSize()
{
	long supplied;
	XSizeHints sizeHints;
	if (XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied) &&
		(sizeHints.flags & PResizeInc) != 0)
		{
		sizeHints.flags     -= PResizeInc;
		sizeHints.width_inc  = 1;
		sizeHints.height_inc = 1;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		itsDisplay->Flush();
		}
}

/******************************************************************************
 SetBackColor

 ******************************************************************************/

void
JXWindow::SetBackColor
	(
	const JColorID color
	)
{
	itsBackColor = color;
	if (!itsUseBkgdPixmapFlag)
		{
		XSetWindowBackground(*itsDisplay, itsXWindow, color);
		}
}

/******************************************************************************
 SetIcon

	Sets the icon to be displayed when the window is iconified.

	Call this after SetWMClass() to avoid losing the icon's mask.

	We take ownership of icon.

	Once you use an icon with a mask, you will not able to clear the mask.
	You must instead specify a square mask.  This is a design flaw in X.

 ******************************************************************************/

void
JXWindow::SetIcon
	(
	JXImage* icon
	)
{
	XWMHints wmHints;
	wmHints.flags = 0;

	// get current hints

	XWMHints* origHints = XGetWMHints(*itsDisplay, itsXWindow);
	if (origHints != nullptr)
		{
		wmHints = *origHints;
		XFree(origHints);
		}

	// set new icon

	if (itsIconDir != nullptr)
		{
		const JBoolean ok = itsIconDir->Close();
		assert( ok );
		itsIconDir = nullptr;
		}

	jdelete itsIcon;
	itsIcon = icon;

	icon->ConvertToPixmap();

	wmHints.flags       |= IconPixmapHint;
	wmHints.icon_pixmap  = icon->GetPixmap();

	JXImageMask* mask;
	if (icon->GetMask(&mask))
		{
		mask->ConvertToPixmap();

		wmHints.flags    |= IconMaskHint;
		wmHints.icon_mask = mask->GetPixmap();
		}

	XSetWMHints(*itsDisplay, itsXWindow, &wmHints);
}

/******************************************************************************
 SetIcon

	Sets the icon to be displayed when the window is iconified.  This
	version creates a window with a JXWindowIcon that you can ListenTo()
	to accept DND.

	Call this after SetWMClass() to avoid losing the icon's mask.

	We take ownership of the icons.

	Once you use this version, you will no longer be able to use the
	version that doesn't use an X window.  This is a design flaw in X.

 ******************************************************************************/

JXWindowIcon*
JXWindow::SetIcon
	(
	JXImage* normalIcon,
	JXImage* dropIcon
	)
{
	XWMHints wmHints;
	wmHints.flags = 0;

	// get current hints

	XWMHints* origHints = XGetWMHints(*itsDisplay, itsXWindow);
	if (origHints != nullptr)
		{
		wmHints = *origHints;
		XFree(origHints);
		}

	// set new icon

	jdelete itsIcon;
	itsIcon = nullptr;

	if (itsIconDir == nullptr)
		{
		itsIconDir = jnew JXIconDirector(itsDirector, normalIcon, dropIcon);
		assert( itsIconDir != nullptr );
		(itsIconDir->GetWindow())->itsMainWindow = this;
		}
	else
		{
		(itsIconDir->GetIconWidget())->SetIcons(normalIcon, dropIcon);
		}

	wmHints.flags       |= IconWindowHint;
	wmHints.icon_window  = (itsIconDir->GetWindow())->GetXWindow();

	XSetWMHints(*itsDisplay, itsXWindow, &wmHints);

	return itsIconDir->GetIconWidget();
}

/******************************************************************************
 GetIconWidget

 ******************************************************************************/

JBoolean
JXWindow::GetIconWidget
	(
	JXWindowIcon** widget
	)
	const
{
	if (itsIconDir != nullptr)
		{
		*widget = itsIconDir->GetIconWidget();
		return kJTrue;
		}
	else
		{
		*widget = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXWindow::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
}

/******************************************************************************
 EnclosingBoundsMoved (virtual protected)

	We will never get this message.

 ******************************************************************************/

void
JXWindow::EnclosingBoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	assert( 0 );
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXWindow::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

	We will never get this message.

 ******************************************************************************/

void
JXWindow::EnclosingBoundsResized
	(
	const JCoordinate dwb,
	const JCoordinate dhb
	)
{
	assert( 0 );
}

/******************************************************************************
 GetBoundsGlobal (virtual)

	Returns the bounds in global coordinates.

 ******************************************************************************/

JRect
JXWindow::GetBoundsGlobal()
	 const
{
	return itsBounds;
}

/******************************************************************************
 GetFrameGlobal (virtual)

	Returns the frame in global coordinates.

 ******************************************************************************/

JRect
JXWindow::GetFrameGlobal()
	 const
{
	return itsBounds;
}

/******************************************************************************
 GetApertureGlobal (virtual)

	Returns the aperture in global coordinates.

 ******************************************************************************/

JRect
JXWindow::GetApertureGlobal()
	 const
{
	return itsBounds;
}

/******************************************************************************
 ReadGeometry

	Read in position, size, and state and adjust ourselves.

	In the version that takes JString&, the string can be empty, in which
	case, nothing changes.  If the string is not empty, it is assumed
	to contain only the output of WriteGeometry().

 ******************************************************************************/

void
JXWindow::ReadGeometry
	(
	const JString&	data,
	const JBoolean	skipDocking
	)
{
	if (!data.IsEmpty())
		{
		const std::string s(data.GetBytes(), data.GetByteCount());
		std::istringstream input(s);
		ReadGeometry(input, skipDocking);
		}
}

void
JXWindow::ReadGeometry
	(
	std::istream&	input,
	const JBoolean	skipDocking
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentGeometryDataVersion)
		{
		JIgnoreUntil(input, kGeometryDataEndDelimiter);
		return;
		}

	JPoint desktopLoc;
	JCoordinate w,h;
	JBoolean iconified;
	input >> desktopLoc >> w >> h >> JBoolFromString(iconified);

	int dockIt         = -1;
	JXDockWidget* dock = nullptr;
	if (!skipDocking && vers >= 1)
		{
		JIndex id;
		input >> id;

		JBoolean hadDocks = kJTrue;
		if (vers >= 2)
			{
			input >> JBoolFromString(hadDocks);
			}

		JXDockManager* mgr;
		if (id != JXDockManager::kInvalidDockID &&
			JXGetDockManager(&mgr) &&
			mgr->FindDock(id, &dock))
			{
			dockIt = (itsIsDockedFlag && dock == itsDockWidget ? -1 : +1);
			}
		else if (id == JXDockManager::kInvalidDockID && hadDocks)
			{
			dockIt = 0;
			}
		}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);

	if (dockIt == 0 || (dockIt == -1 && !itsIsDockedFlag))
		{
		Undock();
		Place(desktopLoc.x, desktopLoc.y);
		SetSize(w,h);

		if (iconified)
			{
			Iconify();
			}
		else
			{
			Deiconify();
			}
		}
	else if (dockIt == 1 && dock != nullptr && dock->WindowWillFit(this))
		{
		jdelete itsDockingTask;	// automatically cleared
		if (dock->Dock(this))
			{
			itsUndockedWMFrameLoc = desktopLoc;
			itsUndockedGeom.Set(0, 0, h, w);
			}
		}
	else if (dockIt == -1 && itsIsDockedFlag)
		{
		itsUndockedWMFrameLoc = desktopLoc;
		itsUndockedGeom.Set(0, 0, h, w);
		}
}

/******************************************************************************
 SkipGeometry (static)

 ******************************************************************************/

void
JXWindow::SkipGeometry
	(
	std::istream& input
	)
{
	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

	Write out our position, size, and state (iconified).

	We have to write out an ending delimiter that is never used anywhere
	else so we can at least ignore the data if we can't read the given
	version.

 ******************************************************************************/

void
JXWindow::WriteGeometry
	(
	JString* data
	)
	const
{
	std::ostringstream output;
	WriteGeometry(output);
	*data = output.str();
}

void
JXWindow::WriteGeometry
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentGeometryDataVersion;
	if (itsIsDockedFlag)
		{
		output << ' ' << itsUndockedWMFrameLoc;
		output << ' ' << itsUndockedGeom.width() - itsFTCDelta.x;
		output << ' ' << itsUndockedGeom.height() - itsFTCDelta.y;
		output << ' ' << JBoolToString(kJFalse);
		}
	else
		{
		JPoint loc = GetDesktopLocation();
		if (itsHasBeenVisibleFlag && !IsVisible())
			{
			const JXDisplay::WMBehavior& b = itsDisplay->GetWMBehavior();
			loc -= b.reshowOffset;
			}

		output << ' ' << loc;
		output << ' ' << GetFrameWidth() - itsFTCDelta.x;
		output << ' ' << GetFrameHeight() - itsFTCDelta.y;
		output << ' ' << JBoolToString(itsIsIconifiedFlag);
		}

	output << ' ' << (itsDockWidget != nullptr ? itsDockWidget->GetID() :
											  JXDockManager::kInvalidDockID);

	JXDockManager* mgr;
	output << ' ' << JBoolToString(JI2B(JXGetDockManager(&mgr) && mgr->HasDocks()));

	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 HandleEvent

 ******************************************************************************/

void
JXWindow::HandleEvent
	(
	const XEvent& xEvent
	)
{
	if (xEvent.type == EnterNotify)
		{
		HandleEnterNotify(xEvent.xcrossing);
		}
	else if (xEvent.type == LeaveNotify)
		{
		HandleLeaveNotify(xEvent.xcrossing);
		}
	else if (xEvent.type == MotionNotify)
		{
		HandleMotionNotify(xEvent.xmotion);
		}
	else if (xEvent.type == ButtonPress)
		{
		HandleButtonPress(xEvent.xbutton);
		}
	else if (xEvent.type == ButtonRelease)
		{
		HandleButtonRelease(xEvent.xbutton);
		}

	else if (xEvent.type == FocusIn)
		{
		HandleFocusIn(xEvent.xfocus);
		}
	else if (xEvent.type == FocusOut)
		{
		HandleFocusOut(xEvent.xfocus);
		}
	else if (xEvent.type == KeyPress)
		{
		HandleKeyPress(xEvent);
		}

	else if (xEvent.type == Expose)
		{
		HandleExpose(xEvent.xexpose);
		}

	else if (xEvent.type == ConfigureNotify && xEvent.xconfigure.window == itsXWindow)
		{
		UpdateFrame();
		UpdateChildWindowGeometry();	// update itsDesktopLoc
		}
	else if (xEvent.type == ConfigureNotify)
		{
		SetChildWindowGeometry(xEvent.xconfigure.window,
			JRect(xEvent.xconfigure.y, xEvent.xconfigure.x,
				  xEvent.xconfigure.y + xEvent.xconfigure.height,
				  xEvent.xconfigure.x + xEvent.xconfigure.width));
		}

	else if (xEvent.type == MapNotify && xEvent.xmap.window == itsXWindow)
		{
		HandleMapNotify(xEvent.xmap);
		}
	else if (xEvent.type == UnmapNotify && xEvent.xunmap.window == itsXWindow)
		{
		HandleUnmapNotify(xEvent.xunmap);
		}
	else if (xEvent.type == MapNotify || xEvent.type == UnmapNotify)
		{
		SetChildWindowVisible(xEvent.xmap.window,
							  JI2B(xEvent.type == MapNotify));
		}
	else if (xEvent.type             == PropertyNotify &&
			 xEvent.xproperty.window == itsXWindow &&
			 xEvent.xproperty.atom   == itsDisplay->GetWMStateXAtom() &&
			 xEvent.xproperty.state  == PropertyNewValue)
		{
		HandleWMStateChange();
		}

	else if (xEvent.type == ReparentNotify && xEvent.xreparent.window == itsXWindow)
		{
		itsRootChild = None;

		// When window is dragged to different compartment in same window
		// with exactly the same size, we don't receive ConfigureNotify!
		UpdateFrame();
		}
	else if (xEvent.type == ReparentNotify)
		{
		UpdateChildWindowList(xEvent.xreparent.window,
							  JI2B(xEvent.xreparent.parent == itsXWindow));
		}

	else if (xEvent.type == DestroyNotify && xEvent.xdestroywindow.window == itsXWindow)
		{
		// we have been deleted -- primarily for Netscape plug-in

		itsXWindow     = None;
		itsFocusWidget = nullptr;	// too late to validate
		itsDirector->Close();		// deletes us
		}
	else if (xEvent.type == DestroyNotify)
		{
		UpdateChildWindowList(xEvent.xdestroywindow.window, kJFalse);
		}

	else if (IsDeleteWindowMessage(itsDisplay, xEvent))		// trap WM_DELETE_WINDOW first
		{
		Close();											// can delete us
		}
	else if (IsWMPingMessage(itsDisplay, xEvent))
		{
		XEvent e;
		memcpy(&e, &xEvent, sizeof(e));
		e.xclient.window = itsDisplay->GetRootWindow();
		XSendEvent(*itsDisplay, e.xclient.window, False,
				   (SubstructureNotifyMask|SubstructureRedirectMask), &e);
		}
	else if (IsSaveYourselfMessage(itsDisplay, xEvent))
		{
		const JString& cmd = (JXGetApplication())->GetRestartCommand();
		char* s            = const_cast<JUtf8Byte*>(cmd.GetBytes());
		XSetCommand(*itsDisplay, itsXWindow, &s, 1);
		}
	else if (xEvent.type == ClientMessage)					// otherwise, send to all widgets
		{
		DispatchClientMessage(xEvent.xclient);
		}
}

/******************************************************************************
 IsDeleteWindowMessage (static)

 ******************************************************************************/

JBoolean
JXWindow::IsDeleteWindowMessage
	(
	const JXDisplay*	display,
	const XEvent&		xEvent
	)
{
	return JI2B(
		xEvent.type                       == ClientMessage &&
		xEvent.xclient.message_type       == display->GetWMProtocolsXAtom() &&
		xEvent.xclient.format             == 32 &&
		((Atom) xEvent.xclient.data.l[0]) == display->GetDeleteWindowXAtom());
}

/******************************************************************************
 IsWMPingMessage (static)

 ******************************************************************************/

JBoolean
JXWindow::IsWMPingMessage
	(
	const JXDisplay*	display,
	const XEvent&		xEvent
	)
{
	return JI2B(
		xEvent.type                       == ClientMessage &&
		xEvent.xclient.message_type       == display->GetWMProtocolsXAtom() &&
		xEvent.xclient.format             == 32 &&
		((Atom) xEvent.xclient.data.l[0]) == display->GetWMPingXAtom());
}

/******************************************************************************
 IsSaveYourselfMessage (static)

 ******************************************************************************/

JBoolean
JXWindow::IsSaveYourselfMessage
	(
	const JXDisplay*	display,
	const XEvent&		xEvent
	)
{
	return JI2B(
		xEvent.type                       == ClientMessage &&
		xEvent.xclient.message_type       == display->GetWMProtocolsXAtom() &&
		xEvent.xclient.format             == 32 &&
		((Atom) xEvent.xclient.data.l[0]) == display->GetSaveYourselfXAtom());
}

/******************************************************************************
 AcceptSaveYourself

 ******************************************************************************/

void
JXWindow::AcceptSaveYourself
	(
	const JBoolean accept
	)
{
	Atom protocolList[3] =
		{
		itsDisplay->GetDeleteWindowXAtom(),		// always accept this one
		itsDisplay->GetWMPingXAtom(),			// always accept this one
		itsDisplay->GetSaveYourselfXAtom()
		};
	XSetWMProtocols(*itsDisplay, itsXWindow, protocolList, (accept ? 3 : 2));
}

/******************************************************************************
 SetWMWindowType

 ******************************************************************************/

void
JXWindow::SetWMWindowType
	(
	const WMType type
	)
{
	Atom atom = itsDisplay->GetWMWindowTypeXAtom(type);
	XChangeProperty(*itsDisplay, itsXWindow, itsDisplay->GetWMWindowTypeXAtom(),
					XA_ATOM, 32, PropModeReplace, (unsigned char*) &atom, 1);
}

/******************************************************************************
 HandleEnterNotify (private)

	If the cursor is grabbed, then this event means that we no longer
	need to process the previous LeaveNotify.

 ******************************************************************************/

void
JXWindow::HandleEnterNotify
	(
	const XCrossingEvent& xEvent
	)
{
	if (JXButtonStates::AllOff(xEvent.state))
		{
		if (itsIsDockedFlag && theFocusFollowsCursorInDockFlag)
			{
			RequestFocus();
			}

		itsDisplay->SetMouseContainer(this);
		itsButtonPressReceiver->
			DispatchNewMouseEvent(EnterNotify, JPoint(xEvent.x, xEvent.y),
								  kJXNoButton, xEvent.state);
		}
	else	// cursor grabbed
		{
		itsCursorLeftFlag = kJFalse;
		}
}

/******************************************************************************
 HandleLeaveNotify (private)

	If the cursor is grabbed, then we don't want to process this event
	until the drag is finished.

 ******************************************************************************/

void
JXWindow::HandleLeaveNotify
	(
	const XCrossingEvent& xEvent
	)
{
	if (JXButtonStates::AllOff(xEvent.state))
		{
		SetMouseContainer(nullptr, JPoint(xEvent.x, xEvent.y), xEvent.state);
		itsDisplay->SetMouseContainer(nullptr);
		}
	else	// cursor grabbed
		{
		itsCursorLeftFlag = kJTrue;
		}
}

/******************************************************************************
 HandleMotionNotify (private)

 ******************************************************************************/

void
JXWindow::HandleMotionNotify
	(
	const XMotionEvent& xEvent
	)
{
	XEvent discardEvent;
	while (XCheckMaskEvent(*itsDisplay, PointerMotionMask, &discardEvent))
		{ };

	// XQueryPointer returns the current mouse state, not the state
	// when the XMotionEvent was generated.  This can cause HandleMouseDrag()
	// before HandleMouseDown().

	Window rootWindow, childWindow;
	int x_root, y_root, x,y;
	unsigned int state;
	if (XQueryPointer(*itsDisplay, itsXWindow, &rootWindow, &childWindow,
					  &x_root, &y_root, &x, &y, &state))
		{
		const JBoolean isDrag = !JXButtonStates::AllOff(xEvent.state);
		if (itsIsDraggingFlag && isDrag &&			// otherwise wait for ButtonPress
			itsProcessDragFlag && itsMouseContainer != nullptr)
			{
			JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
			Display* xDisplay  = *display;
			Window xWindow     = itsXWindow;

			const JPoint pt = itsMouseContainer->GlobalToLocal(x,y);
			itsMouseContainer->DispatchMouseDrag(pt, JXButtonStates(xEvent.state),
												 JXKeyModifiers(itsDisplay, xEvent.state));

			if (JXDisplay::WindowExists(display, xDisplay, xWindow))
				{
				Update();
				}
			}
		else if (!itsIsDraggingFlag && !isDrag && itsButtonPressReceiver->IsVisible())
			{
			itsButtonPressReceiver->
				DispatchNewMouseEvent(MotionNotify, JPoint(x,y),
									  kJXNoButton, xEvent.state);
			}
		// otherwise wait for ButtonRelease
		}
}

/******************************************************************************
 HandleButtonPress (private)

	Returns kJFalse if the window is deleted.

 ******************************************************************************/

JBoolean
JXWindow::HandleButtonPress
	(
	const XButtonEvent& xEvent
	)
{
	RequestFocus();

	if (itsIsMappedFlag && itsIsIconifiedFlag)
		{
		// hack around window managers that don't notify us correctly

		itsIsIconifiedFlag = kJFalse;
		Broadcast(Deiconified());
		Refresh();
		}

	const JXMouseButton currButton = (JXMouseButton) xEvent.button;
	const unsigned int state = JXButtonStates::SetState(xEvent.state, currButton, kJTrue);

	if (!itsIsDraggingFlag)
		{
		itsIsDraggingFlag  = kJTrue;
		itsProcessDragFlag = kJFalse;		// JXContainer tells us if it wants it

		const JPoint ptG(xEvent.x, xEvent.y);

		itsFirstClick         = itsSecondClick;
		itsSecondClick.button = currButton;
		itsSecondClick.time   = xEvent.time;
		// itsSecondClick.pt calculated by JXContainer that accepts click

		JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
		Display* xDisplay  = *display;
		Window xWindow     = itsXWindow;

		itsButtonPressReceiver->
			DispatchNewMouseEvent(ButtonPress, ptG, currButton, state);

		if (!JXDisplay::WindowExists(display, xDisplay, xWindow))
			{
			return kJFalse;
			}

		itsCursorLeftFlag = kJFalse;
		Update();
		}

	else if (itsIsDraggingFlag && itsProcessDragFlag && itsMouseContainer != nullptr)
		{
		const JPoint pt = itsMouseContainer->GlobalToLocal(xEvent.x, xEvent.y);
		itsMouseContainer->MouseDown(pt, currButton, 1,
									 JXButtonStates(state),
									 JXKeyModifiers(itsDisplay, state));
		}

	// If a blocking dialog window was popped up, then we will never get
	// the ButtonRelease event because HandleOneEventForWindow() tossed it.

	if ((JXGetApplication())->HadBlockingWindow())
		{
		itsCleanAfterBlockFlag = kJTrue;
		if (itsIsDraggingFlag && itsProcessDragFlag && itsMouseContainer != nullptr)
			{
			EndDrag(itsMouseContainer, JPoint(xEvent.x, xEvent.y),
					JXButtonStates(state), JXKeyModifiers(itsDisplay, state));
			}
		itsIsDraggingFlag      = kJFalse;
		itsCleanAfterBlockFlag = kJFalse;
		}

	return kJTrue;
}

/******************************************************************************
 CountClicks

	pt must be in the local coordinates of the given container.

 ******************************************************************************/

JSize
JXWindow::CountClicks
	(
	JXContainer*	container,
	const JPoint&	pt
	)
{
	assert( itsMouseContainer != nullptr && container == itsMouseContainer );

	itsSecondClick.pt = pt;

	if (itsPrevMouseContainer == itsMouseContainer &&
		itsFirstClick.button == itsSecondClick.button &&
		itsSecondClick.time - itsFirstClick.time <= kJXDoubleClickTime &&
		itsMouseContainer->HitSamePart(itsFirstClick.pt, itsSecondClick.pt))
		{
		itsClickCount++;
		}
	else
		{
		itsClickCount = 1;
		}

	// set it here so it stays nullptr until after we check it the first time

	itsPrevMouseContainer = itsMouseContainer;

	return itsClickCount;
}

/******************************************************************************
 HandleButtonRelease (private)

	Returns kJFalse if the window is deleted.

 ******************************************************************************/

JBoolean
JXWindow::HandleButtonRelease
	(
	const XButtonEvent& xEvent
	)
{
	if (!itsIsDraggingFlag)
		{
		// We can't use assert() because JXMenuTable gets blasted after
		// the left button is released, while an other button might still
		// be down.  (pretty unlikely, but still possible)

		return kJTrue;
		}

	const JXMouseButton currButton = (JXMouseButton) xEvent.button;
	const unsigned int state = JXButtonStates::SetState(xEvent.state, currButton, kJFalse);

	if (itsProcessDragFlag && itsMouseContainer != nullptr)
		{
		JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
		Display* xDisplay  = *display;
		Window xWindow     = itsXWindow;

		// this could delete us

		const JPoint pt = itsMouseContainer->GlobalToLocal(xEvent.x, xEvent.y);
		itsMouseContainer->DispatchMouseUp(pt, currButton,
										   JXButtonStates(state),
										   JXKeyModifiers(itsDisplay, state));

		if (!JXDisplay::WindowExists(display, xDisplay, xWindow))
			{
			return kJFalse;
			}
		}

	// If drag is finished, release the pointer and process buffered LeaveNotify.

	if (JXButtonStates::AllOff(state))
		{
		itsIsDraggingFlag = kJFalse;
		if (!itsPointerGrabbedFlag)
			{
			// balance XGrabPointer() in BeginDrag()
			XUngrabPointer(*itsDisplay, xEvent.time);
			itsDisplay->SetMouseGrabber(nullptr);
			}
		if (itsCursorLeftFlag)
			{
			SetMouseContainer(nullptr, JPoint(xEvent.x, xEvent.y), state);
			}
		}

	// If a blocking dialog window was popped up, then we will never get
	// other ButtonRelease events because HandleOneEventForWindow() tossed them.

	else if ((JXGetApplication())->HadBlockingWindow() && !itsCleanAfterBlockFlag)
		{
		itsCleanAfterBlockFlag = kJTrue;
		if (itsIsDraggingFlag && itsProcessDragFlag && itsMouseContainer != nullptr)
			{
			EndDrag(itsMouseContainer, JPoint(xEvent.x, xEvent.y),
					JXButtonStates(state), JXKeyModifiers(itsDisplay, state));
			}
		itsIsDraggingFlag      = kJFalse;
		itsCleanAfterBlockFlag = kJFalse;
		}

	return kJTrue;
}

/******************************************************************************
 SetMouseContainer

 ******************************************************************************/

void
JXWindow::SetMouseContainer
	(
	JXContainer*		obj,
	const JPoint&		ptG,
	const unsigned int	state
	)
{
	if (obj != nullptr)
		{
		itsDisplay->SetMouseContainer(this);	// may not get EnterNotify
		}

	if (itsMouseContainer != obj)
		{
		if (itsMouseContainer != nullptr)
			{
			itsMouseContainer->MouseLeave();
			itsMouseContainer->DeactivateCursor();
			}

		itsMouseContainer = obj;
		if (itsMouseContainer != nullptr)
			{
			itsMouseContainer->ActivateCursor(ptG, JXKeyModifiers(itsDisplay, state));
			itsMouseContainer->MouseEnter();
			}
		}
}

/******************************************************************************
 BeginDrag

	Generate fake messages to get the given widget ready for HandleMouseDrag().

 ******************************************************************************/

JBoolean
JXWindow::BeginDrag
	(
	JXContainer*			obj,
	const JPoint&			ptG,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	assert( obj->GetWindow() == this && !itsIsDraggingFlag );
	assert( !buttonStates.AllOff() );

	const int grabResult =
		XGrabPointer(*itsDisplay, itsXWindow, False, kPointerGrabMask,
					 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	if (grabResult != GrabSuccess)
		{
		return kJFalse;
		}
	itsDisplay->SetMouseContainer(this);
	itsDisplay->SetMouseGrabber(this);

	JXContainer* savedReceiver = itsButtonPressReceiver;
	itsButtonPressReceiver     = obj;
	itsBPRChangedFlag          = kJFalse;

	XButtonPressedEvent xEvent;
	xEvent.type = ButtonPress;
	xEvent.time = CurrentTime;
	xEvent.x    = ptG.x;
	xEvent.y    = ptG.y;

	JXButtonStates newButtonStates;
	for (JIndex i=1; i<=kXButtonCount; i++)
		{
		if (buttonStates.button(i))
			{
			xEvent.state  = newButtonStates.GetState() | modifiers.GetState();
			xEvent.button = i;
			if (!HandleButtonPress(xEvent))
				{
				return kJFalse;		// window was deleted
				}
			newButtonStates.SetState(i, kJTrue);
			}
		}

	if (!itsBPRChangedFlag)
		{
		itsButtonPressReceiver = savedReceiver;
		}
	return itsProcessDragFlag;
}

/******************************************************************************
 EndDrag

	Generate fake messages to the current drag recipient so that it thinks
	that the mouse was released.

 ******************************************************************************/

void
JXWindow::EndDrag
	(
	JXContainer*			obj,
	const JPoint&			ptG,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
//	assert( itsMouseContainer == obj && itsIsDraggingFlag && itsProcessDragFlag );
//	assert( !buttonStates.AllOff() );

	if (itsMouseContainer != obj || !itsIsDraggingFlag || !itsProcessDragFlag ||
		buttonStates.AllOff())
		{
		return;
		}

	XButtonReleasedEvent xEvent;
	xEvent.type = ButtonRelease;
	xEvent.time = CurrentTime;
	xEvent.x    = ptG.x;
	xEvent.y    = ptG.y;

	JXButtonStates newButtonStates(buttonStates);
	for (JIndex i=1; i<=kXButtonCount; i++)
		{
		if (buttonStates.button(i))
			{
			xEvent.state  = newButtonStates.GetState() | modifiers.GetState();
			xEvent.button = i;
			if (!HandleButtonRelease(xEvent))
				{
				return;		// window was deleted
				}
			newButtonStates.SetState(i, kJFalse);
			}
		}

	XUngrabPointer(*itsDisplay, CurrentTime);
	itsDisplay->SetMouseGrabber(nullptr);
}

/******************************************************************************
 GrabPointer

 ******************************************************************************/

JBoolean
JXWindow::GrabPointer
	(
	JXContainer* obj
	)
{
	assert( obj->GetWindow() == this );

	const int success =
		XGrabPointer(*itsDisplay, itsXWindow, False, kPointerGrabMask,
					 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

	if (success == GrabSuccess)
		{
		itsPointerGrabbedFlag  = kJTrue;
		itsButtonPressReceiver = obj;
		itsBPRChangedFlag      = kJTrue;		// notify BeginDrag()
		itsDisplay->SetMouseContainer(this);
		itsDisplay->SetMouseGrabber(this);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 UngrabPointer

 ******************************************************************************/

void
JXWindow::UngrabPointer
	(
	JXContainer* obj
	)
{
	if (obj == itsButtonPressReceiver)
		{
		XUngrabPointer(*itsDisplay, CurrentTime);
		itsPointerGrabbedFlag  = kJFalse;
		itsButtonPressReceiver = this;
		itsBPRChangedFlag      = kJTrue;		// notify BeginDrag()
		itsDisplay->SetMouseGrabber(nullptr);
		}
}

/******************************************************************************
 HandleFocusIn (private)

 ******************************************************************************/

void
JXWindow::HandleFocusIn
	(
	const XFocusChangeEvent& xEvent
	)
{
	itsHasFocusFlag = kJTrue;
	XSetICFocus(itsXIC);

	if (itsFocusWidget != nullptr)
		{
		itsFocusWidget->HandleWindowFocusEvent();
		}

	if (itsIsDockedFlag && itsDockWidget != nullptr)
		{
		(itsDockWidget->GetDockDirector())->SetFocusWindow(this);
		}

//	std::cout << "in : " << xEvent.window << ' ' << xEvent.detail << std::endl;
}

/******************************************************************************
 HandleFocusOut (private)

 ******************************************************************************/

void
JXWindow::HandleFocusOut
	(
	const XFocusChangeEvent& xEvent
	)
{
	itsHasFocusFlag = kJFalse;
	XUnsetICFocus(itsXIC);

	if (itsFocusWidget != nullptr)
		{
		itsFocusWidget->HandleWindowUnfocusEvent();
		}

	if (itsIsDockedFlag && itsDockWidget != nullptr)
		{
		(itsDockWidget->GetDockDirector())->ClearFocusWindow(this);
		}

//	std::cout << "out: " << xEvent.window << ' ' << xEvent.detail << std::endl;
}

/******************************************************************************
 SetXIMPosition

 ******************************************************************************/

void
JXWindow::SetXIMPosition
	(
	const JPoint& ptG
	)
{
	XPoint xpt;
	xpt.x = ptG.x;
	xpt.y = ptG.y;

	XVaNestedList attr = XVaCreateNestedList(0, XNSpotLocation, &xpt, NULL);
	XSetICValues(itsXIC, XNPreeditAttributes, attr, NULL);
	XFree(attr);
}

/******************************************************************************
 HandleKeyPress (private)

 ******************************************************************************/

void
JXWindow::HandleKeyPress
	(
	const XEvent& xEvent	// avoids cast in call to JXGetButtonAndModifierStates()
	)
{
	if (!IsActive())
		{
		return;
		}
	if (itsMainWindow != nullptr)	// send keypresses to main window, not icon
		{
		itsMainWindow->HandleKeyPress(xEvent);
		return;
		}

	JUtf8Byte buffer[256];
	KeySym keySym = 0;
	Status status;
	JSize byteCount =
		Xutf8LookupString(itsXIC, const_cast<XKeyPressedEvent*>(&(xEvent.xkey)),
						  buffer, sizeof(buffer)-1, &keySym, &status);

	assert( status != XBufferOverflow );

	if (status == XLookupNone)
		{
		return;
		}
	else if (status == XLookupChars)
		{
		keySym = 0;
		}

	if (byteCount == 0)
		{
		buffer[0] = '\0';
		}

	TossKeyRepeatEvents(xEvent.xkey.keycode, xEvent.xkey.state, keySym);

	// translate some useful keys

	if (keySym == XK_KP_Tab || keySym == XK_ISO_Left_Tab)
		{
		keySym = XK_Tab;
		}
	else if (keySym == XK_Left || keySym == XK_KP_Left)
		{
		byteCount = 1;
		buffer[0] = kJLeftArrow;
		buffer[1] = '\0';
		}
	else if (keySym == XK_Up || keySym == XK_KP_Up)
		{
		byteCount = 1;
		buffer[0] = kJUpArrow;
		buffer[1] = '\0';
		}
	else if (keySym == XK_Right || keySym == XK_KP_Right)
		{
		byteCount = 1;
		buffer[0] = kJRightArrow;
		buffer[1] = '\0';
		}
	else if (keySym == XK_Down || keySym == XK_KP_Down)
		{
		byteCount = 1;
		buffer[0] = kJDownArrow;
		buffer[1] = '\0';
		}

	// the control modifier causes these to be mis-interpreted

	else if (XK_space <= keySym && keySym <= XK_question)
		{
		byteCount = 1;
		buffer[0] = keySym;
		buffer[1] = '\0';
		}
	else if (XK_KP_0 <= keySym && keySym <= XK_KP_9)
		{
		byteCount = 1;
		buffer[0] = '0' + (keySym - XK_KP_0);
		buffer[1] = '\0';
		}

	// dispatch key

	unsigned int state;
	const JBoolean foundState = JXGetButtonAndModifierStates(xEvent, itsDisplay, &state);
	assert( foundState );

	const JXKeyModifiers modifiers(itsDisplay, state);

	JXKeyModifiers noShift(modifiers);
	noShift.SetState(kJXShiftKeyIndex, kJFalse);
	noShift.SetState(kJXShiftLockKeyIndex, kJFalse);
	noShift.SetState(kJXNumLockKeyIndex, kJFalse);
	noShift.SetState(kJXScrollLockKeyIndex, kJFalse);
	const JBoolean modsOff = noShift.AllOff();

	JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
	Display* xDisplay  = *display;
	Window xWindow     = itsXWindow;

	// ESC cancels Drag-And-Drop

	if (byteCount == 1 && buffer[0] == kJEscapeKey &&
		GetDNDManager()->IsDragging())
		{
		if (GetDNDManager()->CancelDND())
			{
			EndDrag(itsMouseContainer, JPoint(0,0), JXButtonStates(state), modifiers);
			}

		// We won't dispatch any motion events until the user releases
		// the mouse, but the user needs some reassurance that the drop
		// really has been cancelled.

		DisplayXCursor(kJXDefaultCursor);
		}

	// We check WillAcceptFocus() because we don't want to send keypresses
	// to invisible or inactive widgets.

	else if (keySym == XK_Tab && itsFocusWidget != nullptr &&
			 itsFocusWidget->WillAcceptFocus() &&
			 (( modsOff && itsFocusWidget->WantsTab()) ||
			  (!modsOff && itsFocusWidget->WantsModifiedTab())))
		{
		// We send tab directly to the focus widget so it
		// doesn't get lost as a shortcut.

		DeactivateHint();
		itsFocusWidget->HandleKeyPress(JUtf8Character(kJTabKey), 0, modifiers);
		}
	else if (keySym == XK_Tab)
		{
		SwitchFocus(modifiers.shift());
		}
	else if (IsShortcut(keySym, state))
		{
		// IsShortcut() dispatches event
		}
	else if (itsIsDockedFlag && modifiers.control() &&
			 (keySym == XK_Page_Down || keySym == XK_KP_Page_Down))
		{
		itsDockWidget->ShowPreviousWindow();
		}
	else if (itsIsDockedFlag && modifiers.control() &&
			 (keySym == XK_Page_Up || keySym == XK_KP_Page_Up))
		{
		itsDockWidget->ShowNextWindow();
		}
	else if (itsFocusWidget != nullptr && itsFocusWidget->WillAcceptFocus())
		{
		DeactivateHint();

		if (byteCount > 0)
			{
			const JString s(buffer, byteCount, kJFalse);
			JStringIterator iter(s);
			JUtf8Character c;
			while (iter.Next(&c))
				{
				itsFocusWidget->HandleKeyPress(c, 0, modifiers);
				if (iter.GetPrevCharacterIndex() > 1 && !iter.AtEnd() &&
					!JXDisplay::WindowExists(display, xDisplay, xWindow))
					{
					break;
					}
				}
			}
		else
			{
			itsFocusWidget->HandleKeyPress(JUtf8Character(), keySym, modifiers);
			}
		}

	if (JXDisplay::WindowExists(display, xDisplay, xWindow))
		{
		Update();
		}
}

/******************************************************************************
 TossKeyRepeatEvents (private)

	If the user holds down a key, we don't want to accumulate a backlog of
	KeyPress,KeyRelease events.

 ******************************************************************************/

void
JXWindow::TossKeyRepeatEvents
	(
	const unsigned int	keycode,
	const unsigned int	state,
	const KeySym		keySym
	)
{
	if (keySym != XK_Left  && keySym != XK_KP_Left &&
		keySym != XK_Up    && keySym != XK_KP_Up &&
		keySym != XK_Right && keySym != XK_KP_Right &&
		keySym != XK_Down  && keySym != XK_KP_Down &&

		keySym != XK_Page_Up   && keySym != XK_KP_Page_Up &&
		keySym != XK_Page_Down && keySym != XK_KP_Page_Down &&

		keySym != XK_BackSpace && keySym != XK_Delete)
		{
		return;
		}

	XEvent xEvent;
	while (XPending(*itsDisplay) > 0)
		{
		XPeekEvent(*itsDisplay, &xEvent);
		if ((xEvent.type == KeyPress || xEvent.type == KeyRelease) &&
			xEvent.xkey.keycode == keycode &&
			xEvent.xkey.state   == state)
			{
			XNextEvent(*itsDisplay, &xEvent);
			}
		else
			{
			break;
			}
		}
}

/******************************************************************************
 DeactivateHint

 ******************************************************************************/

void
JXWindow::DeactivateHint()
{
	if (itsCurrHintMgr != nullptr)
		{
		itsCurrHintMgr->Deactivate();
		itsCurrHintMgr = nullptr;
		}
}

/******************************************************************************
 RegisterFocusWidget

 ******************************************************************************/

void
JXWindow::RegisterFocusWidget
	(
	JXWidget* widget
	)
{
	if (!itsFocusList->Includes(widget))
		{
		itsFocusList->Append(widget);
		if (itsFocusList->GetElementCount() == 1)
			{
			assert( itsFocusWidget == nullptr );
			SwitchFocus(kJFalse);
			}
		}
}

/******************************************************************************
 UnregisterFocusWidget

 ******************************************************************************/

void
JXWindow::UnregisterFocusWidget
	(
	JXWidget* widget
	)
{
	if (!itsIsDestructingFlag)
		{
		itsFocusList->Remove(widget);
		if (widget == itsFocusWidget)
			{
			itsFocusWidget = nullptr;
			SwitchFocus(kJFalse);
			}
		}
}

/******************************************************************************
 SwitchFocusToFirstWidget

 ******************************************************************************/

JBoolean
JXWindow::SwitchFocusToFirstWidget()
{
	JXWidget* firstWidget;
	if (!FindNextFocusWidget(0, &firstWidget))
		{
		return kJTrue;
		}

	if (itsFocusWidget == firstWidget)
		{
		return kJTrue;
		}
	else if (UnfocusCurrentWidget())
		{
		itsFocusWidget = firstWidget;
		itsFocusWidget->Focus(0);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SwitchFocusToFirstWidgetWithAncestor

 ******************************************************************************/

JBoolean
JXWindow::SwitchFocusToFirstWidgetWithAncestor
	(
	JXContainer* ancestor
	)
{
	JXWidget** firstWidget =
		std::find_if(begin(*itsFocusList), end(*itsFocusList),
			[ancestor] (JXWidget* widget)
			{
				return (widget->WillAcceptFocus() &&
						ancestor->IsAncestor(widget));
			});

	if (firstWidget == end(*itsFocusList) || itsFocusWidget == *firstWidget)
		{
		return kJTrue;
		}
	else if (UnfocusCurrentWidget())
		{
		itsFocusWidget = *firstWidget;
		itsFocusWidget->Focus(0);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 OKToUnfocusCurrentWidget

 ******************************************************************************/

JBoolean
JXWindow::OKToUnfocusCurrentWidget()
	const
{
	if (itsFocusWidget == nullptr ||
		itsFocusWidget->OKToUnfocus())
		{
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 UnfocusCurrentWidget

 ******************************************************************************/

JBoolean
JXWindow::UnfocusCurrentWidget()
{
	if (itsFocusWidget == nullptr)
		{
		return kJTrue;
		}
	else if (itsFocusWidget->OKToUnfocus())
		{
		KillFocus();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 KillFocus

	*** This does not check for valid input.  Use with caution!

 ******************************************************************************/

void
JXWindow::KillFocus()
{
	if (itsFocusWidget != nullptr)
		{
		JXWidget* origWidget = itsFocusWidget;
		itsFocusWidget = nullptr;		// clear this first
		origWidget->NotifyFocusLost();
		}
}

/******************************************************************************
 SwitchFocusToWidget

 ******************************************************************************/

JBoolean
JXWindow::SwitchFocusToWidget
	(
	JXWidget* widget
	)
{
	if (itsExpandTask != nullptr)
		{
		itsExpandTask->FocusAfterFTC(widget);
		return kJTrue;
		}
	else if (itsFocusWidget == widget)
		{
		return kJTrue;
		}
	else if (itsFocusList->Includes(widget) &&
			 widget->WillAcceptFocus() &&
			 UnfocusCurrentWidget())
		{
		itsFocusWidget = widget;
		itsFocusWidget->Focus(0);
		return kJTrue;
		}

	return kJFalse;
}

/******************************************************************************
 SwitchFocus (private)

 ******************************************************************************/

void
JXWindow::SwitchFocus
	(
	const JBoolean backward
	)
{
	if (itsFocusList->IsEmpty())
		{
		// nothing to do
		assert( itsFocusWidget == nullptr );
		}
	else if (itsFocusWidget == nullptr && backward)
		{
		if (FindPrevFocusWidget(0, &itsFocusWidget))
			{
			itsFocusWidget->Focus(0);
			}
		}
	else if (itsFocusWidget == nullptr)
		{
		if (FindNextFocusWidget(0, &itsFocusWidget))
			{
			itsFocusWidget->Focus(0);
			}
		}
	else if (itsFocusWidget->OKToUnfocus())
		{
		JXWidget* widget = itsFocusWidget;
		KillFocus();

		// In the following code, we assume nothing about itsFocusList
		// because NotifyFocusLost() could do anything.  (Usually,
		// of course, it should not affect itsFocusList at all.)

		const JBoolean empty = itsFocusList->IsEmpty();

		JIndex currIndex;
		if (!empty && itsFocusList->Find(widget, &currIndex))
			{
			if (backward)
				{
				FindPrevFocusWidget(currIndex, &widget);
				}
			else
				{
				FindNextFocusWidget(currIndex, &widget);
				}
			}
		else if (!empty && backward)
			{
			FindPrevFocusWidget(0, &widget);
			}
		else if (!empty)
			{
			FindNextFocusWidget(0, &widget);
			}
		else
			{
			widget = nullptr;
			}

		if (widget != nullptr)
			{
			itsFocusWidget = widget;
			itsFocusWidget->Focus(0);
			}
		}
}

/******************************************************************************
 FindNextFocusWidget (private)

	Look forward in the list to find a widget after the specified one.
	If nothing can be found, sets *widget = nullptr and returns kJFalse.

	(startIndex == 0) => start at beginning of list

 ******************************************************************************/

JBoolean
JXWindow::FindNextFocusWidget
	(
	const JIndex	origStartIndex,
	JXWidget**		focusWidget
	)
	const
{
	*focusWidget = nullptr;

	const JSize count = itsFocusList->GetElementCount();
	if (count == 0)
		{
		return kJFalse;
		}

	JIndex startIndex = origStartIndex;
	if (startIndex == 0 || startIndex > count)
		{
		startIndex = count;
		}

	JIndex i = startIndex+1;
	if (i > count)
		{
		i = 1;
		}

	while (1)
		{
		JXWidget* widget = itsFocusList->GetElement(i);
		if (widget->WillAcceptFocus())
			{
			*focusWidget = widget;
			return kJTrue;
			}
		else if (i == startIndex)
			{
			return kJFalse;
			}

		i++;
		if (i > count)
			{
			i = 1;
			}
		}
}

/******************************************************************************
 FindPrevFocusWidget (private)

	Look backwards in the list to find a widget in front of the specified one.
	If nothing can be found, sets *widget = nullptr and returns kJFalse.

	(startIndex == 0) => start at end of list

 ******************************************************************************/

JBoolean
JXWindow::FindPrevFocusWidget
	(
	const JIndex	origStartIndex,
	JXWidget**		focusWidget
	)
	const
{
	*focusWidget = nullptr;

	const JSize count = itsFocusList->GetElementCount();
	if (count == 0)
		{
		return kJFalse;
		}

	JIndex startIndex = origStartIndex;
	if (startIndex == 0 || startIndex > count)
		{
		startIndex = 1;
		}

	JIndex i = startIndex-1;
	if (i == 0)
		{
		i = count;
		}

	while (1)
		{
		JXWidget* widget = itsFocusList->GetElement(i);
		if (widget->WillAcceptFocus())
			{
			*focusWidget = widget;
			return kJTrue;
			}
		else if (i == startIndex)
			{
			return kJFalse;
			}

		i--;
		if (i == 0)
			{
			i = count;
			}
		}
}

/******************************************************************************
 InstallShortcut

	To specify a control character, you must pass in the unmodified character
	along with modifiers.control().  e.g. Pass in 'A',control instead
	of JXCtrl('A'),control.

 ******************************************************************************/

JBoolean
JXWindow::InstallShortcut
	(
	JXWidget*				widget,
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	return InstallShortcut(Shortcut(widget, key, modifiers.GetState()));
}

/******************************************************************************
 InstallMenuShortcut

 ******************************************************************************/

JBoolean
JXWindow::InstallMenuShortcut
	(
	JXTextMenu*				menu,
	const JIndex			menuItem,
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	return InstallShortcut(Shortcut(menu, menuItem, key, modifiers.GetState()));
}

/******************************************************************************
 InstallShortcut (private)

	Returns kJTrue if shortcut was successfully registered.

	ShiftLock is always ignored.

 ******************************************************************************/

JBoolean
JXWindow::InstallShortcut
	(
	const Shortcut& origShortcut
	)
{
	Shortcut s = origShortcut;
	s.state    = JXKeyModifiers::SetState(itsDisplay, s.state,
										  kJXShiftLockKeyIndex, kJFalse);

	const JBoolean inserted = itsShortcuts->InsertSorted(s, kJFalse);

	// For characters other than the alphabet or tab, return, etc., we
	// never know whether or not the Shift key is required in order to type
	// them.  We therefore install both cases.

	if (inserted &&
		0 < origShortcut.key && origShortcut.key <= 255 &&
		!isalpha(origShortcut.key) && !iscntrl(origShortcut.key))
		{
		s.state = JXKeyModifiers::ToggleState(itsDisplay, s.state, kJXShiftKeyIndex);
		itsShortcuts->InsertSorted(s, kJFalse);
		}

	return inserted;
}

/******************************************************************************
 MenuItemInserted

	Update the item indices of the shortcuts.

 ******************************************************************************/

void
JXWindow::MenuItemInserted
	(
	JXTextMenu*		menu,
	const JIndex	newItem
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu && s.menuItem >= newItem)
			{
			(s.menuItem)++;
			itsShortcuts->SetElement(i, s);
			}
		}
}

/******************************************************************************
 MenuItemRemoved

	Remove the shortcuts for the item and update the item indices of the
	other shortcuts.

 ******************************************************************************/

void
JXWindow::MenuItemRemoved
	(
	JXTextMenu*		menu,
	const JIndex	oldItem
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu && s.menuItem > oldItem)
			{
			(s.menuItem)--;
			itsShortcuts->SetElement(i, s);
			}
		else if (s.menu == menu && s.menuItem == oldItem)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 InstallShortcuts

	Parses the given string to get the shortcuts.  A caret (^) indicates
	that the next character is a control character.  A hash (#) indicates that
	the next character requires the Meta modifier.  F1 - F35 are parsed into
	function keys.

 ******************************************************************************/

void
JXWindow::InstallShortcuts
	(
	JXWidget*		widget,
	const JString&	list
	)
{
	if (list.IsEmpty())
		{
		return;
		}

	JXKeyModifiers modifiers(itsDisplay);

	JStringIterator iter(list);
	JUtf8Character c;
	while (iter.Next(&c))
		{
		JSize fnIndex;
		if (c == '^')
			{
			if (!iter.Next(&c) || c.GetByteCount() > 1)
				{
				continue;
				}

			const JUtf8Character c1 = c.ToUpper();
			const JUtf8Character c2 = c.ToLower();
			modifiers.SetState(kJXControlKeyIndex, kJTrue);		// e.g. Ctrl-M
			InstallShortcut(widget, (unsigned char) c1.GetBytes()[0], modifiers);
			if (c2 != c1)
				{
				InstallShortcut(widget, (unsigned char) c2.GetBytes()[0], modifiers);
				}
			modifiers.SetState(kJXControlKeyIndex, kJFalse);	// e.g. return key
			InstallShortcut(widget, (unsigned char) JXCtrl(c1), modifiers);
			}
		else if (c == '#')
			{
			if (!iter.Next(&c) || c.GetByteCount() > 1)
				{
				continue;
				}

			const JUtf8Character c1 = c.ToUpper();
			const JUtf8Character c2 = c.ToLower();
			modifiers.SetState(kJXMetaKeyIndex, kJTrue);
			InstallShortcut(widget, (unsigned char) c1.GetBytes()[0], modifiers);
			if (c2 != c1)
				{
				InstallShortcut(widget, (unsigned char) c2.GetBytes()[0], modifiers);
				}
			modifiers.SetState(kJXMetaKeyIndex, kJFalse);
			}
		else if (c == 'F' &&
				 (fnIndex = strtoul(list.GetBytes() + iter.GetPrevByteIndex(), nullptr, 10)) > 0 &&
				 fnIndex <= 35)
			{
			InstallShortcut(widget, XK_F1 + fnIndex-1, modifiers);
			iter.SkipNext(fnIndex >= 10 ? 2 : 1);
			}
		else if (c.GetByteCount() == 1)
			{
			InstallShortcut(widget, (unsigned char) c.GetBytes()[0], modifiers);
			}
		}
}

/******************************************************************************
 GetULShortcutIndex (static)

	Returns the index into label of the first shortcut character in
	the given list.  Returns zero if the first shortcut character is not
	found in label.  We return zero rather than JBoolean because
	JXWindowPainter::String() accepts zero to mean "no shortcut".

	We perform a case-insensitive search first for "^c", then " c",
	then "[^a-z]c", and finally anywhere.

	For convenience, list can be nullptr.

 ******************************************************************************/

JIndex
JXWindow::GetULShortcutIndex
	(
	const JString&	label,
	const JString*	list
	)
{
	if (label.IsEmpty() || list == nullptr || list->IsEmpty())
		{
		return 0;
		}

	JStringIterator listIter(*list);
	JUtf8Character c;

	listIter.Next(&c);		// list is not empty
	if (c == '^' || c == '#')
		{
		if (listIter.AtEnd())
			{
			return 0;
			}
		listIter.Next(&c);
		}
	listIter.Invalidate();

	c = c.ToLower();

	JString s = label;
	s.ToLower();	// may affect multiple characters

	JUtf8Character prev = s.GetFirstCharacter();
	if (c == prev)
		{
		return 1;
		}

	JIndex bdryIndex = 0, anyIndex = 0;

	JStringIterator labelIter(s, kJIteratorStartAfter, 1);
	JUtf8Character c1;
	while (labelIter.Next(&c1))
		{
		if (c == c1)
			{
			const JIndex i = labelIter.GetPrevCharacterIndex();
			if (prev.IsSpace())
				{
				return i;
				}
			else if (bdryIndex == 0 && !c1.IsAlpha())
				{
				bdryIndex = i;
				}
			else if (anyIndex == 0)
				{
				anyIndex = i;
				}
			}
		}

	return (bdryIndex > 0 ? bdryIndex :
			(anyIndex > 0 ? anyIndex : 0));
}

/******************************************************************************
 ClearShortcuts

 ******************************************************************************/

void
JXWindow::ClearShortcuts
	(
	JXWidget* widget
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.widget == widget)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 ClearMenuShortcut

 ******************************************************************************/

void
JXWindow::ClearMenuShortcut
	(
	JXTextMenu*		menu,
	const JIndex	menuItem
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu && s.menuItem == menuItem)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 ClearAllMenuShortcuts

 ******************************************************************************/

void
JXWindow::ClearAllMenuShortcuts
	(
	JXTextMenu* menu
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 IsShortcut (private)

 ******************************************************************************/

//#include <stdio.h>

JBoolean
JXWindow::IsShortcut
	(
	const KeySym		keySym,
	const unsigned int	origState
	)
{
	int key            = keySym;
	unsigned int state = origState;

//	printf("IsShortcut: origState: %X\n", state);

	JXKeyModifiers modifiers(itsDisplay, state);
	modifiers.SetState(kJXNumLockKeyIndex, kJFalse);
	modifiers.SetState(kJXScrollLockKeyIndex, kJFalse);
	modifiers.SetState(kJXShiftLockKeyIndex, kJFalse);
	state = modifiers.GetState();

	if (0 < key && key <= 255)
		{
		key = tolower(key);
		}
	else if (key == XK_Return || key == XK_KP_Enter)
		{
		key = '\r';
		}
	else if (key == XK_Escape)
		{
		key = kJEscapeKey;
		}
	else if (keySym == XK_KP_Tab || keySym == XK_ISO_Left_Tab)
		{
		key = kJTabKey;
		}
	else if (keySym == XK_Left || keySym == XK_KP_Left)
		{
		key = kJLeftArrow;
		}
	else if (keySym == XK_Up || keySym == XK_KP_Up)
		{
		key = kJUpArrow;
		}
	else if (keySym == XK_Right || keySym == XK_KP_Right)
		{
		key = kJRightArrow;
		}
	else if (keySym == XK_Down || keySym == XK_KP_Down)
		{
		key = kJDownArrow;
		}

//	printf("IsShortcut: newState: %X\n\n", state);

	Shortcut target(nullptr, key, state);
	JIndex i;
	const JBoolean found = itsShortcuts->SearchSorted(target, JListT::kAnyMatch, &i);

	if (found)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.widget != nullptr && (s.widget)->WillAcceptShortcut())
			{
			(s.widget)->HandleShortcut(s.key, modifiers);
			}
		else if (s.menu != nullptr)
			{
			(s.menu)->HandleNMShortcut(s.menuItem, modifiers);
			}
		}

	// We always return kJTrue if it is a shortcut, even if the Widget didn't
	// want to accept it.  This guarantees that keys will always behave
	// the same.  Otherwise, a deactivated button's shortcut would go to the
	// active input field.

	return found;
}

/******************************************************************************
 CompareShortcuts (static private)

	Sorts by key, then by state.

 ******************************************************************************/

JListT::CompareResult
JXWindow::CompareShortcuts
	(
	const Shortcut& s1,
	const Shortcut& s2
	)
{
	if (s1.key < s2.key)
		{
		return JListT::kFirstLessSecond;
		}
	else if (s1.key > s2.key)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (s1.state < s2.state)
		{
		return JListT::kFirstLessSecond;
		}
	else if (s1.state > s2.state)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

/******************************************************************************
 HandleMapNotify (private)

 ******************************************************************************/

void
JXWindow::HandleMapNotify
	(
	const XMapEvent& mapEvent
	)
{
	itsIsMappedFlag = kJTrue;

	if (itsIsOverlayFlag)
		{
		// Menu windows are not touched by the window manager.
		return;
		}

	Broadcast(Mapped());

	// focus to the window for convenience

	const JXDisplay::WMBehavior& b = itsDisplay->GetWMBehavior();
	if (!b.desktopMapsWindowsFlag && itsFocusWhenShowFlag)
		{
		(itsDisplay->GetMenuManager())->CloseCurrentMenus();	// avoid deadlock
		RequestFocus();
		}

	if (itsIsDockedFlag)
		{
		itsIsIconifiedFlag = kJFalse;
		return;
		}

	// broadcast whether window is iconified or deiconified

	HandleWMStateChange();

	// The user only expects the window to start iconified the first time.

	SetWindowStateHint(NormalState);
}

/******************************************************************************
 HandleUnmapNotify (private)

 ******************************************************************************/

void
JXWindow::HandleUnmapNotify
	(
	const XUnmapEvent& unmapEvent
	)
{
	itsIsMappedFlag = kJFalse;

	if (IsVisible() && !itsIsIconifiedFlag)
		{
		itsIsIconifiedFlag = kJTrue;
		Broadcast(Iconified());
		}
	else
		{
		Broadcast(Unmapped());
		}

//	std::cout << "JXWindow::HandleUnmapNotify: " << itsIsMappedFlag << ' ' << itsIsIconifiedFlag << std::endl;
}

/******************************************************************************
 HandleWMStateChange (private)

 ******************************************************************************/

void
JXWindow::HandleWMStateChange()
{
	if (itsIsOverlayFlag || itsIsDockedFlag || !itsIsMappedFlag)
		{
		// Menu windows are not touched by the window manager.
		return;
		}

	// broadcast whether window is iconified or deiconified

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* xdata;
	XGetWindowProperty(*itsDisplay, itsXWindow, itsDisplay->GetWMStateXAtom(),
					   0, LONG_MAX, False, AnyPropertyType,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &xdata);
	if (actualType != itsDisplay->GetWMStateXAtom() || actualFormat != 32)
		{
#if ! defined _J_OSX && ! defined _J_CYGWIN
		std::cerr << std::endl;
		std::cerr << "Error detected in JXWindow::HandleMapNotify():" << std::endl;
		std::cerr << "Your window manager is not setting the WM_STATE property correctly!" << std::endl;
		std::cerr << std::endl;
//		JXApplication::Abort(JXDocumentManager::kServerDead, kJFalse);
#endif

		if (itsIsIconifiedFlag)
			{
			itsIsIconifiedFlag = kJFalse;
			Broadcast(Deiconified());
			Refresh();
			}
		}
	else
		{
		assert( remainingBytes == 0 );

		if (*reinterpret_cast<JUInt32*>(xdata) == NormalState && itsIsIconifiedFlag)
			{
			itsIsIconifiedFlag = kJFalse;
			Broadcast(Deiconified());
			Refresh();
			}
		else if (*reinterpret_cast<JUInt32*>(xdata) == IconicState && !itsIsIconifiedFlag)
			{
			itsIsIconifiedFlag = kJTrue;
			Broadcast(Iconified());
			}
		}

//	std::cout << "JXWindow::HandleWMStateChange: " << itsIsMappedFlag << ' ' << itsIsIconifiedFlag << std::endl;

	XFree(xdata);
}

/******************************************************************************
 CheckForMapOrExpose

	For use by blocking loops that want to refresh a particular window.
	This isn't safe in general because it can invoke huge amounts of
	code.  Use with caution.

 ******************************************************************************/

void
JXWindow::CheckForMapOrExpose()
{
	XEvent xEvent;
	while (XCheckIfEvent(*itsDisplay, &xEvent, GetNextMapOrExposeEvent,
						 reinterpret_cast<char*>(&itsXWindow)))
		{
		HandleEvent(xEvent);
		}
}

// static private

Bool
JXWindow::GetNextMapOrExposeEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	const Window window    = (event->xany).window;
	const Window itsWindow = *(reinterpret_cast<Window*>(arg));
	if (window == itsWindow &&
		(event->type == MapNotify || event->type == UnmapNotify ||
		 event->type == Expose))
		{
		return True;
		}
	else
		{
		return False;
		}
}

/******************************************************************************
 HandleExpose (private)

	When we get an expose event, we process all the following expose events
	as well.

 ******************************************************************************/

void
JXWindow::HandleExpose
	(
	const XExposeEvent& exposeEvent
	)
{
	if (itsUseBkgdPixmapFlag)
		{
		return;
		}

	HandleExpose1(exposeEvent);

	XEvent xEvent;
	while (XCheckIfEvent(*itsDisplay, &xEvent, GetNextExposeEvent,
						 reinterpret_cast<char*>(&itsXWindow)))
		{
		HandleExpose1(xEvent.xexpose);
		}
}

void
JXWindow::HandleExpose1
	(
	const XExposeEvent& exposeEvent
	)
{
	XRectangle xrect;
	xrect.x      = exposeEvent.x;
	xrect.y      = exposeEvent.y;
	xrect.width  = exposeEvent.width;
	xrect.height = exposeEvent.height;
	XUnionRectWithRegion(&xrect, itsUpdateRegion, itsUpdateRegion);
	itsDisplay->WindowNeedsUpdate(this);
}

// static private

Bool
JXWindow::GetNextExposeEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	const Window window    = (event->xany).window;
	const Window itsWindow = *(reinterpret_cast<Window*>(arg));
	if (window == itsWindow && event->type == Expose)
		{
		return True;
		}
	else
		{
		return False;
		}
}

/******************************************************************************
 GetDockWindow

 ******************************************************************************/

JBoolean
JXWindow::GetDockWindow
	(
	JXWindow** window
	)
	const
{
	if (itsDockWidget != nullptr)
		{
		*window = itsDockWidget->JXContainer::GetWindow();
		return kJTrue;
		}
	else
		{
		*window = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 Dock

	dock can be nullptr.  We take an X Window instead of a JXWindow in order to
	allow JXWindows to be docked in other program's windows.

	geom must be in the local coordinates of parent.

	Returns kJFalse if the window cannot be docked.

 ******************************************************************************/

JBoolean
JXWindow::Dock
	(
	JXDockWidget*	dock,
	const Window	parent,
	const JRect&	geom
	)
{
	if (itsIsOverlayFlag || itsMainWindow != nullptr ||
		geom.width() < itsMinSize.x || geom.height() < itsMinSize.y ||
		itsDockingTask != nullptr)
		{
		return kJFalse;
		}

	if (itsIsDockedFlag)
		{
		Broadcast(Undocked());
		}
	else
		{
		itsUndockedGeom       = itsBounds;
		itsUndockedWMFrameLoc = itsWMFrameLoc;
		}

	itsDockingTask = jnew JXDockWindowTask(this, parent, JPoint(geom.left, geom.top), dock);
	assert( itsDockingTask != nullptr );
	itsDockingTask->Start();
	ClearWhenGoingAway(itsDockingTask, &itsDockingTask);

	itsIsDockedFlag = kJTrue;						// after creating task
	UndockedSetSize(geom.width(), geom.height());	// after setting itsIsDockedFlag

	itsDockXWindow = parent;
	itsDockWidget  = dock;
	Broadcast(Docked());

	return kJTrue;
}

/******************************************************************************
 Undock

 ******************************************************************************/

void
JXWindow::Undock()
{
	if (!itsIsDockedFlag)
		{
		return;
		}

	const JBoolean wasVisible = IsVisible();

	if (itsDockWidget != nullptr)
		{
		(itsDockWidget->GetDockDirector())->ClearFocusWindow(this);
		}

	Hide();
	itsIsMappedFlag = kJFalse;

	XReparentWindow(*itsDisplay, itsXWindow, itsDisplay->GetRootWindow(),
					itsWMFrameLoc.x, itsWMFrameLoc.y);

	itsIsDockedFlag = kJFalse;
	itsDockXWindow  = None;
	itsDockWidget   = nullptr;
	itsRootChild    = None;	// don't wait for ReparentNotify
	jdelete itsDockingTask;	// automatically cleared

	Sync(itsDisplay);
	Place(itsUndockedWMFrameLoc.x, itsUndockedWMFrameLoc.y);
	SetSize(itsUndockedGeom.width(), itsUndockedGeom.height());

	if (wasVisible)
		{
		JXRaiseWindowTask* task = jnew JXRaiseWindowTask(this);
		assert( task != nullptr );
		task->Go();
		}

	Broadcast(Undocked());
}

/******************************************************************************
 UndockAllChildWindows

 ******************************************************************************/

void
JXWindow::UndockAllChildWindows()
{
	if (itsChildWindowList != nullptr)
		{
		for (const ChildWindowInfo& info : *itsChildWindowList)
			{
			JXWindow* w;
			if (itsDisplay->FindXWindow(info.xWindow, &w))
				{
				w->Undock();
				}
			}
		}
}

/******************************************************************************
 UpdateChildWindowList (private)

 ******************************************************************************/

void
JXWindow::UpdateChildWindowList
	(
	const Window	xWindow,
	const JBoolean	add
	)
{
	if (add && itsChildWindowList == nullptr)
		{
		itsChildWindowList = jnew JArray<ChildWindowInfo>;
		assert( itsChildWindowList != nullptr );
		}

	if (itsChildWindowList != nullptr)
		{
		// check if xWindow is already in the list

		JIndex index = 0;

		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const ChildWindowInfo info = itsChildWindowList->GetElement(i);
			if (info.xWindow == xWindow)
				{
				index = i;
				break;
				}
			}

		// add/remove xWindow

		XWindowAttributes attr;
		if (add && index == 0 &&
			XGetWindowAttributes(*itsDisplay, xWindow, &attr))
			{
			ChildWindowInfo info(xWindow, JI2B(attr.map_state != IsUnmapped),
								 JRect(attr.y, attr.x,
									   attr.y+attr.height, attr.x+attr.width));
			itsChildWindowList->AppendElement(info);
			}
		else if (!add && index > 0)
			{
			itsChildWindowList->RemoveElement(index);
			}

		// toss list if it is empty

		if (itsChildWindowList->IsEmpty())
			{
			jdelete itsChildWindowList;
			itsChildWindowList = nullptr;
			}
		}
}

/******************************************************************************
 SetChildWindowGeometry (private)

 ******************************************************************************/

void
JXWindow::SetChildWindowGeometry
	(
	const Window	xWindow,
	const JRect&	geom
	)
{
	if (itsChildWindowList != nullptr)
		{
		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ChildWindowInfo info = itsChildWindowList->GetElement(i);
			if (info.xWindow == xWindow)
				{
				if (info.geom != geom)
					{
					info.geom = geom;
					itsChildWindowList->SetElement(i, info);
					}
				break;
				}
			}
		}
}

/******************************************************************************
 UpdateChildWindowGeometry (private)

 ******************************************************************************/

void
JXWindow::UpdateChildWindowGeometry()
{
	if (itsChildWindowList != nullptr)
		{
		for (const ChildWindowInfo& info : *itsChildWindowList)
			{
			JXWindow* w;
			if (itsDisplay->FindXWindow(info.xWindow, &w))
				{
				w->UpdateFrame();
				}
			}
		}
}

/******************************************************************************
 SetChildWindowVisible (private)

 ******************************************************************************/

void
JXWindow::SetChildWindowVisible
	(
	const Window	xWindow,
	const JBoolean	visible
	)
{
	if (itsChildWindowList != nullptr)
		{
		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ChildWindowInfo info = itsChildWindowList->GetElement(i);
			if (info.xWindow == xWindow)
				{
				if (info.visible != visible)
					{
					info.visible = visible;
					itsChildWindowList->SetElement(i, info);
					}
				break;
				}
			}
		}
}

/******************************************************************************
 PrintWindowConfig

 ******************************************************************************/

void
JXWindow::PrintWindowConfig()
{
	const JXDisplay::WMBehavior& b = itsDisplay->GetWMBehavior();

	std::cout << std::endl;
	std::cout << "wmFrameCompensateFlag:    " << b.frameCompensateFlag << std::endl;
	std::cout << "wmDesktopMapsWindowsFlag: " << b.desktopMapsWindowsFlag << std::endl;
	std::cout << "wmReshowOffset:           " << b.reshowOffset << std::endl;
	std::cout << "itsWMFrameLoc:            " << itsWMFrameLoc << std::endl;
	std::cout << "itsDesktopLoc:            " << itsDesktopLoc << std::endl;

	if (itsIsDockedFlag)
		{
		std::cout << "itsUndockedWMFrameLoc:    " << itsUndockedWMFrameLoc << std::endl;
		std::cout << "itsUndockedGeom:          " << itsUndockedGeom << std::endl;
		}

	std::cout << std::endl;
}
