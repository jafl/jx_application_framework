/******************************************************************************
 JXContainer.h

	Interface for the JXContainer class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXContainer
#define _H_JXContainer

#include <jx-af/jcore/JBroadcaster.h>
#include "jx-af/jx/jXEventUtil.h"
#include "jx-af/jx/JXCursor.h"
#include <jx-af/jcore/JPtrArray.h>
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/jColor.h>
#include <X11/Xlib.h>

class JString;
class JFontManager;
class JXDisplay;
class JXWindow;
class JXWidget;
class JXWindowPainter;
class JXCursorAnimator;
class JXCursorAnimationTask;
class JXFontManager;
class JXSelectionManager;
class JXDNDManager;
class JXMenuManager;
class JXHintManager;
class JXFTCCell;

class JXContainer : virtual public JBroadcaster
{
	friend class JXWindow;
	friend class JXDNDManager;
	friend class JXFTCCell;

public:

	~JXContainer();

	bool		IsAncestor(JXContainer* obj) const;

	virtual void	Show();					// must call inherited
	virtual void	Hide();					// must call inherited
	virtual void	Refresh() const = 0;
	virtual void	Redraw() const = 0;
	bool			IsVisible() const;
	void			SetVisible(const bool vis);
	bool			WouldBeVisible() const;

	virtual void	Activate();				// must call inherited
	virtual void	Deactivate();			// must call inherited
	bool			IsActive() const;
	void			SetActive(const bool active);
	bool			WouldBeActive() const;

	virtual void	Suspend();
	virtual void	Resume();
	bool			IsSuspended() const;

	bool			IsDNDSource() const;
	bool			IsDNDTarget() const;

	virtual JPoint	GlobalToLocal(const JCoordinate x, const JCoordinate y) const = 0;
	JPoint			GlobalToLocal(const JPoint& pt) const;
	JRect			GlobalToLocal(const JRect& r) const;
	virtual JPoint	LocalToGlobal(const JCoordinate x, const JCoordinate y) const = 0;
	JPoint			LocalToGlobal(const JPoint& pt) const;
	JRect			LocalToGlobal(const JRect& r) const;

	virtual void	Place(const JCoordinate enclX, const JCoordinate enclY) = 0;
	virtual void	Move(const JCoordinate dx, const JCoordinate dy) = 0;
	virtual void	SetSize(const JCoordinate w, const JCoordinate h) = 0;
	virtual void	AdjustSize(const JCoordinate dw, const JCoordinate dh) = 0;

	JRect			GetBounds() const;				// local coords
	virtual JRect	GetBoundsGlobal() const = 0;
	JCoordinate		GetBoundsWidth() const;
	JCoordinate		GetBoundsHeight() const;

	JRect			GetFrame() const;				// encl coords
	JRect			GetFrameLocal() const;
	virtual JRect	GetFrameGlobal() const = 0;
	JCoordinate		GetFrameWidth() const;
	JCoordinate		GetFrameHeight() const;

	JRect			GetAperture() const;			// local coords
	virtual JRect	GetApertureGlobal() const = 0;
	JCoordinate		GetApertureWidth() const;
	JCoordinate		GetApertureHeight() const;

	JXDisplay*		GetDisplay() const;
	JXWindow*		GetWindow() const;

	JXContainer*	GetEnclosure() const;
//	bool			SetEnclosure(JXContainer* obj);

	JFontManager*		GetFontManager() const;
	JXFontManager*		GetXFontManager() const;
	JXSelectionManager*	GetSelectionManager() const;
	JXDNDManager*		GetDNDManager() const;
	JXMenuManager*		GetMenuManager() const;

	JCursorIndex	GetDefaultCursor() const;

	bool	GetHint(JString* text) const;
	void	SetHint(const JString& text);
	void	ClearHint();

	bool	GetVisibleRectGlobal(const JRect& origRectG,
								 JRect* visRectG) const;

	// primarily invoked (automagically) after BuildWindow() finishes

	void	ExpandToFitContent();

	static void	DebugExpandToFitContent(const bool horiz);
	static void	DebugExpandToFitContentExtras(const bool noop, const bool overlap);

	// public only because it has to be called in special cases
	virtual bool	RunInternalFTC(const bool horizontal, JCoordinate* newSize);
	virtual void	FTCAdjustSize(const JCoordinate dw, const JCoordinate dh);
	JRect			ComputePaddingForInternalFTC() const;

	// called by JXDisplay

	bool	FindContainer(const JPoint& ptG,
						  JXContainer** container) const;

	// called by Menu objects

	virtual bool	IsMenu() const;
	virtual bool	IsMenuTable() const;

protected:

	JXContainer(JXWindow* window, JXContainer* enclosure);
	JXContainer(JXDisplay* display, JXWindow* window, JXContainer* enclosure);

	void			TurnOnBufferedDrawing();
	void			DrawAll(JXWindowPainter& p, const JRect& frameG);
	virtual void	Draw(JXWindowPainter& p, const JRect& rect) = 0;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) = 0;
	virtual void	DrawBackground(JXWindowPainter& p, const JRect& frame) = 0;

	virtual void	HandleMouseEnter();
	virtual void	HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseLeave();

	virtual bool	AcceptDrag(const JPoint& pt, const JXMouseButton button,
							   const JXKeyModifiers& modifiers);	// must call inherited
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);
	virtual bool	HitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
								   const JPoint& pt, const Time time,
								   const JXWidget* source);
	virtual void	HandleDNDEnter();
	virtual void	HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void	HandleDNDLeave();
	virtual void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
								  const Atom action, const Time time,
								  const JXWidget* source);

	// mostly for use by JXScrollableWidget
	virtual void	HandleDNDScroll(const JPoint& pt, const JXMouseButton scrollButton,
									const JXKeyModifiers& modifiers);

	bool			DispatchClientMessage(const XClientMessageEvent& clientMessage);
	virtual bool	HandleClientMessage(const XClientMessageEvent& clientMessage);
							// must call inherited

	void			NotifyBoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) = 0;
	virtual void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy) = 0;

	void			NotifyBoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) = 0;
	virtual void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh) = 0;

	virtual bool		IncludeInFTC() const;
	virtual bool		NeedsInternalFTC() const;
	virtual JCoordinate	GetFTCMinContentSize(const bool horizontal) const;
	virtual JRect		GetFrameForFTC() const;

	bool	GetEnclosedObjects(JPtrArrayIterator<JXContainer>** iter) const;
	void	DeleteEnclosedObjects();

	// static cursor

	void			ShowCursor();
	void			HideCursor();
	void			SetDefaultCursor(const JCursorIndex index);
	void			DispatchCursor(const JPoint& ptG, const JXKeyModifiers& modifiers);
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);
	void			DisplayCursor(const JCursorIndex index);

	// animated cursor

	JXCursorAnimator*	CreateCursorAnimator();
	JXCursorAnimator*	GetCursorAnimator() const;
	void				AnimateCursor() const;
	void				RemoveCursorAnimator();

	// called by JXWindow

	void	ActivateCursor(const JPoint& ptG, const JXKeyModifiers& modifiers);
	void	DeactivateCursor();

	// FTC

	std::ostream&	GetFTCLog() const;

private:

	JXWindow*				itsWindow;
	JXContainer*			itsEnclosure;
	JPtrArray<JXContainer>*	itsEnclosedObjs;	// nullptr if empty
	bool					itsGoingAwayFlag;

	bool	itsActiveFlag;
	bool	itsWasActiveFlag;	// true => activate when enclosure is activated
	bool	itsVisibleFlag;
	bool	itsWasVisibleFlag;	// true => show when enclosure is made visible
	JSize	itsSuspendCount;

	bool	itsIsDNDSourceFlag;
	bool	itsIsDNDTargetFlag;

	// avoids repeated DispatchMouse()

	bool	itsIsShowingFlag;
	bool	itsIsHidingFlag;
	bool	itsIsActivatingFlag;
	bool	itsIsDeactivatingFlag;

	// cursor information

	JCursorIndex			itsDefCursor;
	JCursorIndex			itsInvisibleCursor;
	bool					itsCursorVisibleFlag;
	JCursorIndex			itsCurrCursor;
	JXCursorAnimator*		itsCursorAnim;
	JXCursorAnimationTask*	itsCursorAnimTask;

	// hint

	JXHintManager*	itsHintMgr;		// nullptr if no hint

	// FTC

	static bool				theDebugFTCFlag;
	static bool				theDebugHorizFTCFlag;
	static bool				theDebugVertFTCFlag;
	static bool				theDebugFTCNoopExaminations;
	static bool				theDebugFTCWillOverlapNonincludedWidget;
	static std::ostream*	theDebugFTCLogBuffer;

private:

	void	JXContainerX(JXDisplay* display, JXWindow* window,
						 JXContainer* enclosure);

	void	AddEnclosedObject(JXContainer* theObject);
	void	RemoveEnclosedObject(JXContainer* theObject);

	bool		FTCBuildLayout(const bool expandHorizontally, JXFTCCell** root) const;
	JXFTCCell*	FTCGroupAlignedObjects(JXContainer* target,
									   JPtrArray<JXContainer>* objList,
									   JPtrArray<JXContainer>* fullObjList,
									   const bool horizontal,
									   const bool exact,
									   const bool first) const;
	bool		FTCWillOverlapNonincludedWidget(const JXContainer* obj1,
												const JXContainer* obj2,
												const JPtrArray<JXContainer>& fullObjList,
												const JPtrArray<JXContainer>& matchedList) const;
	void		FTCTrimBlockedMatches(const JXContainer* target,
									  const JPtrArray<JXContainer>& fullObjList,
									  const JPtrArray<JXContainer>& matchedList,
									  const bool horizontal,
									  const bool deleteBlockedWidgetCells,
									  JPtrArray<JXFTCCell>* cellList) const;

	static JListT::CompareResult
		FTCCompareHorizontally(JXContainer* const & w1, JXContainer* const & w2);
	static JListT::CompareResult
		FTCCompareVertically(JXContainer* const & w1, JXContainer* const & w2);

	// called by JXWindow

	void	DispatchNewMouseEvent(const int eventType, const JPoint& ptG,
								  const JXMouseButton button,
								  const unsigned int state);
	void	DispatchMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							  const JXKeyModifiers& modifiers);
	void	DispatchMouseUp(const JPoint& pt, const JXMouseButton button,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);

	void	MouseEnter();
	void	MouseHere(const JPoint& pt, const JXKeyModifiers& modifiers);
	void	MouseLeave();

	void	MouseDown(const JPoint& pt, const JXMouseButton button,
					  const JSize clickCount,
					  const JXButtonStates& buttonStates,
					  const JXKeyModifiers& modifiers);

	// called by JXDNDManager

	void	BecomeDNDSource();
	void	FinishDNDSource();
	void	DNDEnter();
	void	DNDHere(const JPoint& pt, const JXWidget* source);
	void	DNDScroll(const JPoint& pt, const JXMouseButton scrollButton,
					  const JXKeyModifiers& modifiers);
	void	DNDLeave();
	void	DNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
					const Atom action, const Time time, const JXWidget* source);

	// not allowed

	JXContainer(const JXContainer&) = delete;
	JXContainer& operator=(const JXContainer&) = delete;
};


/******************************************************************************
 IsVisible

 ******************************************************************************/

