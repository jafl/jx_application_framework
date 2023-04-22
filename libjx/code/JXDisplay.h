/******************************************************************************
 JXDisplay.h

	Interface for the JXDisplay class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDisplay
#define _H_JXDisplay

#include "JXCursor.h"
#include "jXEventUtil.h"
#include "JXKeyModifiers.h"	// for kJXKeyModifierMapCount
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/JString.h>

class JFontManager;
class JXGC;
class JXColorManager;
class JXWindow;
class JXContainer;
class JXFontManager;
class JXSelectionManager;
class JXDNDManager;
class JXMenuManager;
class JXWDManager;
class JXImageCache;
class JXButtonStates;

class JXDisplay : virtual public JBroadcaster
{
public:

	enum
	{
		kWMStateXAtomIndex,
		kWMClientMachineXAtomIndex,
		kWMProtocolsXAtomIndex,
		kDeleteWindowXAtomIndex,
		kSaveYourselfXAtomIndex,
		kWMPingXAtomIndex,
		kWMPidXAtomIndex,
		kWMUserTimeXAtomIndex,
		kWMActiveWindowXAtomIndex,
		kWMDesktopXAtomIndex,
		kWMCurrentDesktopXAtomIndex,

		kWMWindowTypeXAtomIndex,
		kWMNormalTypeXAtomIndex,
		kWMDialogTypeXAtomIndex,
		kWMPulldownMenuTypeXAtomIndex,
		kWMPopupMenuTypeXAtomIndex,
		kWMTooltipTypeXAtomIndex,

		kJXWMBehaviorV0XAtomIndex,

		kStandardXAtomCount
	};

	// filled in by JXWindow::AnalyzeWindowManager

	struct WMBehavior
	{
		bool	desktopMapsWindowsFlag;	// windows on virtual desktops are unmapped
		bool	frameCompensateFlag;	// compensate for wm frame when placing
		JPoint	reshowOffset;			// re-showing a window also moves it

		WMBehavior()
			:
			desktopMapsWindowsFlag(true),
			frameCompensateFlag(false)
		{ };

		bool	Load(JXDisplay* display);
		void	Save(JXDisplay* display) const;

		private:

		bool	Read(std::istream& input, const JFileVersion vers);
		void	WriteV0(std::ostream& output) const;
	};

public:

	static bool	Create(const JString& displayName, JXDisplay** display);

	~JXDisplay() override;

	bool	Close();

	operator Display*() const;

	const JString&	GetName() const;
	Display*		GetXDisplay() const;
	JXColorManager*	GetColorManager() const;
	JSize			GetMaxStringByteCount() const;
	bool			IsMacOS() const;

	JFontManager*		GetFontManager() const;
	JXFontManager*		GetXFontManager() const;
	JXSelectionManager*	GetSelectionManager() const;
	JXDNDManager*		GetDNDManager() const;
	JXMenuManager*		GetMenuManager() const;
	bool				GetWDManager(JXWDManager** mgr) const;
	JXImageCache*		GetImageCache() const;

	JCursorIndex	CreateBuiltInCursor(const JUtf8Byte* name,
										const unsigned int shape);
	JCursorIndex	CreateCustomCursor(const JUtf8Byte* name,
									   const JXCursor& cursor);
	bool			GetCursor(const JUtf8Byte* name,
							  JCursorIndex* index) const;
	void			DisplayCursorInAllWindows(const JCursorIndex index);

	void	Beep() const;

	void	RaiseAllWindows();
	void	HideAllWindows();
	void	UndockAllWindows();
	void	CloseAllOtherWindows(JXWindow* window);

	// These are the current values (hot from the X server via a query)

	void	GetCurrentButtonKeyState(JXButtonStates* buttonStates,
									 JXKeyModifiers* modifiers) const;

	// These are the values received in the latest event

	const JXButtonStates&	GetLatestButtonStates() const;
	const JXKeyModifiers&	GetLatestKeyModifiers() const;

	bool	KeysymToModifier(const KeySym keysym, JIndex* modifierIndex) const;
	bool	KeycodeToModifier(const KeyCode keycode, JIndex* modifierIndex) const;

	// XFlush() and XSync()

	void	Flush() const;
	void	Synchronize() const;

	void	SendXEvent(const Window xWindow, XEvent* xEvent, const long mask = 0) const;

	// useful for Drag-And-Drop, and for Menu objects

	bool	FindMouseContainer(JXContainer** obj, Window* xWindow = nullptr,
								   JPoint* ptG = nullptr, JPoint* ptR = nullptr) const;
	bool	FindMouseContainer(const JXContainer* coordOwner,
								   const JPoint& pt, JXContainer** obj,
								   Window* xWindow = nullptr,
								   JPoint* ptG = nullptr, JPoint* ptR = nullptr) const;

	// called by JXApplication

	void	HandleEvent(const XEvent& xEvent, const Time currentTime);
	void	Idle(const Time currentTime);
	void	Update();
	void	DispatchMouse();
	void	DispatchCursor();

	void	ShrinkDisplayBoundsToActiveScreen();
	void	RestoreDisplayBounds();

	// called by JXWindow

	void	WindowCreated(JXWindow* window, const Window xWindow);
	void	WindowDeleted(JXWindow* window);
	void	WindowNeedsUpdate(JXWindow* window);

	bool	GetMouseContainer(JXWindow** window) const;
	void		SetMouseContainer(JXWindow* window);
	bool	GetMouseGrabber(JXWindow** window) const;
	void		SetMouseGrabber(JXWindow* window);

	Cursor		GetXCursorID(const JCursorIndex index) const;

	static bool	WindowExists(JXDisplay* display, const Display* xDisplay,
								 const Window xWindow);

	Atom	GetWMProtocolsXAtom() const;
	Atom	GetWMClientMachineXAtom() const;
	Atom	GetDeleteWindowXAtom() const;
	Atom	GetWMPingXAtom() const;
	Atom	GetWMPidXAtom() const;
	Atom	GetWMUserTimeXAtom() const;
	Atom	GetWMActiveWindowXAtom() const;
	Atom	GetSaveYourselfXAtom() const;
	Atom	GetWMStateXAtom() const;
	Atom	GetWMDesktopXAtom() const;
	Atom	GetWMCurrentDesktopXAtom() const;
	Atom	GetWMWindowTypeXAtom() const;
	Atom	GetWMWindowTypeXAtom(const JIndex type) const;

	const WMBehavior&	GetWMBehavior() const;
	void				SetWMBehavior(const WMBehavior& behavior);

	// called by Menu objects

	bool	SwitchDrag(JXContainer* fromObj, const JPoint& fromObjPt,
						   const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers,
						   JXContainer* toObj);

	bool	GetKeyboardGrabber(JXWindow** window) const;
	void		SetKeyboardGrabber(JXWindow* window);

	// called by JXWDManager, etc.

	JXWDManager*	GetWDManager() const;
	void			SetWDManager(JXWDManager* mgr);		// only call once

	// called by various JX objects

	int		GetScreen() const;
	Visual*	GetDefaultVisual() const;
	JSize	GetDepth() const;
	JSize	GetHorizResolution() const;
	JSize	GetVertResolution() const;

	Window		GetRootWindow() const;
	JRect		GetBounds() const;
	const JXGC*	GetDefaultGC() const;

	Time	GetLastEventTime() const;

	Atom	RegisterXAtom(const JUtf8Byte* name);
	void	RegisterXAtoms(const JSize count, const JUtf8Byte** name, Atom* atom);

	bool	FindXWindow(const Window xWindow, JXWindow** window) const;

	const int*	GetJXKeyModifierMapping() const;
	XIM			GetXIM() const;

	// installed by JXApplication

	static void	CheckForXErrors();
	static int	JXErrorHandler(Display* xDisplay, XErrorEvent* error);

protected:

	JXDisplay(const JString& displayName, Display* xDisplay);

private:

	struct WindowInfo
	{
		JXWindow*	window;		// not owned
		Window		xWindow;

		WindowInfo()
			:
			window(nullptr), xWindow(0)
		{ };

		WindowInfo(JXWindow* w, const Window xw)
			:
			window(w), xWindow(xw)
		{ };
	};

	static std::weak_ordering
		CompareXWindows(const WindowInfo& info1, const WindowInfo& info2);

	struct CursorInfo
	{
		JString*	name;
		Cursor		xid;

		CursorInfo()
			:
			name(nullptr), xid(None)
		{ };

		CursorInfo(JString* n, const Cursor id)
			:
			name(n), xid(id)
		{ };
	};

private:

	JString					itsName;
	Display*				itsXDisplay;
	JXColorManager*			itsColorManager;
	const JSize				itsMaxStringByteCount;
	bool					itsIsMacOSFlag;		// scroll wheel is fine-grain on macOS
	mutable JArray<JRect>*	itsBounds;
	JSize					itsShrinkDisplayToScreenRefCount;

	Time		itsLastEventTime;
	Time		itsLastIdleTime;
	Time		itsLastMotionNotifyTime;

	JPoint				itsLatestMouseLocation;
	JXButtonStates		itsLatestButtonStates;
	JXKeyModifiers		itsLatestKeyModifiers;
	XModifierKeymap*	itsModifierKeymap;
	int					itsJXKeyModifierMapping [ 1+kJXKeyModifierMapCount ];
	XIM					itsXIM;

	JArray<CursorInfo>*	itsCursorList;
	JXGC*				itsDefaultGC;

	JArray<WindowInfo>*	itsWindowList;
	bool			itsNeedsUpdateFlag;
	JXWindow*			itsMouseContainer;		// can be nullptr
	JXWindow*			itsMouseGrabber;		// usually nullptr
	JXWindow*			itsKeyboardGrabber;		// usually nullptr

	JXFontManager*		itsFontManager;
	JXSelectionManager*	itsSelectionManager;
	JXDNDManager*		itsDNDManager;
	JXMenuManager*		itsMenuManager;
	JXWDManager*		itsWDManager;			// can be nullptr
	JXImageCache*		itsImageCache;

	Atom	itsStandardXAtoms[ kStandardXAtomCount ];

	WMBehavior	itsWMBehavior;

private:

	bool	FindMouseContainer(const Window rootWindow,
								   const JCoordinate xRoot, const JCoordinate yRoot,
								   const Window origChildWindow,
								   JXContainer** obj, Window* xWindow = nullptr,
								   JPoint* ptG = nullptr, JPoint* ptR = nullptr) const;

	Cursor	CreateCustomXCursor(const JXCursor& cursor) const;
	void	UpdateModifierMapping();

	// not allowed

	JXDisplay(const JXDisplay&) = delete;
	JXDisplay& operator=(const JXDisplay&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kXEventMessage;	// avoid conflict with XEvent struct
	static const JUtf8Byte* kXError;

	class XEventMessage : public JBroadcaster::Message
		{
		public:

			XEventMessage(const XEvent& event)
				:
				JBroadcaster::Message(kXEventMessage),
				itsXEvent(event),
				itsProcessedFlag(false)
				{ };

			const XEvent&
			GetXEvent() const
			{
				return itsXEvent;
			};

			bool
			WasProcessed() const
			{
				return itsProcessedFlag;
			};

			void
			SetProcessed()
			{
				itsProcessedFlag = true;
			};

		private:

			const XEvent&	itsXEvent;
			bool		itsProcessedFlag;
		};

	class XError : public JBroadcaster::Message
		{
		public:

			XError(const XErrorEvent& error)
				:
				JBroadcaster::Message(kXError),
				itsXError(error),
				itsCaughtFlag(false)
				{ };

			int
			GetType() const
			{
				return itsXError.error_code;
			};

			XID
			GetXID() const
			{
				return itsXError.resourceid;
			};

			const XErrorEvent&
			GetXError() const
			{
				return itsXError;
			};

			bool
			WasCaught() const
			{
				return itsCaughtFlag;
			};

			void
			SetCaught()
			{
				itsCaughtFlag = true;
			};

		private:

			const XErrorEvent&	itsXError;
			bool			itsCaughtFlag;
		};
};


/******************************************************************************
 Cast to Display*

 ******************************************************************************/

