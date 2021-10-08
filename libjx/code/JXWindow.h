/******************************************************************************
 JXWindow.h

	Interface for the JXWindow class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWindow
#define _H_JXWindow

#include "jx-af/jx/JXContainer.h"
#include "jx-af/jx/JXCursor.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <jx-af/jcore/JString.h>

class JXDisplay;
class JXGC;
class JXImage;
class JXWindowDirector;
class JXWidget;
class JXTextMenu;
class JXIconDirector;
class JXWindowIcon;
class JXHintManager;
class JXDockWidget;
class JXDockWindowTask;
class JXExpandWindowToFitContentTask;

class JXWindow : public JXContainer
{
	friend class JXDockWindowTask;
	friend class JXExpandWindowToFitContentTask;

public:

	enum CloseAction
	{
		kDeactivateDirector,
		kCloseDirector,
		kCloseDisplay,
		kQuitApp
	};

	enum WMType
	{
		kWMNormalType = 1,
		kWMDialogType,
		kWMPulldownMenuType,
		kWMPopupMenuType,
		kWMTooltipType
	};

public:

	JXWindow(JXWindowDirector* director, const JCoordinate w, const JCoordinate h,
			 const JString& title, const bool isOverlay = false);

	~JXWindow();

	JXDisplay*	GetDisplay() const;

	const JString&	GetTitle() const;
	void			SetTitle(const JString& title);

	bool		Close();
	CloseAction	GetCloseAction() const;
	void		SetCloseAction(const CloseAction closeAction);

	void	SetTransientFor(const JXWindowDirector* director);
	void	SetWMClass(const JUtf8Byte* c_class, const JUtf8Byte* instance);
	void	HideFromTaskbar();

	void	Show() override;
	void	Hide() override;
	void	Raise(const bool grabKeyboardFocus = true);
	void	Lower();
	void	Refresh() const override;
	void	RefreshRect(const JRect& rect) const;
	void	Redraw() const override;
	void	RedrawRect(const JRect& rect) const;
	void	CheckForMapOrExpose();

	void	Iconify();
	void	Deiconify();
	bool	IsIconified() const;

	bool	GetFocusWidget(JXWidget** widget) const;
	bool	SwitchFocusToFirstWidget();
	bool	SwitchFocusToFirstWidgetWithAncestor(JXContainer* ancestor);
	bool	OKToUnfocusCurrentWidget() const;
	bool	UnfocusCurrentWidget();
	void	KillFocus();

	void	Activate() override;
	void	Resume() override;
	bool	WillFocusWhenShown() const;
	void	ShouldFocusWhenShow(const bool focusWhenShow);
	void	RequestFocus();
	bool	HasFocus() const;

	JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const override;
	JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const override;

	JPoint	GlobalToRoot(const JCoordinate x, const JCoordinate y) const;
	JPoint	GlobalToRoot(const JPoint& pt) const;
	JRect	GlobalToRoot(const JRect& r) const;
	JPoint	RootToGlobal(const JCoordinate x, const JCoordinate y) const;
	JPoint	RootToGlobal(const JPoint& pt) const;
	JRect	RootToGlobal(const JRect& r) const;

	void	Place(const JCoordinate enclX, const JCoordinate enclY) override;
	void	Move(const JCoordinate dx, const JCoordinate dy) override;
	void	SetSize(const JCoordinate w, const JCoordinate h) override;
	void	AdjustSize(const JCoordinate dw, const JCoordinate dh) override;
	void	CenterOnScreen();
	void	PlaceAsDialogWindow();

	JPoint	GetMinSize() const;
	void	SetMinSize(const JCoordinate w, const JCoordinate h);
	void	LockCurrentMinSize();
	void	ClearMinSize();

	JPoint	GetMaxSize() const;
	void	SetMaxSize(const JCoordinate w, const JCoordinate h);
	void	LockCurrentMaxSize();
	void	ClearMaxSize();
	void	LockCurrentSize();

	void	SetStepSize(const JCoordinate dw, const JCoordinate dh);
	void	ClearStepSize();

	JRect	GetBoundsGlobal() const override;
	JRect	GetFrameGlobal() const override;
	JRect	GetApertureGlobal() const override;
	JPoint	GetDesktopLocation() const;

	void		ReadGeometry(std::istream& input, const bool skipDocking = false);
	void		WriteGeometry(std::ostream& output) const;
	static void	SkipGeometry(std::istream& input);

	void	ReadGeometry(const JString& data, const bool skipDocking = false);
	void	WriteGeometry(JString* data) const;

	JXWindowDirector*	GetDirector() const;

	JColorID		GetBackColor() const;
	void			SetBackColor(const JColorID color);
	void			SetIcon(JXImage* icon);
	JXWindowIcon*	SetIcon(JXImage* normalIcon, JXImage* dropIcon);
	bool			GetIconWidget(JXWindowIcon** widget) const;

	bool	IsDragging() const;
	void	PrintWindowConfig();

	static bool	WillAutoDockNewWindows();
	static void	ShouldAutoDockNewWindows(const bool autoDock);

	static bool	WillFocusFollowCursorInDock();
	static void	ShouldFocusFollowCursorInDock(const bool focusFollowsCursor);

	void	SetWMWindowType(const WMType type);

	// redrawing options

	void	BufferDrawing(const bool bufferDrawing);

	bool	IsKeepingBufferPixmap() const;
	void	KeepBufferPixmap(const bool keepIt);

	bool	IsUsingPixmapAsBackground() const;
	void	UsePixmapAsBackground(const bool useIt);

	// mouse restrictions

	bool	GrabPointer(JXContainer* obj);
	void	UngrabPointer(JXContainer* obj);

	// utility function for widgets that display text and have shortcuts

	static JIndex	GetULShortcutIndex(const JString& label, const JString* list);

	// called by JXApplication

	void	HandleEvent(const XEvent& xEvent);

	static bool	IsDeleteWindowMessage(const JXDisplay* display, const XEvent& xEvent);
	static bool	IsWMPingMessage(const JXDisplay* display, const XEvent& xEvent);
	static bool	IsSaveYourselfMessage(const JXDisplay* display, const XEvent& xEvent);

	static void	SetDesktopHMargin(const JCoordinate dw);
	static void	SetDesktopVMargin(const JCoordinate dh);

	// called by JXDisplay

	void	Update();
	void	DispatchMouse();

	bool	BeginDrag(JXContainer* obj, const JPoint& ptG,
					  const JXButtonStates& buttonStates,
					  const JXKeyModifiers& modifiers);
	void	EndDrag(JXContainer* obj, const JPoint& ptG,
					const JXButtonStates& buttonStates,
					const JXKeyModifiers& modifiers);

	bool	GetRootChild(Window* rootChild) const;

	void	AcceptSaveYourself(const bool accept);

	static void	AnalyzeWindowManager(JXDisplay* d);

	// called by JXContainer

	bool	GetMouseContainer(JXContainer** window) const;
	void	SetMouseContainer(JXContainer* obj, const JPoint& ptG,
							  const unsigned int state);
	void	RecalcMouseContainer();
	void	SetWantDrag(const bool wantDrag);
	JSize	CountClicks(JXContainer* container, const JPoint& pt);

	void	DispatchCursor();
	void	DisplayXCursor(const JCursorIndex index);

	// called by JXWidget

	void	UpdateForScroll(const JRect& ap, const JRect& src, const JRect& dest);

	void	RegisterFocusWidget(JXWidget* widget);
	void	UnregisterFocusWidget(JXWidget* widget);
	bool	SwitchFocusToWidget(JXWidget* widget);

	bool	InstallShortcut(JXWidget* widget, const int key,
							const JXKeyModifiers& modifiers);
	void	InstallShortcuts(JXWidget* widget, const JString& list);
	void	ClearShortcuts(JXWidget* widget);

	// called by JXWidget and JXSelectionManager

	Window	GetXWindow() const;

	// called by JXTextMenu

	bool	InstallMenuShortcut(JXTextMenu* menu, const JIndex menuItem,
								const int key, const JXKeyModifiers& origModifiers);
	void	ClearMenuShortcut(JXTextMenu* menu, const JIndex menuItem);
	void	ClearAllMenuShortcuts(JXTextMenu* menu);
	void	MenuItemInserted(JXTextMenu* menu, const JIndex newItem);
	void	MenuItemRemoved(JXTextMenu* menu, const JIndex oldItem);

	// called by JXTEBase

	void	SetXIMPosition(const JPoint& ptG);

	// called by JXHintManager

	void	SetCurrentHintManager(JXHintManager* mgr);
	void	DeactivateHint();

	// called by JXDockWidget

	bool	IsDocked() const;
	bool	GetDockWindow(JXWindow** window) const;
	bool	GetDockWidget(JXDockWidget** dock) const;
	bool	Dock(JXDockWidget* dock, const Window parent, const JRect& geom);
	void	Undock();
	void	UndockAllChildWindows();

	bool	HasWindowType() const;
	bool	GetWindowType(JString* type) const;

	void	UndockedPlace(const JCoordinate enclX, const JCoordinate enclY);
	void	UndockedMove(const JCoordinate dx, const JCoordinate dy);

	void	UndockedSetSize(const JCoordinate w, const JCoordinate h,
							const bool ftc = false);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	DrawBackground(JXWindowPainter& p, const JRect& frame) override;

	void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh) override;

	void	FTCAdjustSize(const JCoordinate dw, const JCoordinate dh) override;

private:

	struct Shortcut
	{
		JXWidget*		widget;		// not owned (nullptr if menu   != nullptr)
		JXTextMenu*		menu;		// not owned (nullptr if widget != nullptr)
		JIndex			menuItem;
		int				key;
		unsigned int	state;

		Shortcut()
			:
			widget(nullptr), menu(nullptr), menuItem(0), key(0), state(0)
		{ };

		Shortcut(JXWidget* w, const int k, const unsigned int s)
			:
			widget(w), menu(nullptr), menuItem(0), key(k), state(s)
		{ };

		Shortcut(JXTextMenu* m, const JIndex mi, const int k, const unsigned int s)
			:
			widget(nullptr), menu(m), menuItem(mi), key(k), state(s)
		{ };
	};

	static JListT::CompareResult
		CompareShortcuts(const Shortcut& s1, const Shortcut& s2);

	struct ClickInfo
	{
		JXMouseButton	button;
		Time			time;
		JPoint			pt;

		ClickInfo(const JXMouseButton b, const Time t, const JPoint& p)
			:
			button(b), time(t), pt(p)
		{ };
	};

	struct ChildWindowInfo
	{
		Window		xWindow;
		bool	visible;
		JRect		geom;

		ChildWindowInfo()
			:
			xWindow(None), visible(false)
		{ };

		ChildWindowInfo(const Window w, const bool v, const JRect& r)
			:
			xWindow(w), visible(v), geom(r)
		{ };
	};

private:

	JXWindowDirector*	itsDirector;		// it owns us, we don't own it
	JXIconDirector*		itsIconDir;			// can be nullptr; owned by itsDirector
	JXWindow*			itsMainWindow;		// can be nullptr; uses us for icon
	const bool			itsIsOverlayFlag;

	JXDisplay*	itsDisplay;					// we don't own this
	JXGC*		itsGC;
	Window		itsXWindow;
	Pixmap		itsBufferPixmap;
	XIC			itsXIC;
	JString		itsTitle;
	JString		itsWindowType;
	JColorID	itsBackColor;
	JXImage*	itsIcon;					// can be nullptr
	JRect		itsBounds;
	JPoint		itsDesktopLoc;				// convert to root coordinates
	JPoint		itsWMFrameLoc;				// top left of Window Manager frame
	bool		itsHasBeenVisibleFlag;		// XQuartz placement
	Region		itsUpdateRegion;
	bool		itsIsMappedFlag;
	bool		itsIsIconifiedFlag;
	bool		itsHasFocusFlag;
	bool		itsFocusWhenShowFlag;		// true => RequestFocus() when mapped
	bool		itsBufferDrawingFlag;		// true => draw to pixmap and then copy to window
	bool		itsKeepBufferPixmapFlag;	// true => don't toss itsBufferPixmap
	bool		itsUseBkgdPixmapFlag;		// true => use XSetWindowBackgroundPixmap()
	bool		itsIsDestructingFlag;		// true => in destructor

	bool	itsHasMinSizeFlag;
	JPoint	itsMinSize;
	bool	itsHasMaxSizeFlag;
	JPoint	itsMaxSize;
	JPoint	itsFTCDelta;					// prevent size creep

	JArray<Shortcut>*	itsShortcuts;

	CloseAction		itsCloseAction;			// what to do with WM_DELETE_WINDOW
	JCursorIndex	itsCursorIndex;			// currently displayed cursor

	JXContainer*	itsMouseContainer;		// object that contains the mouse -- not owned
	bool		itsIsDraggingFlag;			// true while dragging (even if not processed)
	bool		itsProcessDragFlag;			// true if send drag to itsMouseContainer
	bool		itsCursorLeftFlag;			// true if mouse left window during drag
	bool		itsCleanAfterBlockFlag;

	JXContainer*	itsButtonPressReceiver;	// receives DispatchMouseEvent() messages
	bool			itsPointerGrabbedFlag;	// true if GrabPointer() in effect
	bool			itsBPRChangedFlag;		// flag for BeginDrag()

	JPtrArray<JXWidget>*	itsFocusList;	// widgets that want focus
	JXWidget*				itsFocusWidget;	// receives key events directly; not owned; can be nullptr
	JXHintManager*			itsCurrHintMgr;	// not owned; can be nullptr; deactivate when key press

	mutable Window	itsRootChild;			// ancestor which is direct child of root window

	// multiple click detection

	JXContainer*	itsPrevMouseContainer;
	ClickInfo		itsFirstClick;
	ClickInfo		itsSecondClick;
	JSize			itsClickCount;

	// docking

	bool				itsIsDockedFlag;
	Window				itsDockXWindow;			// None if not docked
	JXDockWidget*		itsDockWidget;			// can be nullptr, even if docked
	JRect				itsUndockedGeom;
	JPoint				itsUndockedWMFrameLoc;
	JXDockWindowTask*	itsDockingTask;

	JArray<ChildWindowInfo>*	itsChildWindowList;	// nullptr unless has children

	static bool	theAutoDockNewWindowFlag;			// true => check auto-docking settings
	static bool	theFocusFollowsCursorInDockFlag;	// true => automatically set input focus to docked window containing cursor

	// FTC

	JXExpandWindowToFitContentTask*	itsExpandTask;	// nullptr unless waiting for it

private:

	Drawable	PrepareForUpdate();
	void		FinishUpdate(const JRect& rect, const Region region);

	void	HandleEnterNotify(const XCrossingEvent& xEvent);
	void	HandleLeaveNotify(const XCrossingEvent& xEvent);
	void	HandleMotionNotify(const XMotionEvent& xEvent);
	bool	HandleButtonPress(const XButtonEvent& xEvent);
	bool	HandleButtonRelease(const XButtonEvent& xEvent);

	void	HandleFocusIn(const XFocusChangeEvent& xEvent);
	void	HandleFocusOut(const XFocusChangeEvent& xEvent);
	void	HandleKeyPress(const XEvent& keyEvent);
	void	TossKeyRepeatEvents(const unsigned int keycode,
								const unsigned int state,
								const KeySym keySym);

	void	SwitchFocus(const bool backward);
	bool	FindNextFocusWidget(const JIndex origStartIndex,
								JXWidget** focusWidget) const;
	bool	FindPrevFocusWidget(const JIndex origStartIndex,
								JXWidget** focusWidget) const;

	bool	IsShortcut(const KeySym keySym, const unsigned int state);
	bool	InstallShortcut(const Shortcut& s);

	void	UpdateFrame();
	void	UpdateBounds(const JCoordinate w, const JCoordinate h, const bool ftc);
	JPoint	CalcDesktopLocation(const JCoordinate x, const JCoordinate y,
								const JCoordinate direction) const;

	void	HandleMapNotify(const XMapEvent& mapEvent);
	void	HandleUnmapNotify(const XUnmapEvent& unmapEvent);
	void	HandleWMStateChange();
	void	SetWindowStateHint(const int initial_state);

	void	HandleExpose(const XExposeEvent& exposeEvent);
	void	HandleExpose1(const XExposeEvent& exposeEvent);

	void	AdjustTitle();

	void	UpdateChildWindowList(const Window xWindow, const bool add);
	void	SetChildWindowGeometry(const Window xWindow, const JRect& geom);
	void	UpdateChildWindowGeometry();
	void	SetChildWindowVisible(const Window xWindow, const bool visible);

	// used by Hide()

	void		PrivateHide();
	static Bool	GetNextMapNotifyEvent(Display* display, XEvent* event, char* arg);

	// used by CheckForMapOrExpose()

	static Bool	GetNextMapOrExposeEvent(Display* display, XEvent* event, char* arg);

	// used by HandleExpose()

	static Bool	GetNextExposeEvent(Display* display, XEvent* event, char* arg);

	// used by AnalyzeWindowManager

	static void	WaitForWM(JXDisplay* d, JXWindow* w);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kIconified;
	static const JUtf8Byte* kDeiconified;
	static const JUtf8Byte* kMapped;
	static const JUtf8Byte* kUnmapped;
	static const JUtf8Byte* kRaised;
	static const JUtf8Byte* kDocked;
	static const JUtf8Byte* kUndocked;
	static const JUtf8Byte* kMinSizeChanged;
	static const JUtf8Byte* kTitleChanged;

	class Iconified : public JBroadcaster::Message
		{
		public:

			Iconified()
				:
				JBroadcaster::Message(kIconified)
				{ };
		};

	class Deiconified : public JBroadcaster::Message
		{
		public:

			Deiconified()
				:
				JBroadcaster::Message(kDeiconified)
				{ };
		};

	class Mapped : public JBroadcaster::Message
		{
		public:

			Mapped()
				:
				JBroadcaster::Message(kMapped)
				{ };
		};

	class Unmapped : public JBroadcaster::Message
		{
		public:

			Unmapped()
				:
				JBroadcaster::Message(kUnmapped)
				{ };
		};

	class Raised : public JBroadcaster::Message
		{
		public:

			Raised()
				:
				JBroadcaster::Message(kRaised)
				{ };
		};

	class Docked : public JBroadcaster::Message
		{
		public:

			Docked()
				:
				JBroadcaster::Message(kDocked)
				{ };
		};

	class Undocked : public JBroadcaster::Message
		{
		public:

			Undocked()
				:
				JBroadcaster::Message(kUndocked)
				{ };
		};

	class MinSizeChanged : public JBroadcaster::Message
		{
		public:

			MinSizeChanged()
				:
				JBroadcaster::Message(kMinSizeChanged)
				{ };
		};

	class TitleChanged : public JBroadcaster::Message
		{
		public:

			TitleChanged()
				:
				JBroadcaster::Message(kTitleChanged)
				{ };
		};
};


/******************************************************************************
 GetTitle

 ******************************************************************************/