inline bool
JXContainer::IsVisible()
	const
{
	return itsVisibleFlag;
}

/******************************************************************************
 SetVisible

 ******************************************************************************/

inline void
JXContainer::SetVisible
	(
	const bool vis
	)
{
	if (vis && !WouldBeVisible())
		{
		Show();
		}
	else if (!vis && WouldBeVisible())
		{
		Hide();
		}
}

/******************************************************************************
 IsActive

 ******************************************************************************/

inline bool
JXContainer::IsActive()
	const
{
	return itsActiveFlag && itsSuspendCount == 0;
}

/******************************************************************************
 SetActive

 ******************************************************************************/

inline void
JXContainer::SetActive
	(
	const bool active
	)
{
	if (active && !WouldBeActive())
		{
		Activate();
		}
	else if (!active && WouldBeActive())
		{
		Deactivate();
		}
}

/******************************************************************************
 IsSuspended

 ******************************************************************************/

inline bool
JXContainer::IsSuspended()
	const
{
	return itsSuspendCount > 0;
}

/******************************************************************************
 IsDNDSource

 ******************************************************************************/

inline bool
JXContainer::IsDNDSource()
	const
{
	return itsIsDNDSourceFlag;
}

/******************************************************************************
 IsDNDTarget

 ******************************************************************************/