inline
JXDisplay::operator Display*()
	const
{
	return itsXDisplay;
}

/******************************************************************************
 GetXDisplay

 ******************************************************************************/

inline Display*
JXDisplay::GetXDisplay()
	const
{
	return itsXDisplay;
}

/******************************************************************************
 Beep

 ******************************************************************************/

inline void
JXDisplay::Beep()
	const
{
	XBell(itsXDisplay, 0);
}

/******************************************************************************
 SendXEvent

 ******************************************************************************/

inline void
JXDisplay::SendXEvent
	(
	const Window	xWindow,
	XEvent*			xEvent,
	long			mask
	)
	const
{
	XSendEvent(itsXDisplay, xWindow, False, mask, xEvent);
}

/******************************************************************************
 Flush

 ******************************************************************************/

inline void
JXDisplay::Flush()
	const
{
	XFlush(itsXDisplay);
}

/******************************************************************************
 Synchronize

 ******************************************************************************/

inline void
JXDisplay::Synchronize()
	const
{
	XSync(itsXDisplay, False);
}

/******************************************************************************
 GetName

 ******************************************************************************/

inline const JString&
JXDisplay::GetName()
	const
{
	return itsName;
}

/******************************************************************************
 GetScreen

 ******************************************************************************/

inline int
JXDisplay::GetScreen()
	const
{
	return DefaultScreen(itsXDisplay);
}