inline const JString&
JXWindow::GetTitle()
	const
{
	return itsTitle;
}

/******************************************************************************
 IsIconified

 ******************************************************************************/

inline bool
JXWindow::IsIconified()
	const
{
	return itsIsIconifiedFlag;
}

/******************************************************************************
 IsDocked

 ******************************************************************************/

inline bool
JXWindow::IsDocked()
	const
{
	return itsIsDockedFlag;
}

/******************************************************************************
 GetDockWidget

 ******************************************************************************/

inline bool
JXWindow::GetDockWidget
	(
	JXDockWidget** dock
	)
	const
{
	*dock = itsDockWidget;
	return itsDockWidget != nullptr;
}

/******************************************************************************
 Close action

 ******************************************************************************/

inline JXWindow::CloseAction
JXWindow::GetCloseAction()
	const
{
	return itsCloseAction;
}

inline void
JXWindow::SetCloseAction
	(
	const CloseAction closeAction
	)
{
	itsCloseAction = closeAction;
}

/******************************************************************************
 Focus when show

 ******************************************************************************/

inline bool
JXWindow::WillFocusWhenShown()
	const
{
	return itsFocusWhenShowFlag;
}

inline void
JXWindow::ShouldFocusWhenShow
	(
	const bool focusWhenShow
	)
{
	itsFocusWhenShowFlag = focusWhenShow;
}