inline bool
JXContainer::IsDNDTarget()
	const
{
	return itsIsDNDTargetFlag;
}

/******************************************************************************
 GetWindow

 ******************************************************************************/

inline JXWindow*
JXContainer::GetWindow()
	const
{
	return itsWindow;
}

/******************************************************************************
 GetEnclosure

 ******************************************************************************/

inline JXContainer*
JXContainer::GetEnclosure()
	const
{
	return itsEnclosure;
}

/******************************************************************************
 GlobalToLocal

 ******************************************************************************/

inline JPoint
JXContainer::GlobalToLocal
	(
	const JPoint& pt
	)
	const
{
	return GlobalToLocal(pt.x, pt.y);
}

/******************************************************************************
 LocalToGlobal

 ******************************************************************************/

inline JPoint
JXContainer::LocalToGlobal
	(
	const JPoint& pt
	)
	const
{
	return LocalToGlobal(pt.x, pt.y);
}

/******************************************************************************
 GetBounds

	Returns the bounds in local coordinates.

 ******************************************************************************/

inline JRect
JXContainer::GetBounds()
	const
{
	return GlobalToLocal(GetBoundsGlobal());
}

inline JCoordinate
JXContainer::GetBoundsWidth()
	const
{
	return (GetBoundsGlobal()).width();
}

