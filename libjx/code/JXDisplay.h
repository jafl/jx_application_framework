/******************************************************************************
 JXDisplay.h

	Interface for the JXDisplay class

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDisplay
#define _H_JXDisplay

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCursor.h>
#include <JArray.h>
#include <jXEventUtil.h>
#include <JXKeyModifiers.h>	// need defn of kJXKeyModifierMapCount
#include <JRect.h>
#include <JString.h>

class JFontManager;
class JXGC;
class JXColormap;
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

		kStandardXAtomCount
	};

public:

	static JBoolean	Create(const JCharacter* displayName, JXDisplay** display);

	virtual ~JXDisplay();

	JBoolean	Close();

	operator Display*() const;

	const JString&	GetName() const;
	Display*		GetXDisplay() const;
	JXColormap*		GetColormap() const;
	JSize			GetMaxStringLength() const;

	const JFontManager*		GetFontManager() const;
	const JXFontManager*	GetXFontManager() const;
	JXSelectionManager*		GetSelectionManager() const;
	JXDNDManager*			GetDNDManager() const;
	JXMenuManager*			GetMenuManager() const;
	JBoolean				GetWDManager(JXWDManager** mgr) const;
	JXImageCache*			GetImageCache() const;

	JCursorIndex	CreateBuiltInCursor(const JCharacter* name,
										const unsigned int shape);
	JCursorIndex	CreateCustomCursor(const JCharacter* name,
									   const JXCursor& cursor);
	JBoolean		GetCursor(const JCharacter* name,
							  JCursorIndex* index) const;
	void			DisplayCursorInAllWindows(const JCursorIndex index);

	void		AllowPrivateColormap(const JBoolean allow);
	JBoolean	ForcePrivateColormap();

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

	JBoolean	KeysymToModifier(const KeySym keysym, JIndex* modifierIndex) const;
	JBoolean	KeycodeToModifier(const KeyCode keycode, JIndex* modifierIndex) const;

	// XFlush() and XSync()

	void	Flush() const;
	void	Synchronize() const;

	void	SendXEvent(const Window xWindow, XEvent* xEvent, const long mask = 0) const;

	// useful for Drag-And-Drop, and for Menu objects

	JBoolean	FindMouseContainer(JXContainer** obj, Window* xWindow = NULL,
								   JPoint* ptG = NULL, JPoint* ptR = NULL) const;
	JBoolean	FindMouseContainer(const JXContainer* coordOwner,
								   const JPoint& pt, JXContainer** obj,
								   Window* xWindow = NULL,
								   JPoint* ptG = NULL, JPoint* ptR = NULL) const;

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

	JBoolean	GetMouseContainer(JXWindow** window) const;
	void		SetMouseContainer(JXWindow* window);
	JBoolean	GetMouseGrabber(JXWindow** window) const;
	void		SetMouseGrabber(JXWindow* window);

	Cursor		GetXCursorID(const JCursorIndex index) const;

	static JBoolean	WindowExists(JXDisplay* display, const Display* xDisplay,
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

	// called by Menu objects

	JBoolean	SwitchDrag(JXContainer* fromObj, const JPoint& fromObjPt,
						   const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers,
						   JXContainer* toObj);

	JBoolean	GetKeyboardGrabber(JXWindow** window) const;
	void		SetKeyboardGrabber(JXWindow* window);

	// called by JXColormap

	void	ColormapChanged(JXColormap*	colormap);

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

	Atom	RegisterXAtom(const JCharacter* name);
	void	RegisterXAtoms(const JSize count, const JCharacter** name, Atom* atom);

	JBoolean	FindXWindow(const Window xWindow, JXWindow** window) const;

	const int*	GetJXKeyModifierMapping() const;

	// installed by JXApplication

	static void	CheckForXErrors();
	static int	JXErrorHandler(Display* xDisplay, XErrorEvent* error);

protected:

	JXDisplay(const JCharacter* displayName, Display* xDisplay);

private:

	struct WindowInfo
	{
		JXWindow*	window;		// not owned
		Window		xWindow;

		WindowInfo()
			:
			window(NULL), xWindow(0)
		{ };

		WindowInfo(JXWindow* w, const Window xw)
			:
			window(w), xWindow(xw)
		{ };
	};

	static JOrderedSetT::CompareResult
		CompareXWindows(const WindowInfo& info1, const WindowInfo& info2);

	struct CursorInfo
	{
		JString*	name;
		Cursor		xid;

		CursorInfo()
			:
			name(NULL), xid(None)
		{ };

		CursorInfo(JString* n, const Cursor id)
			:
			name(n), xid(id)
		{ };
	};

private:

	JString					itsName;
	Display*				itsXDisplay;
	JXColormap*				itsColormap;
	const JSize				itsMaxStringLength;
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

	JArray<CursorInfo>*	itsCursorList;
	JXGC*				itsDefaultGC;

	JArray<WindowInfo>*	itsWindowList;
	JBoolean			itsNeedsUpdateFlag;
	JXWindow*			itsMouseContainer;		// can be NULL
	JXWindow*			itsMouseGrabber;		// usually NULL
	JXWindow*			itsKeyboardGrabber;		// usually NULL

	JXFontManager*		itsFontManager;
	JXSelectionManager*	itsSelectionManager;
	JXDNDManager*		itsDNDManager;
	JXMenuManager*		itsMenuManager;
	JXWDManager*		itsWDManager;			// can be NULL
	JXImageCache*		itsImageCache;

	Atom	itsStandardXAtoms[ kStandardXAtomCount ];

private:

	JBoolean	FindMouseContainer(const Window rootWindow,
								   const JCoordinate xRoot, const JCoordinate yRoot,
								   const Window origChildWindow,
								   JXContainer** obj, Window* xWindow = NULL,
								   JPoint* ptG = NULL, JPoint* ptR = NULL) const;

	Cursor	CreateCustomXCursor(const JXCursor& cursor) const;
	void	UpdateModifierMapping();

	// not allowed

	JXDisplay(const JXDisplay& source);
	const JXDisplay& operator=(const JXDisplay& source);

public:

	// JBroadcaster messages

	static const JCharacter* kXEventMessage;	// avoid conflict with XEvent struct
	static const JCharacter* kXError;

	class XEventMessage : public JBroadcaster::Message
		{
		public:

			XEventMessage(const XEvent& event)
				:
				JBroadcaster::Message(kXEventMessage),
				itsXEvent(event),
				itsProcessedFlag(kJFalse)
				{ };

			const XEvent&
			GetXEvent() const
			{
				return itsXEvent;
			};

			JBoolean
			WasProcessed() const
			{
				return itsProcessedFlag;
			};

			void
			SetProcessed()
			{
				itsProcessedFlag = kJTrue;
			};

		private:

			const XEvent&	itsXEvent;
			JBoolean		itsProcessedFlag;
		};

	class XError : public JBroadcaster::Message
		{
		public:

			XError(const XErrorEvent& error)
				:
				JBroadcaster::Message(kXError),
				itsXError(error),
				itsCaughtFlag(kJFalse)
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

			JBoolean
			WasCaught() const
			{
				return itsCaughtFlag;
			};

			void
			SetCaught()
			{
				itsCaughtFlag = kJTrue;
			};

		private:

			const XErrorEvent&	itsXError;
			JBoolean			itsCaughtFlag;
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
 GetColormap

 ******************************************************************************/