/******************************************************************************
 HasFocus

	Returns true if keypresses go to this window.

 ******************************************************************************/

inline bool
JXWindow::HasFocus()
	const
{
	return itsHasFocusFlag;
}

/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXWindow::GetDisplay()
	const
{
	return itsDisplay;
}

/******************************************************************************
 GetXWindow

 ******************************************************************************/

inline Window
JXWindow::GetXWindow()
	const
{
	return itsXWindow;
}

/******************************************************************************
 HasWindowType

 ******************************************************************************/

inline bool
JXWindow::HasWindowType()
	const
{
	return !itsWindowType.IsEmpty();
}

/******************************************************************************
 GetWindowType

 ******************************************************************************/

inline bool
JXWindow::GetWindowType
	(
	JString* type
	)
	const
{
	*type = itsWindowType;
	return HasWindowType();
}

/******************************************************************************
 GetDirector

 ******************************************************************************/

inline JXWindowDirector*
JXWindow::GetDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetMouseContainer

 ******************************************************************************/

inline bool
JXWindow::GetMouseContainer
	(
	JXContainer** container
	)
	const
{
	*container = itsMouseContainer;
	return *container != nullptr;
}

/******************************************************************************
 RecalcMouseContainer

	If the user is dragging, the mouse container is the dragging widget, and
	this should not change until the mouse is released.

 ******************************************************************************/