inline JCoordinate
JXContainer::GetBoundsHeight()
	const
{
	return (GetBoundsGlobal()).height();
}

/******************************************************************************
 GetFrame

 ******************************************************************************/

inline JRect
JXContainer::GetFrame()		// enclosure coordinates
	const
{
	if (itsEnclosure != nullptr)
		{
		return (itsEnclosure->GlobalToLocal(GetFrameGlobal()));
		}
	else
		{
		return GetFrameGlobal();
		}
}

inline JRect
JXContainer::GetFrameLocal()
	const
{
	return GlobalToLocal(GetFrameGlobal());
}

inline JCoordinate
JXContainer::GetFrameWidth()
	const
{
	return (GetFrameGlobal()).width();
}

inline JCoordinate
JXContainer::GetFrameHeight()
	const
{
	return (GetFrameGlobal()).height();
}

/******************************************************************************
 GetAperture

	Returns the aperture in local coordinates.

 ******************************************************************************/

inline JRect
JXContainer::GetAperture()
	const
{
	return (GlobalToLocal(GetApertureGlobal()));
}

inline JCoordinate
JXContainer::GetApertureWidth()
	const
{
	return (GetApertureGlobal()).width();
}

inline JCoordinate
JXContainer::GetApertureHeight()
	const
{
	return (GetApertureGlobal()).height();
}

/******************************************************************************
 GetDefaultCursor

 ******************************************************************************/

inline JCursorIndex
JXContainer::GetDefaultCursor()
	const
{
	return itsDefCursor;
}

/******************************************************************************
 GetCursorAnimator (protected)

 ******************************************************************************/

inline JXCursorAnimator*
JXContainer::GetCursorAnimator()
	const
{
	return itsCursorAnim;
}

/******************************************************************************
 GetFTCLog (protected)

 ******************************************************************************/

inline std::ostream&
JXContainer::GetFTCLog()
	const
{
	return (theDebugFTCLogBuffer != nullptr ? *theDebugFTCLogBuffer : std::cout);
}

#endif
