/******************************************************************************
 JXContainer.h

	Interface for the JXContainer class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXContainer
#define _H_JXContainer

#include <JBroadcaster.h>
#include <jXEventUtil.h>
#include <JXCursor.h>
#include <JPtrArray.h>
#include <JRect.h>
#include <jColor.h>
#include <X11/Xlib.h>

class JString;
class JFontManager;
class JXDisplay;
class JXColormap;
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

	virtual ~JXContainer();

	JBoolean		IsAncestor(JXContainer* obj) const;

	virtual void	Show();					// must call inherited
	virtual void	Hide();					// must call inherited
	virtual void	Refresh() const = 0;
	virtual void	Redraw() const = 0;
	JBoolean		IsVisible() const;
	void			SetVisible(const JBoolean vis);
	JBoolean		WouldBeVisible() const;

	virtual void	Activate();				// must call inherited
	virtual void	Deactivate();			// must call inherited
	JBoolean		IsActive() const;
	void			SetActive(const JBoolean active);
	JBoolean		WouldBeActive() const;

	virtual void	Suspend();
	virtual void	Resume();
	JBoolean		IsSuspended() const;

	JBoolean		IsDNDSource() const;
	JBoolean		IsDNDTarget() const;

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
	JXColormap*		GetColormap() const;

	JXContainer*	GetEnclosure() const;
//	JBoolean		SetEnclosure(JXContainer* obj);

	const JFontManager*		GetFontManager() const;
	const JXFontManager*	GetXFontManager() const;
	JXSelectionManager*		GetSelectionManager() const;
	JXDNDManager*			GetDNDManager() const;
	JXMenuManager*			GetMenuManager() const;

	JCursorIndex	GetDefaultCursor() const;

	JBoolean	GetHint(JString* text) const;
	void		SetHint(const JCharacter* text);
	void		ClearHint();

	JBoolean	GetVisibleRectGlobal(const JRect& origRectG,
									 JRect* visRectG) const;

	// primarily invoked (automagically) after BuildWindow() finishes

	void	ExpandToFitContent();

	static void	DebugExpandToFitContent(const JBoolean horiz);
	static void	DebugExpandToFitContentExtras(const JBoolean noop, const JBoolean overlap);

	// called by JXDisplay

	JBoolean	FindContainer(const JPoint& ptG,
							  JXContainer** container) const;

	// called by Menu objects

	virtual JBoolean	IsMenu() const;
	virtual JBoolean	IsMenuTable() const;

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

	virtual JBoolean	AcceptDrag(const JPoint& pt, const JXMouseButton button,
								   const JXKeyModifiers& modifiers);	// must call inherited
	virtual void		HandleMouseDown(const JPoint& pt, const JXMouseButton button,
										const JSize clickCount,
										const JXButtonStates& buttonStates,
										const JXKeyModifiers& modifiers);
	virtual void		HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
										const JXKeyModifiers& modifiers);
	virtual void		HandleMouseUp(const JPoint& pt, const JXMouseButton button,
									  const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers);
	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	// mostly for use by JXScrollableWidget
	virtual void	HandleDNDScroll(const JPoint& pt, const JXMouseButton scrollButton,
									const JXKeyModifiers& modifiers);

	JBoolean			DispatchClientMessage(const XClientMessageEvent& clientMessage);
	virtual JBoolean	HandleClientMessage(const XClientMessageEvent& clientMessage);
							// must call inherited

	void			NotifyBoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) = 0;
	virtual void	EnclosingBoundsMoved(const JCoordinate dx, const JCoordinate dy) = 0;

	void			NotifyBoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) = 0;
	virtual void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh) = 0;

	virtual JBoolean	IncludeInFTC() const;
	virtual JBoolean	NeedsInternalFTC() const;
	virtual JCoordinate	RunInternalFTC(const JBoolean horizontal);
	virtual JCoordinate	GetFTCMinContentSize(const JBoolean horizontal) const;
	virtual JRect		GetFrameForFTC() const;
	virtual void		FTCAdjustSize(const JCoordinate dw, const JCoordinate dh);

	JBoolean	GetEnclosedObjects(JPtrArrayIterator<JXContainer>** iter) const;
	void		DeleteEnclosedObjects();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
	JPtrArray<JXContainer>*	itsEnclosedObjs;	// NULL if empty
	JBoolean				itsGoingAwayFlag;

	JBoolean	itsActiveFlag;
	JBoolean	itsWasActiveFlag;	// kJTrue => activate when enclosure is activated
	JBoolean	itsVisibleFlag;
	JBoolean	itsWasVisibleFlag;	// kJTrue => show when enclosure is made visible
	JSize		itsSuspendCount;

	JBoolean	itsIsDNDSourceFlag;
	JBoolean	itsIsDNDTargetFlag;

	// avoids repeated DispatchMouse()

	JBoolean	itsIsShowingFlag;
	JBoolean	itsIsHidingFlag;
	JBoolean	itsIsActivatingFlag;
	JBoolean	itsIsDeactivatingFlag;

	// cursor information

	JCursorIndex			itsDefCursor;
	JCursorIndex			itsInvisibleCursor;
	JBoolean				itsCursorVisibleFlag;
	JCursorIndex			itsCurrCursor;
	JXCursorAnimator*		itsCursorAnim;
	JXCursorAnimationTask*	itsCursorAnimTask;

	// hint

	JXHintManager*	itsHintMgr;		// NULL if no hint

	// FTC

	static JBoolean			theDebugFTCFlag;
	static JBoolean			theDebugHorizFTCFlag;
	static JBoolean			theDebugVertFTCFlag;
	static JBoolean			theDebugFTCNoopExaminations;
	static JBoolean			theDebugFTCWillOverlapNonincludedWidget;
	static std::ostream*	theDebugFTCLogBuffer;

private:

	void	JXContainerX(JXDisplay* display, JXWindow* window,
						 JXContainer* enclosure);

	void	AddEnclosedObject(JXContainer* theObject);
	void	RemoveEnclosedObject(JXContainer* theObject);

	JXFTCCell*	FTCBuildLayout(const JBoolean expandHorizontally) const;
	JXFTCCell*	FTCGroupAlignedObjects(JXContainer* target,
									   JPtrArray<JXContainer>* objList,
									   JPtrArray<JXContainer>* fullObjList,
									   const JBoolean horizontal,
									   const JBoolean exact,
									   const JBoolean first) const;
	JBoolean	FTCWillOverlapNonincludedWidget(const JXContainer* obj1,
												const JXContainer* obj2,
												const JPtrArray<JXContainer>& fullObjList,
												const JPtrArray<JXContainer>& matchedList) const;
	void		FTCTrimBlockedMatches(const JXContainer* target,
									  const JPtrArray<JXContainer>& fullObjList,
									  const JPtrArray<JXContainer>& matchedList,
									  const JBoolean horizontal,
									  const JBoolean deleteBlockedWidgetCells,
									  JPtrArray<JXFTCCell>* cellList) const;

	static JOrderedSetT::CompareResult
		FTCCompareHorizontally(JXContainer* const & w1, JXContainer* const & w2);
	static JOrderedSetT::CompareResult
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

	JXContainer(const JXContainer& source);
	const JXContainer& operator=(const JXContainer& source);
};


/******************************************************************************
 IsVisible

 ******************************************************************************/

inline JBoolean
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
	const JBoolean vis
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

inline JBoolean
JXContainer::IsActive()
	const
{
	return JI2B(itsActiveFlag && itsSuspendCount == 0);
}

/******************************************************************************
 SetActive

 ******************************************************************************/

inline void
JXContainer::SetActive
	(
	const JBoolean active
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

inline JBoolean
JXContainer::IsSuspended()
	const
{
	return JI2B(itsSuspendCount > 0);
}

/******************************************************************************
 IsDNDSource

 ******************************************************************************/

inline JBoolean
JXContainer::IsDNDSource()
	const
{
	return itsIsDNDSourceFlag;
}

/******************************************************************************
 IsDNDTarget

 ******************************************************************************/

inline JBoolean
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
	if (itsEnclosure != NULL)
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
	return (theDebugFTCLogBuffer != NULL ? *theDebugFTCLogBuffer : std::cout);
}

#endif