/******************************************************************************
 GetDefaultVisual

 ******************************************************************************/

inline Visual*
JXDisplay::GetDefaultVisual()
	const
{
	return DefaultVisual(itsXDisplay, GetScreen());
}

/******************************************************************************
 GetDepth

 ******************************************************************************/

inline JSize
JXDisplay::GetDepth()
	const
{
	return DefaultDepth(itsXDisplay, GetScreen());
}

/******************************************************************************
 GetRootWindow

 ******************************************************************************/

inline Window
JXDisplay::GetRootWindow()
	const
{
	return DefaultRootWindow(itsXDisplay);
}

/******************************************************************************
 GetDefaultGC

 ******************************************************************************/

inline const JXGC*
JXDisplay::GetDefaultGC()
	const
{
	return itsDefaultGC;
}

/******************************************************************************
 GetColorManager

 ******************************************************************************/

inline JXColorManager*
JXDisplay::GetColorManager()
	const
{
	return itsColorManager;
}

/******************************************************************************
 GetMaxStringByteCount

 ******************************************************************************/

inline JSize
JXDisplay::GetMaxStringByteCount()
	const
{
	return itsMaxStringByteCount;
}

/******************************************************************************
 IsMacOS

 ******************************************************************************/

inline bool
JXDisplay::IsMacOS()
	const
{
	return itsIsMacOSFlag;
}