inline void
JXWindow::RecalcMouseContainer()
{
	if (!itsIsDraggingFlag)
		{
		DispatchMouse();
		}
}

/******************************************************************************
 GetFocusWidget

	Returns true if a widget has focus.

 ******************************************************************************/

inline bool
JXWindow::GetFocusWidget
	(
	JXWidget** widget
	)
	const
{
	*widget = itsFocusWidget;
	return itsFocusWidget != nullptr;
}

/******************************************************************************
 GetBackColor

 ******************************************************************************/

inline JColorID
JXWindow::GetBackColor()
	const
{
	return itsBackColor;
}

/******************************************************************************
 SetWantDrag

 ******************************************************************************/

inline void
JXWindow::SetWantDrag
	(
	const bool wantDrag
	)
{
	itsProcessDragFlag = wantDrag;
}

/******************************************************************************
 IsDragging

 ******************************************************************************/

inline bool
JXWindow::IsDragging()
	const
{
	return itsIsDraggingFlag;
}

/******************************************************************************
 GetDesktopLocation

 ******************************************************************************/

inline JPoint
JXWindow::GetDesktopLocation()
	const
{
	return itsWMFrameLoc;
}

/******************************************************************************
 GetMinSize

	Doesn't return JPoint& to avoid locking us into a particular implementation.

 ******************************************************************************/

