/******************************************************************************
 JXWindow.h

	Interface for the JXWindow class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWindow
#define _H_JXWindow

#include <JXContainer.h>
#include <JXCursor.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <JString.h>

class JXDisplay;
class JXGC;
class JXColormap;
class JXImage;
class JXWindowDirector;
class JXWidget;
class JXTextMenu;
class JXIconDirector;
class JXWindowIcon;
class JXHintManager;
class JXDockWidget;
class JXDockWindowTask;

class JXWindow : public JXContainer
{
	friend class JXDockWindowTask;

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
			 const JCharacter* title, const JBoolean isOverlay = kJFalse);

	virtual ~JXWindow();

	JXDisplay*	GetDisplay() const;
	JXColormap*	GetColormap() const;

	const JString&	GetTitle() const;
	void			SetTitle(const JCharacter* title);

	JBoolean	Close();
	CloseAction	GetCloseAction() const;
	void		SetCloseAction(const CloseAction closeAction);

	void	SetTransientFor(const JXWindowDirector* director);
	void	SetWMClass(const JCharacter* c_class, const JCharacter* instance);
	void	HideFromTaskbar();

	virtual void	Show();
	virtual void	Hide();
	void			Raise(const JBoolean grabKeyboardFocus = kJTrue);
	void			Lower();
	virtual void	Refresh() const;
	void			RefreshRect(const JRect& rect) const;
	virtual void	Redraw() const;
	void			RedrawRect(const JRect& rect) const;
	void			CheckForMapOrExpose();

	void		Iconify();
	void		Deiconify();
	JBoolean	IsIconified() const;

	JBoolean	GetFocusWidget(JXWidget** widget) const;
	JBoolean	SwitchFocusToFirstWidget();
	JBoolean	SwitchFocusToFirstWidgetWithAncestor(JXContainer* ancestor);
	JBoolean	OKToUnfocusCurrentWidget() const;
	JBoolean	UnfocusCurrentWidget();
	void		KillFocus();

	virtual void	Activate();
	virtual void	Resume();
	JBoolean		WillFocusWhenShown() const;
	void			ShouldFocusWhenShow(const JBoolean focusWhenShow);
	void			RequestFocus();
	JBoolean		HasFocus() const;

	virtual JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const;
	virtual JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const;

	JPoint	GlobalToRoot(const JCoordinate x, const JCoordinate y) const;
	JPoint	GlobalToRoot(const JPoint& pt) const;
	JRect	GlobalToRoot(const JRect& r) const;
	JPoint	RootToGlobal(const JCoordinate x, const JCoordinate y) const;
	JPoint	RootToGlobal(const JPoint& pt) const;
	JRect	RootToGlobal(const JRect& r) const;

	virtual void	Place(const JCoordinate enclX, const JCoordinate enclY);
	virtual void	Move(const JCoordinate dx, const JCoordinate dy);
	virtual void	SetSize(const JCoordinate w, const JCoordinate h);
	virtual void	AdjustSize(const JCoordinate dw, const JCoordinate dh);
	void			CenterOnScreen();
	void			PlaceAsDialogWindow();

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

	virtual JRect	GetBoundsGlobal() const;
	virtual JRect	GetFrameGlobal() const;
	virtual JRect	GetApertureGlobal() const;
	JPoint			GetDesktopLocation() const;

	void		ReadGeometry(std::istream& input, const JBoolean skipDocking = kJFalse);
	void		WriteGeometry(std::ostream& output) const;
	static void	SkipGeometry(std::istream& input);

	void	ReadGeometry(const JString& data, const JBoolean skipDocking = kJFalse);
	void	WriteGeometry(JString* data) const;

	JXWindowDirector*	GetDirector() const;

	JColorIndex		GetBackColor() const;
	void			SetBackColor(const JColorIndex color);
	void			SetIcon(JXImage* icon);
	JXWindowIcon*	SetIcon(JXImage* normalIcon, JXImage* dropIcon);
	JBoolean		GetIconWidget(JXWindowIcon** widget) const;

	JBoolean	IsDragging() const;
	void		PrintWindowConfig();

	static JBoolean	WillAutoDockNewWindows();
	static void		ShouldAutoDockNewWindows(const JBoolean autoDock);

	static JBoolean	WillFocusFollowCursorInDock();
	static void		ShouldFocusFollowCursorInDock(const JBoolean focusFollowsCursor);

	void	SetWMWindowType(const WMType type);

	// redrawing options

	void		BufferDrawing(const JBoolean bufferDrawing);

	JBoolean	IsKeepingBufferPixmap() const;
	void		KeepBufferPixmap(const JBoolean keepIt);

	JBoolean	IsUsingPixmapAsBackground() const;
	void		UsePixmapAsBackground(const JBoolean useIt);

	// mouse restrictions

	JBoolean	GrabPointer(JXContainer* obj);
	void		UngrabPointer(JXContainer* obj);

	// utility function for widgets that display text and have shortcuts

	static JIndex	GetULShortcutIndex(const JString& label, const JString* list);

	// called by JXApplication

	void	HandleEvent(const XEvent& xEvent);

	static JBoolean	IsDeleteWindowMessage(const JXDisplay* display, const XEvent& xEvent);
	static JBoolean	IsWMPingMessage(const JXDisplay* display, const XEvent& xEvent);
	static JBoolean	IsSaveYourselfMessage(const JXDisplay* display, const XEvent& xEvent);

	static void	SetDesktopHMargin(const JCoordinate dw);
	static void	SetDesktopVMargin(const JCoordinate dh);

	// called by JXDisplay

	void	Update();
	void	ColormapChanged(JXColormap* colormap);
	void	DispatchMouse();

	JBoolean	BeginDrag(JXContainer* obj, const JPoint& ptG,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers);
	void		EndDrag(JXContainer* obj, const JPoint& ptG,
						const JXButtonStates& buttonStates,
						const JXKeyModifiers& modifiers);

	JBoolean	GetRootChild(Window* rootChild) const;

	void	AcceptSaveYourself(const JBoolean accept);

	static void	AnalyzeWindowManager(JXDisplay* d);

	// called by JXContainer

	JBoolean	GetMouseContainer(JXContainer** window) const;
	void		SetMouseContainer(JXContainer* obj, const JPoint& ptG,
								  const unsigned int state);
	void		RecalcMouseContainer();
	void		SetWantDrag(const JBoolean wantDrag);
	JSize		CountClicks(JXContainer* container, const JPoint& pt);

	void	DispatchCursor();
	void	DisplayXCursor(const JCursorIndex index);

	// called by JXWidget

	void	UpdateForScroll(const JRect& ap, const JRect& src, const JRect& dest);

	void		RegisterFocusWidget(JXWidget* widget);
	void		UnregisterFocusWidget(JXWidget* widget);
	JBoolean	SwitchFocusToWidget(JXWidget* widget);

	JBoolean	InstallShortcut(JXWidget* widget, const int key,
								const JXKeyModifiers& modifiers);
	void		InstallShortcuts(JXWidget* widget, const JCharacter* list);
	void		ClearShortcuts(JXWidget* widget);

	// called by JXWidget and JXSelectionManager

	Window	GetXWindow() const;

	// called by JXTextMenu

	JBoolean	InstallMenuShortcut(JXTextMenu* menu, const JIndex menuItem,
									const int key, const JXKeyModifiers& origModifiers);
	void		ClearMenuShortcut(JXTextMenu* menu, const JIndex menuItem);
	void		ClearAllMenuShortcuts(JXTextMenu* menu);
	void		MenuItemInserted(JXTextMenu* menu, const JIndex newItem);
	void		MenuItemRemoved(JXTextMenu* menu, const JIndex oldItem);

	// called by JXHintManager

	void	SetCurrentHintManager(JXHintManager* mgr);
	void	DeactivateHint();

	// called by JXDockWidget

	JBoolean	IsDocked() const;
	JBoolean	GetDockWindow(JXWindow** window) const;
	JBoolean	GetDockWidget(JXDockWidget** dock) const;
	JBoolean	Dock(JXDockWidget* dock, const Window parent, const JRect& geom);
	void		Undock();
	void		UndockAllChildWindows();

	JBoolean	HasWindowType() const;
	JBoolean	GetWindowType(const JCharacter** type) const;

	void	UndockedPlace(const JCoordinate enclX, const JCoordinate enclY);
	void	UndockedMove(const JCoordinate dx, const JCoordinate dy);

	void	UndockedSetSize(const JCoordinate w, const JCoordinate h);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void	DrawBackground(JXWindowPainter& p, const JRect& frame);

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh);

private:

	struct Shortcut
	{
		JXWidget*		widget;		// not owned (NULL if menu   != NULL)
		JXTextMenu*		menu;		// not owned (NULL if widget != NULL)
		JIndex			menuItem;
		int				key;
		unsigned int	state;

		Shortcut()
			:
			widget(NULL), menu(NULL), menuItem(0), key(0), state(0)
		{ };

		Shortcut(JXWidget* w, const int k, const unsigned int s)
			:
			widget(w), menu(NULL), menuItem(0), key(k), state(s)
		{ };

		Shortcut(JXTextMenu* m, const JIndex mi, const int k, const unsigned int s)
			:
			widget(NULL), menu(m), menuItem(mi), key(k), state(s)
		{ };
	};

	static JOrderedSetT::CompareResult
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
		JBoolean	visible;
		JRect		geom;

		ChildWindowInfo()
			:
			xWindow(None), visible(kJFalse)
		{ };

		ChildWindowInfo(const Window w, const JBoolean v, const JRect& r)
			:
			xWindow(w), visible(v), geom(r)
		{ };
	};

private:

	JXWindowDirector*	itsDirector;		// it owns us, we don't own it
	JXIconDirector*		itsIconDir;			// can be NULL; owned by itsDirector
	JXWindow*			itsMainWindow;		// can be NULL; uses us for icon
	const JBoolean		itsIsOverlayFlag;

	JXDisplay*	itsDisplay;					// we don't own this
	JXColormap*	itsColormap;
	JXGC*		itsGC;
	Window		itsXWindow;
	Pixmap		itsBufferPixmap;
	JString		itsTitle;
	JString		itsWindowType;
	JColorIndex	itsBackColor;
	JXImage*	itsIcon;					// can be NULL
	JRect		itsBounds;
	JPoint		itsDesktopLoc;				// convert to root coordinates
	JPoint		itsWMFrameLoc;				// top left of Window Manager frame
	JBoolean	itsHasBeenVisibleFlag;		// XQuartz placement
	Region		itsUpdateRegion;
	JBoolean	itsIsMappedFlag;
	JBoolean	itsIsIconifiedFlag;
	JBoolean	itsHasFocusFlag;
	JBoolean	itsFocusWhenShowFlag;		// kJTrue => RequestFocus() when mapped
	JBoolean	itsBufferDrawingFlag;		// kJTrue => draw to pixmap and then copy to window
	JBoolean	itsKeepBufferPixmapFlag;	// kJTrue => don't toss itsBufferPixmap
	JBoolean	itsUseBkgdPixmapFlag;		// kJTrue => use XSetWindowBackgroundPixmap()
	JBoolean	itsIsDestructingFlag;		// kJTrue => in destructor

	JBoolean	itsHasMinSizeFlag;
	JPoint		itsMinSize;
	JBoolean	itsHasMaxSizeFlag;
	JPoint		itsMaxSize;

	JArray<Shortcut>*	itsShortcuts;

	CloseAction		itsCloseAction;				// what to do with WM_DELETE_WINDOW
	JCursorIndex	itsCursorIndex;				// currently displayed cursor

	JXContainer*	itsMouseContainer;			// object that contains the mouse -- not owned
	JBoolean		itsIsDraggingFlag;			// kJTrue while dragging (even if not processed)
	JBoolean		itsProcessDragFlag;			// kJTrue if send drag to itsMouseContainer
	JBoolean		itsCursorLeftFlag;			// kJTrue if mouse left window during drag
	JBoolean		itsCleanAfterBlockFlag;

	JXContainer*	itsButtonPressReceiver;		// receives DispatchMouseEvent() messages
	JBoolean		itsPointerGrabbedFlag;		// kJTrue if GrabPointer() in effect
	JBoolean		itsBPRChangedFlag;			// flag for BeginDrag()

	JPtrArray<JXWidget>*	itsFocusList;		// widgets that want focus
	JXWidget*				itsFocusWidget;		// receives key events directly; not owned; can be NULL
	JXHintManager*			itsCurrHintMgr;		// not owned; can be NULL; deactivate when key press

	mutable Window	itsRootChild;				// ancestor which is direct child of root window

	// multiple click detection

	JXContainer*	itsPrevMouseContainer;
	ClickInfo		itsFirstClick;
	ClickInfo		itsSecondClick;
	JSize			itsClickCount;

	// docking

	JBoolean			itsIsDockedFlag;
	Window				itsDockXWindow;			// None if not docked
	JXDockWidget*		itsDockWidget;			// can be NULL, even if docked
	JRect				itsUndockedGeom;
	JPoint				itsUndockedWMFrameLoc;
	JXDockWindowTask*	itsDockingTask;

	JArray<ChildWindowInfo>*	itsChildWindowList;		// NULL unless has children

	static JBoolean	theAutoDockNewWindowFlag;			// kJTrue => check auto-docking settings
	static JBoolean	theFocusFollowsCursorInDockFlag;	// kJTrue => automatically set input focus to docked window containing cursor

private:

	Drawable	PrepareForUpdate();
	void		FinishUpdate(const JRect& rect, const Region region);

	void		HandleEnterNotify(const XCrossingEvent& xEvent);
	void		HandleLeaveNotify(const XCrossingEvent& xEvent);
	void		HandleMotionNotify(const XMotionEvent& xEvent);
	JBoolean	HandleButtonPress(const XButtonEvent& xEvent);
	JBoolean	HandleButtonRelease(const XButtonEvent& xEvent);

	void		HandleFocusIn(const XFocusChangeEvent& xEvent);
	void		HandleFocusOut(const XFocusChangeEvent& xEvent);
	void		HandleKeyPress(const XEvent& keyEvent);
	void		TossKeyRepeatEvents(const unsigned int keycode,
									const unsigned int state,
									const KeySym keySym);

	void		SwitchFocus(const JBoolean backward);
	JBoolean	FindNextFocusWidget(const JIndex origStartIndex,
									JXWidget** focusWidget) const;
	JBoolean	FindPrevFocusWidget(const JIndex origStartIndex,
									JXWidget** focusWidget) const;

	JBoolean	IsShortcut(const KeySym keySym, const unsigned int state);
	JBoolean	InstallShortcut(const Shortcut& s);

	void	UpdateFrame();
	void	UpdateBounds(const JCoordinate w, const JCoordinate h);
	JPoint	CalcDesktopLocation(const JCoordinate x, const JCoordinate y,
								const JCoordinate direction) const;

	void	HandleMapNotify(const XMapEvent& mapEvent);
	void	HandleUnmapNotify(const XUnmapEvent& unmapEvent);
	void	HandleWMStateChange();
	void	SetWindowStateHint(const int initial_state);

	void	HandleExpose(const XExposeEvent& exposeEvent);
	void	HandleExpose1(const XExposeEvent& exposeEvent);

	void	AdjustTitle();

	void	UpdateChildWindowList(const Window xWindow, const JBoolean add);
	void	SetChildWindowGeometry(const Window xWindow, const JRect& geom);
	void	UpdateChildWindowGeometry();
	void	SetChildWindowVisible(const Window xWindow, const JBoolean visible);

	// used by Hide()

	void		PrivateHide();
	static Bool	GetNextMapNotifyEvent(Display* display, XEvent* event, char* arg);

	// used by CheckForMapOrExpose()

	static Bool	GetNextMapOrExposeEvent(Display* display, XEvent* event, char* arg);

	// used by HandleExpose()

	static Bool	GetNextExposeEvent(Display* display, XEvent* event, char* arg);

	// used by AnalyzeWindowManager

	static void	WaitForWM(JXDisplay* d, JXWindow* w);

	// not allowed

	JXWindow(const JXWindow& source);
	const JXWindow& operator=(const JXWindow& source);

public:

	// JBroadcaster messages

	static const JCharacter* kIconified;
	static const JCharacter* kDeiconified;
	static const JCharacter* kMapped;
	static const JCharacter* kUnmapped;
	static const JCharacter* kRaised;
	static const JCharacter* kDocked;
	static const JCharacter* kUndocked;
	static const JCharacter* kMinSizeChanged;
	static const JCharacter* kTitleChanged;

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

inline JBoolean
JXWindow::IsIconified()
	const
{
	return itsIsIconifiedFlag;
}

/******************************************************************************
 IsDocked

 ******************************************************************************/