inline JXColormap*
JXDisplay::GetColormap()
	const
{
	return itsColormap;
}

/******************************************************************************
 GetMaxStringLength

 ******************************************************************************/

inline JSize
JXDisplay::GetMaxStringLength()
	const
{
	return itsMaxStringLength;
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
	const JCharacter* name
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
	const JCharacter**	name,
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

inline const JXFontManager*
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

inline JBoolean
JXDisplay::GetWDManager
	(
	JXWDManager** mgr
	)
	const
{
	*mgr = itsWDManager;
	return JI2B( itsWDManager != NULL );
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
	itsNeedsUpdateFlag = kJTrue;
}

/******************************************************************************
 Mouse container

 ******************************************************************************/

inline JBoolean
JXDisplay::GetMouseContainer
	(
	JXWindow** window
	)
	const
{
	*window = itsMouseContainer;
	return JConvertToBoolean( *window != NULL );
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

inline JBoolean
JXDisplay::GetMouseGrabber
	(
	JXWindow** window
	)
	const
{
	*window = itsMouseGrabber;
	return JConvertToBoolean( *window != NULL );
}

inline void
JXDisplay::SetMouseGrabber
	(
	JXWindow* window
	)
{
	itsMouseGrabber = window;
}

inline JBoolean
JXDisplay::GetKeyboardGrabber
	(
	JXWindow** window
	)
	const
{
	*window = itsKeyboardGrabber;
	return JConvertToBoolean( *window != NULL );
}

inline void
JXDisplay::SetKeyboardGrabber
	(
	JXWindow* window
	)
{
	itsKeyboardGrabber = window;
}

#endif