inline JPoint
JXWindow::GetMinSize()
	const
{
	return itsMinSize;
}

/******************************************************************************
 GetMaxSize

	Doesn't return JPoint& to avoid locking us into a particular implementation.

 ******************************************************************************/

inline JPoint
JXWindow::GetMaxSize()
	const
{
	return itsMaxSize;
}

/******************************************************************************
 LockCurrentSize

 ******************************************************************************/

inline void
JXWindow::LockCurrentSize()
{
	SetMinSize(itsBounds.width(), itsBounds.height());
	SetMaxSize(itsBounds.width(), itsBounds.height());
}

inline void
JXWindow::LockCurrentMinSize()
{
	SetMinSize(itsBounds.width(), itsBounds.height());
}

inline void
JXWindow::LockCurrentMaxSize()
{
	SetMaxSize(itsBounds.width(), itsBounds.height());
}

/******************************************************************************
 GlobalToRoot

 ******************************************************************************/

inline JPoint
JXWindow::GlobalToRoot
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x + itsDesktopLoc.x, y + itsDesktopLoc.y);
}

inline JPoint
JXWindow::GlobalToRoot
	(
	const JPoint& pt
	)
	const
{
	return (pt + itsDesktopLoc);
}

/******************************************************************************
 RootToGlobal

 ******************************************************************************/