/******************************************************************************
 Get latest event info

	Use these as a last resort if there is no other way to obtain the
	information.  Most of the time, everything you need is passed to the
	event handler (e.g. HandleMouseDrag(), HandleKeyPress(), etc.)

 ******************************************************************************/

inline Time
JXDisplay::GetLastEventTime()
	const
{
	return itsLastEventTime;
}

inline const JXButtonStates&
JXDisplay::GetLatestButtonStates()
	const
{
	return itsLatestButtonStates;
}

inline const JXKeyModifiers&
JXDisplay::GetLatestKeyModifiers()
	const
{
	return itsLatestKeyModifiers;
}

/******************************************************************************
 GetJXKeyModifierMapping

 ******************************************************************************/

inline const int*
JXDisplay::GetJXKeyModifierMapping()
	const
{
	return itsJXKeyModifierMapping;
}

/******************************************************************************
 GetXIM

 ******************************************************************************/

inline XIM
JXDisplay::GetXIM()
	const
{
	return itsXIM;
}

/******************************************************************************
 GetXCursorID

 ******************************************************************************/

inline Cursor
JXDisplay::GetXCursorID
	(
	const JCursorIndex index
	)
	const
{
	return (itsCursorList->GetElement(index)).xid;
}

/******************************************************************************
 RegisterXAtom

 ******************************************************************************/

inline Atom
JXDisplay::RegisterXAtom
	(
	const JUtf8Byte* name
	)
{
	return XInternAtom(itsXDisplay, name, False);
}

/******************************************************************************
 RegisterXAtoms

 ******************************************************************************/

inline void
JXDisplay::RegisterXAtoms
	(
	const JSize			count,
	const JUtf8Byte**	name,
	Atom*				atom
	)
{
	XInternAtoms(itsXDisplay, (char**) name, count, False, atom);
}

/******************************************************************************
 Atoms for JXWindow

 ******************************************************************************/

inline Atom
JXDisplay::GetWMProtocolsXAtom()
	const
{
	return itsStandardXAtoms[ kWMProtocolsXAtomIndex ];
}

inline Atom
JXDisplay::GetWMClientMachineXAtom()
	const
{
	return itsStandardXAtoms[ kWMClientMachineXAtomIndex ];
}

inline Atom
JXDisplay::GetDeleteWindowXAtom()
	const
{
	return itsStandardXAtoms[ kDeleteWindowXAtomIndex ];
}

inline Atom
JXDisplay::GetWMPingXAtom()
	const
{
	return itsStandardXAtoms[ kWMPingXAtomIndex ];
}