inline JBoolean
JXWindow::IsDocked()
	const
{
	return itsIsDockedFlag;
}

/******************************************************************************
 GetDockWidget

 ******************************************************************************/

inline JBoolean
JXWindow::GetDockWidget
	(
	JXDockWidget** dock
	)
	const
{
	*dock = itsDockWidget;
	return JI2B( itsDockWidget != NULL );
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

inline JBoolean
JXWindow::WillFocusWhenShown()
	const
{
	return itsFocusWhenShowFlag;
}

inline void
JXWindow::ShouldFocusWhenShow
	(
	const JBoolean focusWhenShow
	)
{
	itsFocusWhenShowFlag = focusWhenShow;
}

/******************************************************************************
 HasFocus

	Returns kJTrue if keypresses go to this window.

 ******************************************************************************/

inline JBoolean
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
 GetColormap

 ******************************************************************************/

inline JXColormap*
JXWindow::GetColormap()
	const
{
	return itsColormap;
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

inline JBoolean
JXWindow::HasWindowType()
	const
{
	return !JStringEmpty(itsWindowType);
}

/******************************************************************************
 GetWindowType

 ******************************************************************************/

inline JBoolean
JXWindow::GetWindowType
	(
	const JCharacter** type
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

inline JBoolean
JXWindow::GetMouseContainer
	(
	JXContainer** container
	)
	const
{
	*container = itsMouseContainer;
	return JConvertToBoolean( *container != NULL );
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

	Returns kJTrue if a widget has focus.

 ******************************************************************************/

inline JBoolean
JXWindow::GetFocusWidget
	(
	JXWidget** widget
	)
	const
{
	*widget = itsFocusWidget;
	return JConvertToBoolean( itsFocusWidget != NULL );
}

/******************************************************************************
 GetBackColor

 ******************************************************************************/

inline JColorIndex
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
	const JBoolean wantDrag
	)
{
	itsProcessDragFlag = wantDrag;
}

/******************************************************************************
 IsDragging

 ******************************************************************************/

inline JBoolean
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

inline JBoolean
JXWindow::IsKeepingBufferPixmap()
	const
{
	return itsKeepBufferPixmapFlag;
}

inline void
JXWindow::KeepBufferPixmap
	(
	const JBoolean keepIt
	)
{
	itsKeepBufferPixmapFlag = keepIt;
}

inline JBoolean
JXWindow::IsUsingPixmapAsBackground()
	const
{
	return itsUseBkgdPixmapFlag;
}

inline void
JXWindow::UsePixmapAsBackground
	(
	const JBoolean useIt
	)
{
	itsUseBkgdPixmapFlag = useIt;
	if (itsUseBkgdPixmapFlag)
		{
		BufferDrawing(kJTrue);
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

inline JBoolean
JXWindow::WillAutoDockNewWindows()
{
	return theAutoDockNewWindowFlag;
}

inline void
JXWindow::ShouldAutoDockNewWindows
	(
	const JBoolean autoDock
	)
{
	theAutoDockNewWindowFlag = autoDock;
}

/******************************************************************************
 Docking strategy (static)

 ******************************************************************************/

inline JBoolean
JXWindow::WillFocusFollowCursorInDock()
{
	return theFocusFollowsCursorInDockFlag;
}

inline void
JXWindow::ShouldFocusFollowCursorInDock
	(
	const JBoolean focusFollowsCursor
	)
{
	theFocusFollowsCursorInDockFlag = focusFollowsCursor;
}

#endif