inline JPoint
JXWindow::RootToGlobal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x - itsDesktopLoc.x, y - itsDesktopLoc.y);
}

inline JPoint
JXWindow::RootToGlobal
	(
	const JPoint& pt
	)
	const
{
	return (pt - itsDesktopLoc);
}

/******************************************************************************
 Buffer pixmap control

	Do NOT use these feature unless you feel like wasting an enormous amount
	of server memory.  It is not appropriate for anything other than a
	very temporary window.

	Keeping the buffer pixmap means that redraws do not have to make a
	server round-trip to create a new pixmap each time.

	Using the pixmap as the window background means that updates caused by
	expose events are instantaneous because the server knows what to display.

 ******************************************************************************/

inline bool
JXWindow::IsKeepingBufferPixmap()
	const
{
	return itsKeepBufferPixmapFlag;
}

inline void
JXWindow::KeepBufferPixmap
	(
	const bool keepIt
	)
{
	itsKeepBufferPixmapFlag = keepIt;
}

inline bool
JXWindow::IsUsingPixmapAsBackground()
	const
{
	return itsUseBkgdPixmapFlag;
}

inline void
JXWindow::UsePixmapAsBackground
	(
	const bool useIt
	)
{
	itsUseBkgdPixmapFlag = useIt;
	if (itsUseBkgdPixmapFlag)
		{
		BufferDrawing(true);
		}
	else
		{
		SetBackColor(itsBackColor);
		}
}

/******************************************************************************
 SetCurrentHintManager

 ******************************************************************************/

inline void
JXWindow::SetCurrentHintManager
	(
	JXHintManager* mgr
	)
{
	itsCurrHintMgr = mgr;
}

/******************************************************************************
 Docking strategy (static)

 ******************************************************************************/

inline bool
JXWindow::WillAutoDockNewWindows()
{
	return theAutoDockNewWindowFlag;
}

inline void
JXWindow::ShouldAutoDockNewWindows
	(
	const bool autoDock
	)
{
	theAutoDockNewWindowFlag = autoDock;
}

/******************************************************************************
 Docking strategy (static)

 ******************************************************************************/

inline bool
JXWindow::WillFocusFollowCursorInDock()
{
	return theFocusFollowsCursorInDockFlag;
}

inline void
JXWindow::ShouldFocusFollowCursorInDock
	(
	const bool focusFollowsCursor
	)
{
	theFocusFollowsCursorInDockFlag = focusFollowsCursor;
}

#endif