inline Atom
JXDisplay::GetWMPidXAtom()
	const
{
	return itsStandardXAtoms[ kWMPidXAtomIndex ];
}

inline Atom
JXDisplay::GetWMUserTimeXAtom()
	const
{
	return itsStandardXAtoms[ kWMUserTimeXAtomIndex ];
}

inline Atom
JXDisplay::GetWMActiveWindowXAtom()
	const
{
	return itsStandardXAtoms[ kWMActiveWindowXAtomIndex ];
}

inline Atom
JXDisplay::GetSaveYourselfXAtom()
	const
{
	return itsStandardXAtoms[ kSaveYourselfXAtomIndex ];
}

inline Atom
JXDisplay::GetWMStateXAtom()
	const
{
	return itsStandardXAtoms[ kWMStateXAtomIndex ];
}

inline Atom
JXDisplay::GetWMDesktopXAtom()
	const
{
	return itsStandardXAtoms[ kWMDesktopXAtomIndex ];
}

inline Atom
JXDisplay::GetWMCurrentDesktopXAtom()
	const
{
	return itsStandardXAtoms[ kWMCurrentDesktopXAtomIndex ];
}

inline Atom
JXDisplay::GetWMWindowTypeXAtom()
	const
{
	return itsStandardXAtoms[ kWMWindowTypeXAtomIndex ];
}

inline Atom
JXDisplay::GetWMWindowTypeXAtom
	(
	const JIndex type
	)
	const
{
	return itsStandardXAtoms[ kWMWindowTypeXAtomIndex + type ];
}

/******************************************************************************
 GetXFontManager

 ******************************************************************************/

inline JXFontManager*
JXDisplay::GetXFontManager()
	const
{
	return itsFontManager;
}

/******************************************************************************
 GetSelectionManager

 ******************************************************************************/

inline JXSelectionManager*
JXDisplay::GetSelectionManager()
	const
{
	return itsSelectionManager;
}

/******************************************************************************
 GetDNDManager

 ******************************************************************************/

inline JXDNDManager*
JXDisplay::GetDNDManager()
	const
{
	return itsDNDManager;
}

/******************************************************************************
 GetMenuManager

 ******************************************************************************/

inline JXMenuManager*
JXDisplay::GetMenuManager()
	const
{
	return itsMenuManager;
}

/******************************************************************************
 GetWDManager

 ******************************************************************************/

inline bool
JXDisplay::GetWDManager
	(
	JXWDManager** mgr
	)
	const
{
	*mgr = itsWDManager;
	return itsWDManager != nullptr;
}

/******************************************************************************
 GetImageCache

 ******************************************************************************/

inline JXImageCache*
JXDisplay::GetImageCache()
	const
{
	return itsImageCache;
}

/******************************************************************************
 WindowNeedsUpdate

 ******************************************************************************/

inline void
JXDisplay::WindowNeedsUpdate
	(
	JXWindow* window
	)
{
	itsNeedsUpdateFlag = true;
}

/******************************************************************************
 Mouse container

 ******************************************************************************/

inline bool
JXDisplay::GetMouseContainer
	(
	JXWindow** window
	)
	const
{
	*window = itsMouseContainer;
	return *window != nullptr;
}

inline void
JXDisplay::SetMouseContainer
	(
	JXWindow* window
	)
{
	itsMouseContainer = window;
}

/******************************************************************************
 Grabbers

 ******************************************************************************/

inline bool
JXDisplay::GetMouseGrabber
	(
	JXWindow** window
	)
	const
{
	*window = itsMouseGrabber;
	return *window != nullptr;
}

inline void
JXDisplay::SetMouseGrabber
	(
	JXWindow* window
	)
{
	itsMouseGrabber = window;
}

inline bool
JXDisplay::GetKeyboardGrabber
	(
	JXWindow** window
	)
	const
{
	*window = itsKeyboardGrabber;
	return *window != nullptr;
}

inline void
JXDisplay::SetKeyboardGrabber
	(
	JXWindow* window
	)
{
	itsKeyboardGrabber = window;
}

/******************************************************************************
 Window Manager behavior

 ******************************************************************************/

inline const JXDisplay::WMBehavior&
JXDisplay::GetWMBehavior()
	const
{
	return itsWMBehavior;
}

inline void
JXDisplay::SetWMBehavior
	(
	const WMBehavior& behavior
	)
{
	itsWMBehavior = behavior;
}

#endif
