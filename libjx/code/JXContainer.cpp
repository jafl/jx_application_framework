/******************************************************************************
 JXContainer.cpp

	Abstract base class to provide concept of enclosure.

	*** All JXContainers must be allocated on the heap.

	This class also implements the event dispatcher and default event handling
	(do nothing) so derived classes only have to implement handlers for the
	events that they support.

	Draw, DrawBorder, and DrawBackground are special cases.  They are pure
	virtual because they always have to be provided.

	The default handling of multiple clicks is to always handle them.
	Often, however, only clicks that are "close" should be treated as multiples.
	To define "close," override HitSamePart.

	Drag-And-Drop (DND) can be supported by overriding the four HandleDND*()
	functions.  They operate just like the corresponding HandleMouse*()
	functions.  HandleDNDLeave() is called if the mouse leaves.
	HandleDNDDrop() is called if the mouse is released.  The data that was
	dropped is accessed via the selection manager. (Get the name of the
	selection from JXDNDManager::GetDNDSelectionName().)

	Since each class will only accept certain data types, each derived class
	should override WillAcceptDrop() and return kJTrue if they will accept
	at least one of the given data types.  If the function returns kJFalse,
	the object will not receive any DND messages.

	Since Widgets are the ones that actually own data, only Widgets can
	initiate DND (via BeginDND()).  Containers can accept drops, however.
	This is why all the DND functions other than BeginDND() are defined
	by JXContainer.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXContainer.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXCursorAnimator.h>
#include <JXCursorAnimationTask.h>
#include <JXDNDManager.h>
#include <JXHintManager.h>
#include <jXUtil.h>
#include <jXGlobals.h>
#include <JOrderedSetUtil.h>
#include <jAssert.h>

JBoolean JXContainer::theDebugFTCFlag      = kJFalse;
JBoolean JXContainer::theDebugHorizFTCFlag = kJFalse;
JBoolean JXContainer::theDebugVertFTCFlag  = kJFalse;

/******************************************************************************
 Constructor (protected)

	If enclosure is NULL, we are a window.

 ******************************************************************************/

JXContainer::JXContainer
	(
	JXWindow*		window,
	JXContainer*	enclosure
	)
{
	JXContainerX(window->GetDisplay(), window, enclosure);
}

// for use by JXWindow

JXContainer::JXContainer
	(
	JXDisplay*		display,
	JXWindow*		window,
	JXContainer*	enclosure
	)
{
	JXContainerX(display, window, enclosure);
}

// private

void
JXContainer::JXContainerX
	(
	JXDisplay*		display,
	JXWindow*		window,
	JXContainer*	enclosure
	)
{
	assert( display != NULL && window != NULL );

	itsWindow          = window;
	itsEnclosure       = enclosure;
	itsEnclosedObjs    = NULL;
	itsActiveFlag      = kJFalse;
	itsWasActiveFlag   = kJTrue;
	itsVisibleFlag     = kJFalse;
	itsWasVisibleFlag  = kJTrue;
	itsSuspendCount    = 0;
	itsIsDNDSourceFlag = kJFalse;
	itsIsDNDTargetFlag = kJFalse;
	itsGoingAwayFlag   = kJFalse;

	if (enclosure != NULL)
		{
		// inherit settings from enclosure

		itsActiveFlag   = enclosure->itsActiveFlag;
		itsVisibleFlag  = enclosure->itsVisibleFlag;
		itsSuspendCount = enclosure->itsSuspendCount;
		}

	itsIsShowingFlag      = kJFalse;
	itsIsHidingFlag       = kJFalse;
	itsIsActivatingFlag   = kJFalse;
	itsIsDeactivatingFlag = kJFalse;

	itsDefCursor         = kJXDefaultCursor;
	itsInvisibleCursor   = JXGetInvisibleCursor(display);
	itsCursorVisibleFlag = kJTrue;
	itsCurrCursor        = kJXDefaultCursor;
	itsCursorAnim        = NULL;
	itsCursorAnimTask    = NULL;

	itsHintMgr = NULL;

	if (itsEnclosure != NULL)
		{
		itsEnclosure->AddEnclosedObject(this);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXContainer::~JXContainer()
{
	if (itsEnclosure != NULL)
		{
		itsEnclosure->RemoveEnclosedObject(this);
		}

	JXContainer* c;
	if (itsWindow->GetMouseContainer(&c) && c == this)
		{
		itsWindow->SetMouseContainer(NULL, JPoint(0,0), 0);
		}

	jdelete itsCursorAnimTask;
	jdelete itsCursorAnim;
	jdelete itsHintMgr;

	DeleteEnclosedObjects();
}

/******************************************************************************
 TurnOnBufferedDrawing

 ******************************************************************************/

void
JXContainer::TurnOnBufferedDrawing()
{
	itsWindow->BufferDrawing(kJTrue);
}

/******************************************************************************
 IsAncestor

	Returns kJTrue if we are an ancestor of the given object.

 ******************************************************************************/

JBoolean
JXContainer::IsAncestor
	(
	JXContainer* obj
	)
	const
{
	JXContainer* encl = obj->itsEnclosure;
	while (1)
		{
		if (encl == this)
			{
			return kJTrue;
			}
		else if (encl == NULL)
			{
			return kJFalse;
			}

		encl = encl->itsEnclosure;
		}
}

/******************************************************************************
 SetEnclosure

	Returns kJFalse if we are a JXWindow or obj is in a different JXWindow.

	Before we can implement this, we need to define pure virtual
	EnclosureChanged() so derived classes can update their state.  This may
	be very difficult if said state is application specific and can only be
	computed at a higher level, e.g., JXDirector or JXApplication.

 ******************************************************************************/
/*
JBoolean
JXContainer::SetEnclosure
	(
	JXContainer* obj
	)
{
	if (itsEnclosure != NULL && obj->GetWindow() == itsWindow)
		{
		itsEnclosure->RemoveEnclosedObject(this);
		itsEnclosure = obj;
		itsEnclosure->AddEnclosedObject(this);
		itsWindow->Refresh();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
*/
/******************************************************************************
 DrawAll (protected)

 ******************************************************************************/

void
JXContainer::DrawAll
	(
	JXWindowPainter&	p,
	const JRect&		origFrameG
	)
{
	JRect clipRectG = GetFrameGlobal();
	if (!JIntersection(clipRectG, origFrameG, &clipRectG))
		{
		return;
		}

	JRect apClipRectG = GetApertureGlobal();
	const JBoolean apVisible =
		JIntersection(apClipRectG, origFrameG, &apClipRectG);

	// draw enclosed objects first, if visible

	Region visRegion = NULL;
	if (apVisible && itsEnclosedObjs != NULL)
		{
		XRectangle xClipRect = JXJToXRect(apClipRectG);
		visRegion            = JXRectangleRegion(&xClipRect);

		// draw visible objects in reverse order so all the
		// other routines can check them in the normal order

		const JSize objCount = itsEnclosedObjs->GetElementCount();
		for (JIndex i=objCount; i>=1; i--)
			{
			JXContainer* obj = itsEnclosedObjs->NthElement(i);
			if (obj->IsVisible())
				{
				obj->DrawAll(p, apClipRectG);

				// update region that we need to draw to

				XRectangle xFrame = JXJToXRect(obj->GetFrameGlobal());
				JXSubtractRectFromRegion(visRegion, &xFrame, visRegion);
				}
			}
		}

	// prepare the painter to draw

	const JRect boundsG = GetBoundsGlobal();

	p.SetOrigin(boundsG.left, boundsG.top);

	// draw border in the strip around the aperture

	const JRect frameRect    = GetFrameLocal();
	const JRect apertureRect = GlobalToLocal(apClipRectG);
	if (frameRect != apertureRect)
		{
		XRectangle xFrameRect    = JXJToXRect(frameRect);
		Region borderRegion      = JXRectangleRegion(&xFrameRect);
		XRectangle xApertureRect = JXJToXRect(apertureRect);
		JXSubtractRectFromRegion(borderRegion, &xApertureRect, borderRegion);
		p.Reset(clipRectG, borderRegion);
		DrawBorder(p, GetFrameLocal());
		XDestroyRegion(borderRegion);
		}

	// draw background and contents, if visible

	if (!theDebugFTCFlag && visRegion != NULL && XEmptyRegion(visRegion))
		{
		// If nothing else is visible, we can quit now.

		XDestroyRegion(visRegion);
		return;
		}

	if (apVisible)
		{
		Region origDefClipRegion = NULL;
		if (visRegion != NULL)
			{
			Region region;
			if (p.GetDefaultClipRegion(&region))
				{
				origDefClipRegion = JXCopyRegion(region);
				}
			p.SetDefaultClipRegion(visRegion);	// in case derived class calls Reset()
			XOffsetRegion(visRegion, -boundsG.left, -boundsG.top);	// convert to local coordinates
			p.Reset(apClipRectG, visRegion);
			}
		else
			{
			p.JPainter::Reset(apClipRectG);
			}

		const JRect apLocal = GetAperture();
		DrawBackground(p, apLocal);

		// draw contents clipped to aperture, if visible

		JRect boundsClipRectG;
		if (JIntersection(boundsG, apClipRectG, &boundsClipRectG))
			{
			p.ResetAllButClipping();
			Draw(p, GlobalToLocal(boundsClipRectG));
			}

		// clean up

		if (origDefClipRegion != NULL)
			{
			p.SetDefaultClipRegion(origDefClipRegion);
			XDestroyRegion(origDefClipRegion);
			}
		}

	// clean up

	if (visRegion != NULL)
		{
		XDestroyRegion(visRegion);
		}
}

/******************************************************************************
 GetVisibleRectGlobal

	Calculates the portion of the given rectangle that is actually visible,
	in global coords.  Returns kJTrue if the visible rectangle is not empty.

 ******************************************************************************/

JBoolean
JXContainer::GetVisibleRectGlobal
	(
	const JRect&	origRectG,
	JRect*			visRectG
	)
	const
{
	*visRectG = origRectG;

	const JXContainer* encl = this;
	while (encl != NULL)
		{
		JRect enclRect = encl->GetApertureGlobal();
		if (!JIntersection(*visRectG, enclRect, visRectG))
			{
			return kJFalse;
			}
		encl = encl->GetEnclosure();
		}

	// it isn't empty because JIntersection never returned kJFalse

	return kJTrue;
}

/******************************************************************************
 FindContainer

	Returns the JXContainer that contains the given point.

 ******************************************************************************/

JBoolean
JXContainer::FindContainer
	(
	const JPoint&	ptG,
	JXContainer**	container
	)
	const
{
	if (!IsVisible() || !IsActive() || !(GetFrameGlobal()).Contains(ptG))
		{
		*container = NULL;
		return kJFalse;
		}

	// check if enclosed object contains it

	if (itsEnclosedObjs != NULL)
		{
		const JSize objCount = itsEnclosedObjs->GetElementCount();
		for (JIndex i=1; i<=objCount; i++)
			{
			JXContainer* obj = itsEnclosedObjs->NthElement(i);
			if (obj->FindContainer(ptG, container))
				{
				return kJTrue;
				}
			}
		}

	// we contain it

	*container = const_cast<JXContainer*>(this);
	return kJTrue;
}

/******************************************************************************
 DispatchNewMouseEvent (private)

 ******************************************************************************/

void
JXContainer::DispatchNewMouseEvent
	(
	const int			eventType,
	const JPoint&		ptG,
	const JXMouseButton	button,
	const unsigned int	state
	)
{
	if (!IsVisible())
		{
		return;
		}

	// check if enclosed object wants it

	if (IsActive() && itsEnclosedObjs != NULL)
		{
		const JSize objCount = itsEnclosedObjs->GetElementCount();
		for (JIndex i=1; i<=objCount; i++)
			{
			JXContainer* obj = itsEnclosedObjs->NthElement(i);
			if (obj->IsVisible() &&
				(obj->GetFrameGlobal()).Contains(ptG))
				{
				obj->DispatchNewMouseEvent(eventType, ptG, button, state);
				return;
				}
			}
		}

	// handle it ourselves

	itsWindow->SetMouseContainer(this, ptG, state);

	if (eventType == EnterNotify)
		{
		// nothing to do
		}

	else if (eventType == ButtonPress)
		{
		assert( button != kJXNoButton );

		const JPoint pt = GlobalToLocal(ptG);
		const JXKeyModifiers modifiers(GetDisplay(), state);
		const JBoolean wantDrag = AcceptDrag(pt, button, modifiers);
		if (wantDrag)
			{
			itsWindow->SetWantDrag(kJTrue);
			const JSize clickCount = itsWindow->CountClicks(this, pt);
			MouseDown(pt, button, clickCount,
					  JXButtonStates(state), modifiers);	// can delete us
			}
		}

	else if (eventType == MotionNotify)
		{
		const JPoint pt = GlobalToLocal(ptG);
		const JXKeyModifiers modifiers(GetDisplay(), state);
		if (IsActive() && itsCursorAnim == NULL)
			{
			AdjustCursor(pt, modifiers);
			}
		MouseHere(pt, modifiers);
		}
}

/******************************************************************************
 DispatchMouseDrag (private)

 ******************************************************************************/

void
JXContainer::DispatchMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsIsDNDSourceFlag)
		{
		GetDNDManager()->HandleDND(pt, buttonStates, modifiers, (JXMouseButton) 0);
		}
	else
		{
		HandleMouseDrag(pt, buttonStates, modifiers);
		}
}

/******************************************************************************
 DispatchMouseUp (private)

 ******************************************************************************/

void
JXContainer::DispatchMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsIsDNDSourceFlag && buttonStates.AllOff())
		{
		GetDNDManager()->FinishDND();
		}
	else if (!itsIsDNDSourceFlag)
		{
		// can delete us

		HandleMouseUp(pt, button, buttonStates, modifiers);
		}
}

/******************************************************************************
 GetHint

	Returns kJTrue if this button has a hint.

 ******************************************************************************/

JBoolean
JXContainer::GetHint
	(
	JString* text
	)
	const
{
	if (itsHintMgr != NULL)
		{
		*text = itsHintMgr->GetText();
		return kJTrue;
		}
	else
		{
		text->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 SetHint

	Set the hint for this button.

 ******************************************************************************/

void
JXContainer::SetHint
	(
	const JCharacter* text
	)
{
	if (JStringEmpty(text))
		{
		ClearHint();
		}
	else if (itsHintMgr == NULL)
		{
		itsHintMgr = jnew JXHintManager(this, text);
		assert( itsHintMgr != NULL );
		}
	else
		{
		itsHintMgr->SetText(text);
		}
}

/******************************************************************************
 ClearHint

	Remove the hint for this button.

 ******************************************************************************/

void
JXContainer::ClearHint()
{
	jdelete itsHintMgr;
	itsHintMgr = NULL;
}

/******************************************************************************
 MouseEnter (private)

 ******************************************************************************/

void
JXContainer::MouseEnter()
{
	if (itsHintMgr != NULL)
		{
		itsHintMgr->Activate();
		}

	HandleMouseEnter();
}

/******************************************************************************
 MouseHere (private)

 ******************************************************************************/

void
JXContainer::MouseHere
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsHintMgr != NULL)
		{
		itsHintMgr->HandleMouseHere(pt);
		}

	HandleMouseHere(pt, modifiers);
}

/******************************************************************************
 MouseLeave (private)

 ******************************************************************************/

void
JXContainer::MouseLeave()
{
	if (itsHintMgr != NULL)
		{
		itsHintMgr->HandleMouseLeave();
		}

	HandleMouseLeave();
}

/******************************************************************************
 HandleMouseEnter (virtual protected)

 ******************************************************************************/

void
JXContainer::HandleMouseEnter()
{
}

/******************************************************************************
 HandleMouseHere (virtual protected)

 ******************************************************************************/

void
JXContainer::HandleMouseHere
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 HandleMouseLeave (virtual protected)

 ******************************************************************************/

void
JXContainer::HandleMouseLeave()
{
}

/******************************************************************************
 AcceptDrag (virtual protected)

 ******************************************************************************/

JBoolean
JXContainer::AcceptDrag
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers
	)
{
	return IsActive();
}

/******************************************************************************
 MouseDown (private)

 ******************************************************************************/

void
JXContainer::MouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsHintMgr != NULL)
		{
		itsHintMgr->Deactivate();
		}

	if (itsIsDNDSourceFlag &&
		(button == kJXButton4 || button == kJXButton5 || button == 6 || button == 7))
		{
		GetDNDManager()->HandleDND(pt, buttonStates, modifiers, button);
		}
	else if (!itsIsDNDSourceFlag)
		{
		// can delete us

		HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXContainer::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXContainer::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXContainer::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 HitSamePart (virtual protected)

	This default implementation always returns kJTrue.

 ******************************************************************************/

JBoolean
JXContainer::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 BecomeDNDSource (private)

 ******************************************************************************/

void
JXContainer::BecomeDNDSource()
{
	itsIsDNDSourceFlag = kJTrue;
}

/******************************************************************************
 FinishDNDSource (private)

 ******************************************************************************/

void
JXContainer::FinishDNDSource()
{
	itsIsDNDSourceFlag = kJFalse;
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Derived classes that accept drops should override this function and
	return kJTrue if they will accept the current drop.  If they return kJFalse,
	they will not receive any DND messages.

	source is non-NULL if the drag is between widgets in the same program.
	This provides a way for compound documents to identify drags between their
	various parts.

 ******************************************************************************/

JBoolean
JXContainer::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	return kJFalse;
}

/******************************************************************************
 DNDEnter (private)

 ******************************************************************************/

void
JXContainer::DNDEnter()
{
	if (!itsIsDNDTargetFlag)
		{
		itsIsDNDTargetFlag = kJTrue;
		Refresh();
		HandleDNDEnter();
		}
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
JXContainer::HandleDNDEnter()
{
}

/******************************************************************************
 DNDHere (private)

 ******************************************************************************/

void
JXContainer::DNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	HandleDNDHere(pt, source);
	itsWindow->Update();
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
JXContainer::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
}

/******************************************************************************
 DNDScroll (private)

 ******************************************************************************/

void
JXContainer::DNDScroll
	(
	const JPoint&			pt,
	const JXMouseButton		scrollButton,
	const JXKeyModifiers&	modifiers
	)
{
	HandleDNDScroll(pt, scrollButton, modifiers);
	itsWindow->Update();
}

/******************************************************************************
 HandleDNDScroll (virtual protected)

	This is called while the mouse is inside the widget, even if the widget
	does not currently accept the drop, because it might accept it after it
	is scrolled.

	Normally, this should only be implemented by JXScrollableWidget.

 ******************************************************************************/

void
JXContainer::HandleDNDScroll
	(
	const JPoint&			pt,
	const JXMouseButton		scrollButton,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 DNDLeave (private)

 ******************************************************************************/

void
JXContainer::DNDLeave()
{
	if (itsIsDNDTargetFlag)
		{
		HandleDNDLeave();
		itsIsDNDTargetFlag = kJFalse;
		Refresh();
		}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
JXContainer::HandleDNDLeave()
{
}

/******************************************************************************
 DNDDrop (private)

 ******************************************************************************/

void
JXContainer::DNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	if (itsIsDNDTargetFlag)
		{
		HandleDNDDrop(pt, typeList, action, time, source);
		itsIsDNDTargetFlag = kJFalse;
		Refresh();
		}
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

 ******************************************************************************/

void
JXContainer::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
}

/******************************************************************************
 DispatchClientMessage (protected)

	We send the message to every object since we don't know anything about
	who wants.  If somebody returns kJTrue, then it means that it was for
	them, so we stop passing it around.

 ******************************************************************************/

JBoolean
JXContainer::DispatchClientMessage
	(
	const XClientMessageEvent& clientMessage
	)
{
	if (HandleClientMessage(clientMessage))
		{
		return kJTrue;
		}

	if (itsEnclosedObjs != NULL)
		{
		const JSize objCount = itsEnclosedObjs->GetElementCount();
		for (JIndex i=1; i<=objCount; i++)
			{
			JXContainer* obj = itsEnclosedObjs->NthElement(i);
			if (obj->DispatchClientMessage(clientMessage))
				{
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleClientMessage (virtual protected)

	Derived class should return kJTrue if message was only for it.

 ******************************************************************************/

JBoolean
JXContainer::HandleClientMessage
	(
	const XClientMessageEvent& clientMessage
	)
{
	return kJFalse;
}

/******************************************************************************
 Show (virtual)

 ******************************************************************************/

void
JXContainer::Show()
{
	itsIsShowingFlag = kJTrue;

	if (itsEnclosure != NULL &&
		(!itsEnclosure->IsVisible() || itsEnclosure->itsIsHidingFlag))
		{
//		assert( !itsVisibleFlag );
		itsWasVisibleFlag = kJTrue;
		}
	else if (!itsVisibleFlag)
		{
		itsVisibleFlag = kJTrue;
		if (itsEnclosedObjs != NULL)
			{
			const JSize objCount = itsEnclosedObjs->GetElementCount();
			for (JIndex i=1; i<=objCount; i++)
				{
				JXContainer* widget = itsEnclosedObjs->NthElement(i);
				if (widget->itsWasVisibleFlag)
					{
					widget->Show();
					}
				}
			}

		if (itsEnclosure == NULL ||
			!(itsEnclosure->itsIsShowingFlag ||
			  itsEnclosure->itsIsHidingFlag))
			{
			Refresh();
			itsWindow->RecalcMouseContainer();
			}
		}

	itsIsShowingFlag = kJFalse;
}

/******************************************************************************
 Hide (virtual)

 ******************************************************************************/

void
JXContainer::Hide()
{
	itsIsHidingFlag = kJTrue;

	if (itsEnclosure != NULL && !itsEnclosure->IsVisible())
		{
//		assert( !itsVisibleFlag );
		itsWasVisibleFlag = kJFalse;
		}
	else if (itsVisibleFlag)
		{
		// Refresh() only works while visible

		if (itsEnclosure == NULL ||
			!(itsEnclosure->itsIsShowingFlag ||
			  itsEnclosure->itsIsHidingFlag))
			{
			Refresh();
			}

		if (itsEnclosedObjs != NULL)
			{
			const JSize objCount = itsEnclosedObjs->GetElementCount();
			for (JIndex i=1; i<=objCount; i++)
				{
				JXContainer* widget       = itsEnclosedObjs->NthElement(i);
				const JBoolean wasVisible = widget->IsVisible();
				if (wasVisible)
					{
					widget->Hide();
					}
				widget->itsWasVisibleFlag = wasVisible;	// must set this after hiding
				}
			}

		// we have to set our status after everybody else
		// to avoid the first if condition

		itsVisibleFlag    = kJFalse;
		itsWasVisibleFlag = kJTrue;

		if (itsEnclosure == NULL ||
			!(itsEnclosure->itsIsShowingFlag ||
			  itsEnclosure->itsIsHidingFlag))
			{
			itsWindow->RecalcMouseContainer();
			}
		}

	itsIsHidingFlag = kJFalse;
}

/******************************************************************************
 WouldBeVisible

 ******************************************************************************/

JBoolean
JXContainer::WouldBeVisible()
	const
{
	if (itsEnclosure == NULL || itsEnclosure->IsVisible())
		{
		return itsVisibleFlag;
		}
	else
		{
		return itsWasVisibleFlag;
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXContainer::Activate()
{
	itsIsActivatingFlag = kJTrue;

	if (itsEnclosure != NULL &&
		(!itsEnclosure->itsActiveFlag || itsEnclosure->itsIsDeactivatingFlag))
		{
//		assert( !itsActiveFlag );
		itsWasActiveFlag = kJTrue;
		}
	else if (!itsActiveFlag)
		{
		itsActiveFlag = kJTrue;
		if (itsEnclosedObjs != NULL)
			{
			const JSize objCount = itsEnclosedObjs->GetElementCount();
			for (JIndex i=1; i<=objCount; i++)
				{
				JXContainer* widget = itsEnclosedObjs->NthElement(i);
				if (widget->itsWasActiveFlag)
					{
					widget->Activate();
					}
				}
			}

		if (itsEnclosure == NULL ||
			!(itsEnclosure->itsIsActivatingFlag ||
			  itsEnclosure->itsIsDeactivatingFlag))
			{
			Refresh();
			itsWindow->RecalcMouseContainer();
			}
		}

	itsIsActivatingFlag = kJFalse;
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXContainer::Deactivate()
{
	itsIsDeactivatingFlag = kJTrue;

	if (itsEnclosure != NULL && !itsEnclosure->itsActiveFlag)
		{
//		assert( !itsActiveFlag );
		itsWasActiveFlag = kJFalse;
		}
	else if (itsActiveFlag)
		{
		if (itsEnclosedObjs != NULL)
			{
			const JSize objCount = itsEnclosedObjs->GetElementCount();
			for (JIndex i=1; i<=objCount; i++)
				{
				JXContainer* widget      = itsEnclosedObjs->NthElement(i);
				const JBoolean wasActive = widget->itsActiveFlag;
				if (wasActive)
					{
					widget->Deactivate();
					}
				widget->itsWasActiveFlag = wasActive;	// must set this after deactivating
				}
			}

		// we have to set our status after everybody else
		// to avoid the first if condition

		itsActiveFlag    = kJFalse;
		itsWasActiveFlag = kJTrue;

		if (itsEnclosure == NULL ||
			!(itsEnclosure->itsIsActivatingFlag ||
			  itsEnclosure->itsIsDeactivatingFlag))
			{
			Refresh();
			itsWindow->RecalcMouseContainer();
			}
		}

	itsIsDeactivatingFlag = kJFalse;
}

/******************************************************************************
 WouldBeActive

 ******************************************************************************/

JBoolean
JXContainer::WouldBeActive()
	const
{
	if (itsEnclosure == NULL || itsEnclosure->itsActiveFlag)
		{
		return itsActiveFlag;
		}
	else
		{
		return itsWasActiveFlag;
		}
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXContainer::Suspend()
{
	if (itsEnclosedObjs != NULL)
		{
		const JSize objCount = itsEnclosedObjs->GetElementCount();
		for (JIndex i=1; i<=objCount; i++)
			{
			(itsEnclosedObjs->NthElement(i))->Suspend();
			}
		}

	if (itsSuspendCount == 0)
		{
		Refresh();
		}
	itsSuspendCount++;
}

/******************************************************************************
 Resume (virtual)

 ******************************************************************************/

void
JXContainer::Resume()
{
	if (itsSuspendCount > 0)
		{
		itsSuspendCount--;
		}

	if (itsEnclosedObjs != NULL)
		{
		const JSize objCount = itsEnclosedObjs->GetElementCount();
		for (JIndex i=1; i<=objCount; i++)
			{
			(itsEnclosedObjs->NthElement(i))->Resume();
			}
		}

	if (itsSuspendCount == 0)
		{
		Refresh();
		if (itsEnclosure == NULL)
			{
			itsWindow->RecalcMouseContainer();
			}
		}
}

/******************************************************************************
 ShowCursor (protected)

 ******************************************************************************/

void
JXContainer::ShowCursor()
{
	itsCursorVisibleFlag = kJTrue;
	if (IsVisible())
		{
		itsWindow->DispatchCursor();
		}
}

/******************************************************************************
 HideCursor (protected)

 ******************************************************************************/

void
JXContainer::HideCursor()
{
	itsCursorVisibleFlag = kJFalse;
	if (IsVisible())
		{
		itsWindow->DispatchCursor();
		}
}

/******************************************************************************
 SetDefaultCursor (protected)

 ******************************************************************************/

void
JXContainer::SetDefaultCursor
	(
	const JCursorIndex cursor
	)
{
	itsDefCursor = cursor;
	if (IsVisible())
		{
		itsWindow->DispatchCursor();
		}
}

/******************************************************************************
 DispatchCursor

 ******************************************************************************/

void
JXContainer::DispatchCursor
	(
	const JPoint&			ptG,
	const JXKeyModifiers&	modifiers
	)
{
	// check if cursor is in enclosed object

	if (IsActive() && itsEnclosedObjs != NULL)
		{
		const JSize objCount = itsEnclosedObjs->GetElementCount();
		for (JIndex i=1; i<=objCount; i++)
			{
			JXContainer* obj = itsEnclosedObjs->NthElement(i);
			if (obj->IsVisible() && (obj->GetFrameGlobal()).Contains(ptG))
				{
				obj->DispatchCursor(ptG, modifiers);
				return;
				}
			}
		}

	// if not, display our own cursor

	ActivateCursor(ptG, modifiers);
}

/******************************************************************************
 ActivateCursor

 ******************************************************************************/

void
JXContainer::ActivateCursor
	(
	const JPoint&			ptG,
	const JXKeyModifiers&	modifiers
	)
{
	assert( IsVisible() );

	const JBoolean isActive = IsActive();
	if (isActive && itsCursorAnim != NULL)
		{
		itsCursorAnim->Activate();
		assert( itsCursorAnimTask != NULL );
		itsCursorAnimTask->Start();
		}
	else if (isActive)
		{
		AdjustCursor(GlobalToLocal(ptG), modifiers);
		}
	else
		{
		DisplayCursor(kJXInactiveCursor);
		}
}

/******************************************************************************
 DeactivateCursor

 ******************************************************************************/

void
JXContainer::DeactivateCursor()
{
	if (itsCursorAnim != NULL)
		{
		itsCursorAnim->Deactivate();
		itsCursorAnimTask->Stop();
		}
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Derived classes can override to adjust the cursor as appropriate.

	Should not be called if cursor animator is active.

 ******************************************************************************/

void
JXContainer::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	DisplayCursor(itsDefCursor);
}

/******************************************************************************
 DisplayCursor (protected)

	Should only be called by DispatchCursor() and AdjustCursor().

 ******************************************************************************/

void
JXContainer::DisplayCursor
	(
	const JCursorIndex index
	)
{
	itsCurrCursor = index;

	if (itsCursorVisibleFlag)
		{
		itsWindow->DisplayXCursor(itsCurrCursor);
		}
	else
		{
		itsWindow->DisplayXCursor(itsInvisibleCursor);
		}
}

/******************************************************************************
 CreateCursorAnimator (protected)

 ******************************************************************************/

JXCursorAnimator*
JXContainer::CreateCursorAnimator()
{
	jdelete itsCursorAnim;
	itsCursorAnim = jnew JXCursorAnimator(itsWindow);
	assert( itsCursorAnim != NULL );

	jdelete itsCursorAnimTask;
	itsCursorAnimTask = jnew JXCursorAnimationTask(itsCursorAnim);
	assert( itsCursorAnimTask != NULL );

	if (IsVisible())
		{
		itsWindow->DispatchCursor();
		}

	return itsCursorAnim;
}

/******************************************************************************
 AnimateCursor (protected)

 ******************************************************************************/

void
JXContainer::AnimateCursor()
	const
{
	if (itsCursorAnim != NULL)
		{
		itsCursorAnim->NextFrame();
		}
}

/******************************************************************************
 RemoveCursorAnimator (protected)

	Removes the current cursor animator and sets the cursor back to normal.

 ******************************************************************************/

void
JXContainer::RemoveCursorAnimator()
{
	jdelete itsCursorAnim;
	itsCursorAnim = NULL;

	jdelete itsCursorAnimTask;
	itsCursorAnimTask = NULL;

	if (IsVisible())
		{
		itsWindow->DispatchCursor();
		}
}

/******************************************************************************
 GetDisplay

 ******************************************************************************/

JXDisplay*
JXContainer::GetDisplay()
	const
{
	return itsWindow->GetDisplay();
}

/******************************************************************************
 GetColormap

 ******************************************************************************/

JXColormap*
JXContainer::GetColormap()
	const
{
	return itsWindow->GetColormap();
}

/******************************************************************************
 GetFontManager

 ******************************************************************************/

const JFontManager*
JXContainer::GetFontManager()
	const
{
	return (itsWindow->GetDisplay())->GetFontManager();
}

/******************************************************************************
 GetXFontManager

 ******************************************************************************/

const JXFontManager*
JXContainer::GetXFontManager()
	const
{
	return (itsWindow->GetDisplay())->GetXFontManager();
}

/******************************************************************************
 GetSelectionManager

 ******************************************************************************/

JXSelectionManager*
JXContainer::GetSelectionManager()
	const
{
	return (itsWindow->GetDisplay())->GetSelectionManager();
}

/******************************************************************************
 GetDNDManager

 ******************************************************************************/

JXDNDManager*
JXContainer::GetDNDManager()
	const
{
	return (itsWindow->GetDisplay())->GetDNDManager();
}

/******************************************************************************
 GetMenuManager

 ******************************************************************************/

JXMenuManager*
JXContainer::GetMenuManager()
	const
{
	return (itsWindow->GetDisplay())->GetMenuManager();
}

/******************************************************************************
 GlobalToLocal

 ******************************************************************************/

JRect
JXContainer::GlobalToLocal
	(
	const JRect& r
	)
	const
{
	const JPoint topLeft  = GlobalToLocal(r.left, r.top);
	const JPoint botRight = GlobalToLocal(r.right, r.bottom);
	return JRect(topLeft.y, topLeft.x, botRight.y, botRight.x);
}

/******************************************************************************
 LocalToGlobal

 ******************************************************************************/

JRect
JXContainer::LocalToGlobal
	(
	const JRect& r
	)
	const
{
	const JPoint topLeft  = LocalToGlobal(r.left, r.top);
	const JPoint botRight = LocalToGlobal(r.right, r.bottom);
	return JRect(topLeft.y, topLeft.x, botRight.y, botRight.x);
}

/******************************************************************************
 NotifyBoundsMoved (protected)

	Tell all enclosed objects that our Bounds has moved.

 ******************************************************************************/

void
JXContainer::NotifyBoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	if (dx != 0 || dy != 0)
		{
		BoundsMoved(dx,dy);

		if (itsEnclosedObjs != NULL)
			{
			const JSize objCount = itsEnclosedObjs->GetElementCount();
			for (JIndex i=1; i<=objCount; i++)
				{
				(itsEnclosedObjs->NthElement(i))->EnclosingBoundsMoved(dx,dy);
				}
			}
		}
}

/******************************************************************************
 NotifyBoundsResized (protected)

	Tell all enclosed objects that our Bounds has changed size.

 ******************************************************************************/

void
JXContainer::NotifyBoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (dw != 0 || dh != 0)
		{
		BoundsResized(dw,dh);

		if (itsEnclosedObjs != NULL)
			{
			const JSize objCount = itsEnclosedObjs->GetElementCount();
			for (JIndex i=1; i<=objCount; i++)
				{
				(itsEnclosedObjs->NthElement(i))->EnclosingBoundsResized(dw,dh);
				}
			}
		}
}

/******************************************************************************
 DeleteEnclosedObjects (protected)

 ******************************************************************************/

void
JXContainer::DeleteEnclosedObjects()
{
	if (itsEnclosedObjs != NULL)
		{
		itsGoingAwayFlag = kJTrue;		// ignore RemoveEnclosedObject messages
		itsEnclosedObjs->DeleteAll();
		itsGoingAwayFlag = kJFalse;
		jdelete itsEnclosedObjs;
		itsEnclosedObjs = NULL;
		}
}

/******************************************************************************
 AddEnclosedObject (private)

	By prepending the new object to the list, it is drawn last, and all
	other dispatchers consider it first.

 ******************************************************************************/

void
JXContainer::AddEnclosedObject
	(
	JXContainer* theObject
	)
{
	if (itsEnclosedObjs == NULL)
		{
		itsEnclosedObjs = jnew JPtrArray<JXContainer>(JPtrArrayT::kForgetAll);
		assert( itsEnclosedObjs != NULL );
		}

	if (!itsEnclosedObjs->Includes(theObject))
		{
		itsEnclosedObjs->Prepend(theObject);
		}
}

/*****************************************************************************
 RemoveEnclosedObject (private)

	We ignore this message if we are in the process of destructing.

 ******************************************************************************/

void
JXContainer::RemoveEnclosedObject
	(
	JXContainer* theObject
	)
{
	if (itsEnclosedObjs != NULL && !itsGoingAwayFlag)
		{
		itsEnclosedObjs->Remove(theObject);
		if (itsEnclosedObjs->IsEmpty())
			{
			jdelete itsEnclosedObjs;
			itsEnclosedObjs = NULL;
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

	We provide this as a gateway to JBroadcaster::Receive().

 ******************************************************************************/

void
JXContainer::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JBroadcaster::Receive(sender, message);
}

/******************************************************************************
 Menu ID routines (virtual)

 ******************************************************************************/

JBoolean
JXContainer::IsMenu()
	const
{
	return kJFalse;
}

JBoolean
JXContainer::IsMenuTable()
	const
{
	return kJFalse;
}

#include <JXFTCCell.h>
#include <JIntRange.h>

/******************************************************************************
 DebugExpandToFitContent (static)

 ******************************************************************************/

void
JXContainer::DebugExpandToFitContent
	(
	const JBoolean horiz
	)
{
	if (horiz)
		{
		theDebugHorizFTCFlag = kJTrue;
		}
	else
		{
		theDebugVertFTCFlag = kJTrue;
		}

	theDebugFTCFlag = kJTrue;
}

/******************************************************************************
 ExpandToFitContent

	Assumes that widgets are laid out in implicit, nested tables and do not
	overlap.  Discovers and constructs these implicit tables and expands
	the cells and their contents so text is not cut off.

 ******************************************************************************/

void
JXContainer::ExpandToFitContent()
{
	if (theDebugFTCFlag)
		{
		std::cout << "==========" << std::endl;
		std::cout << "ExpandToFitContent: " << GetFrameForExpandToFitContent() << ' ' << GetWindow()->GetTitle() << std::endl;
		}

	// expand horizontally - translations

	JXFTCCell* root = FTCBuildLayout(kJTrue);

	if (theDebugHorizFTCFlag)
		{
		return;
		}

	if (root != NULL)
		{
		root->Expand(kJTrue);
		jdelete root;
		}

	// expand vertically - translation font size

	root = FTCBuildLayout(kJFalse);

	if (theDebugVertFTCFlag)
		{
		return;
		}

	if (root != NULL)
		{
		root->Expand(kJFalse);
		jdelete root;
		}
}

/******************************************************************************
 FTCBuildLayout (protected)

	Assumes that widgets are laid out in implicit, nested tables and do not
	overlap.  Discovers and constructs these implicit tables:

	Each widget is enclosed in a JXFTCCell.  JXFTCCell's nest.

	1) Iterates to find cells that match exactly, alternating between
	   horizontal and vertical.  Cells are sorted by relevant dimension
	   from smallest to largest.

	2) When this can no longer merge any cells, turns off exact match.

	3) Iterates to find cells that merely have some kind of alignment,
	   alternating between horizontal and vertical.  Cells are sorted
	   by relevant position from smallest to largest.

	4) When this can no longer merge cells, returns the single root cell
	   or null, if it was not possible to collapse everything.

 ******************************************************************************/

JXFTCCell*
JXContainer::FTCBuildLayout
	(
	const JBoolean expandHorizontally
	)
	const
{
	if (theDebugFTCFlag)
		{
		std::cout << "----------" << std::endl;
		std::cout << "BuildLayout:" << std::endl;
		std::cout << "(expand " << (expandHorizontally ? "horiz" : "vert") << ")" << std::endl;
		}

	JPtrArray<JXContainer> objList(*itsEnclosedObjs, JPtrArrayT::kForgetAll),
						   fullObjList(JPtrArrayT::kForgetAll),
						   cellList(JPtrArrayT::kForgetAll);

	JPtrArrayIterator<JXContainer> iter(&objList);
	JBoolean horizontal = !expandHorizontally, exact = kJTrue;
	JSize count = 0, noChangeCount = 0;
	do {
		if (!exact)
			{
			objList.SetCompareFunction(horizontal ? FTCCompareHorizontally : FTCCompareVertically);
			objList.Sort();
			}

		const JSize objCount = objList.GetElementCount();
		fullObjList.CopyPointers(objList, JPtrArrayT::kForgetAll, kJFalse);

		if (theDebugFTCFlag)
			{
			std::cout << "+++ " << (horizontal ? "Horizontal" : "Vertical")
				<< " (iter: " << count
				<< "; cells: " << objCount
				<< "; exact: " << exact << ")" << std::endl;
			}

		JXContainer* obj;
		while (iter.Next(&obj))
			{
			if (theDebugFTCFlag)
				{
				std::cout << "widget: " << obj->GetFrameForExpandToFitContent() << ' ' << obj->ToString() << std::endl;
				}

			iter.RemovePrev();	// do not process it
			JXContainer* cell = FTCGroupAlignedObjects(obj, &objList, &fullObjList, horizontal, exact);
			cellList.AppendElement(cell);

			if (theDebugFTCFlag)
				{
				std::cout << "cell: " << cell->GetFrameForExpandToFitContent() << ' ' << cell->ToString() << std::endl << std::endl;
				}
			}
		objList.CopyPointers(cellList, JPtrArrayT::kForgetAll, kJFalse);	// resets iter
		cellList.RemoveAll();

		count++;
		horizontal = ! horizontal;

		const JBoolean noChange = JI2B( objCount == objList.GetElementCount() );
		if (noChange)
			{
			noChangeCount++;
			if (exact && noChangeCount >= 2)
				{
				exact = kJFalse;
				noChangeCount = 0;
				}
			else if (noChangeCount >= 2)
				{
				break;
				}
			}
		else
			{
			noChangeCount = 0;
			}
		}
		while (objList.GetElementCount() > 1 || count >= 100);	// checking count is paranoia

	if (objList.GetElementCount() == 1)
		{
		return dynamic_cast<JXFTCCell*>(objList.GetFirstElement());
		}
	else
		{
		if (!theDebugFTCFlag)
			{
			objList.DeleteAll();
			}

		return NULL;	// unable to enclose everything in a single table
		}
}

/******************************************************************************
 FTCGroupAlignedObjects (private)

	Finds all other objects that align horizontally or vertically with the
	given object.  Creates a cell for each one (if necesssary) and returns
	a container cell for the created cells.

	1) Iterates over unprocessed objects to find matches.

		a) If exact, relevant position & dimension must match.

		b) Otherwise, there must be some overlap and the containing
		   rectangle must not overlap other, unmatched objects.
		   Objects must be sorted by position from smallest to largest
		   to ensure that the closest objects are processed first.
		   Otherwise, one could get an incorrect "overlaps unmatched"
		   result.

	2) For exact match, iterates over all unmatched objects to find
	   "blocking" objects that break up the expanse of matched objects.

 ******************************************************************************/

inline JIntRange
ftcGetInterval
	(
	const JRect&	r,
	const JBoolean	horizontal
	)
{
	return JIntRange(
		horizontal ? r.top    : r.left,
		horizontal ? r.bottom : r.right);
}

// must be macro, because function cannot access member data
#define ftcReparentCell(cell,newParent) \
	cell->itsEnclosure->itsEnclosedObjs->Remove(cell); \
	cell->itsEnclosure = newParent; \
	cell->itsEnclosure->itsEnclosedObjs->AppendElement(cell);

JXFTCCell*
JXContainer::FTCGroupAlignedObjects
	(
	JXContainer*			target,
	JPtrArray<JXContainer>*	objList,
	JPtrArray<JXContainer>*	fullObjList,
	const JBoolean			horizontal,
	const JBoolean			exact
	)
	const
{
	const JBoolean vertical = ! horizontal;

	const JRect targetRect         = target->GetFrameForExpandToFitContent();
	const JIntRange targetInterval = ftcGetInterval(targetRect, horizontal);

	JXFTCCell* container =
		jnew JXFTCCell(NULL, target->GetEnclosure(),
					   horizontal ? JXFTCCell::kHorizontal : JXFTCCell::kVertical);
	assert( container != NULL );

	JPtrArray<JXFTCCell> cellList(JPtrArrayT::kForgetAll);

	JXFTCCell* cell = jnew JXFTCCell(target, container, JXFTCCell::kNoDirection);
	assert( cell != NULL );
	cellList.AppendElement(cell);

	JRect covering = cell->GetFrameForExpandToFitContent();

	JXContainer* obj;
	JRect rect;
	JIntRange interval, intersection;
	JPtrArray<JXContainer> matchedList(JPtrArrayT::kForgetAll);

	JPtrArrayIterator<JXContainer> objIter(objList);
	while (objIter.Next(&obj))
		{
		rect     = obj->GetFrameForExpandToFitContent();
		interval = ftcGetInterval(rect, horizontal);

		if ((exact && targetInterval != interval) ||
			(!exact &&
			 (!JIntersection(targetInterval, interval, &intersection) ||
			  FTCWillOverlapNonincludedWidget(target, obj, *fullObjList, matchedList))))
			{
			continue;
			}

		JXFTCCell* cellObj = dynamic_cast<JXFTCCell*>(obj);
		if (cellObj != NULL)
			{
			cell = cellObj;
			}
		else	// only create new cell to wrap real widget
			{
			cell = jnew JXFTCCell(obj, container, JXFTCCell::kNoDirection);
			assert( cell != NULL );
			}
		cellList.AppendElement(cell);
		matchedList.AppendElement(obj);

		if (theDebugFTCFlag)
			{
			std::cout << "match: " << cell->GetFrameForExpandToFitContent() << ' ' << obj->ToString() << std::endl;
			}
		}

	// exact matches must only count if there are no intervening, unmatched objects

	if (exact)
		{
		FTCTrimBlockedMatches(target, *fullObjList, matchedList, horizontal, &cellList);
		}

	// short-circuit if no additional cells found

	JXFTCCell* targetCell = dynamic_cast<JXFTCCell*>(target);
	if (cellList.GetElementCount() == 1 && targetCell != NULL)
		{
		jdelete container;	// deletes targetCell
		return targetCell;
		}
	else if (cellList.GetElementCount() == 1)
		{
		jdelete container;	// deletes targetCell

		targetCell = jnew JXFTCCell(target, target->GetEnclosure(), JXFTCCell::kNoDirection);
		assert( targetCell != NULL );
		return targetCell;
		}
	else if (targetCell != NULL)
		{
		ftcReparentCell(targetCell, container);

		// replace unnecessary cell
		cellList.DeleteElement(1);
		cellList.Prepend(targetCell);
		}

	// unwrap contained cells if same direction

	JPtrArrayIterator<JXFTCCell> cellIter(&cellList);
	if (!exact)
		{
		while (cellIter.Next(&cell))
			{
			if (cell->GetDirection() == container->GetDirection())
				{
				JPtrArrayIterator<JXContainer> i2(cell->itsEnclosedObjs);
				while (i2.Next(&obj))
					{
					JXFTCCell* c2 = dynamic_cast<JXFTCCell*>(obj);
					assert( c2 != NULL );
					ftcReparentCell(c2, container);
					cellList.InsertBefore(cell, c2);
					}
				objList->Remove(cell);
				fullObjList->Remove(cell);
				cellIter.DeletePrev();
				}
			}
		}

	// compute coverage and reparent cells to container

	cellIter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (cellIter.Next(&cell))
		{
		covering = JCovering(covering, cell->GetFrameForExpandToFitContent());

		ftcReparentCell(cell, container);

		// try to remove both, in case cell containing widget was matched
		objList->Remove(cell);
		objList->Remove(cell->GetWidget());
		}

	// return cell containing remaining cells

	container->Place(covering.left, covering.top);
	container->SetSize(covering.width(), covering.height());

	return container;
}

/******************************************************************************
 FTCWillOverlapNonincludedWidget (private)

	Computes the covering for obj1 & obj2 and checks if any unmatched obj
	intersects this covering.

 ******************************************************************************/

const JBoolean kDebugWillOverlapNonincludedWidget = kJFalse;

JBoolean
JXContainer::FTCWillOverlapNonincludedWidget
	(
	const JXContainer*				obj1,
	const JXContainer*				obj2,
	const JPtrArray<JXContainer>&	fullObjList,
	const JPtrArray<JXContainer>&	matchedList
	)
	const
{
	const JRect covering = JCovering(
		obj1->GetFrameForExpandToFitContent(),
		obj2->GetFrameForExpandToFitContent());

	if (kDebugWillOverlapNonincludedWidget && theDebugFTCFlag)
		{
		std::cout << "-----" << std::endl;
		std::cout << "covering: " << covering << std::endl;
		}

	JPtrArrayIterator<JXContainer> iter(fullObjList);
	JXContainer* obj;
	JRect r;
	while (iter.Next(&obj))
		{
		const JBoolean check = JI2B(
			obj != obj1 && obj != obj2 && !matchedList.Includes(obj));

		if (kDebugWillOverlapNonincludedWidget && theDebugFTCFlag && check)
			{
			std::cout << "check: " << obj->GetFrameForExpandToFitContent() << ' ' << obj->ToString() << std::endl;
			}

		if (check && JIntersection(covering, obj->GetFrameForExpandToFitContent(), &r))
			{
			if (kDebugWillOverlapNonincludedWidget && theDebugFTCFlag)
				{
				std::cout << "----- " << kJTrue << std::endl;
				}

			return kJTrue;
			}
		}

	if (kDebugWillOverlapNonincludedWidget && theDebugFTCFlag)
		{
		std::cout << "----- " << kJTrue << std::endl;
		}
	return kJFalse;
}

/******************************************************************************
 FTCTrimBlockedMatches (private)

	1) Find objects that break up the expanse of matched objects.

	2) Remove objects beyond the narrowest, contiguous range

 ******************************************************************************/

void
JXContainer::FTCTrimBlockedMatches
	(
	const JXContainer*				target,
	const JPtrArray<JXContainer>&	fullObjList,
	const JPtrArray<JXContainer>&	matchedList,
	const JBoolean					horizontal,
	JPtrArray<JXFTCCell>*			cellList
	)
	const
{
	const JBoolean vertical = ! horizontal;

	const JRect targetRect         = target->GetFrameForExpandToFitContent();
	const JIntRange targetInterval = ftcGetInterval(targetRect, horizontal);

	JXContainer *obj, *minBlocker = NULL, *maxBlocker = NULL;

	// find blockers

	JPtrArrayIterator<JXContainer> allObjIter(fullObjList);
	JIntRange intersection;
	while (allObjIter.Next(&obj))
		{
		if (obj == target || matchedList.Includes(obj))
			{
			continue;
			}

		const JRect rect         = obj->GetFrameForExpandToFitContent();
		const JIntRange interval = ftcGetInterval(rect, horizontal);

		const JBoolean overlaps = JI2B(
			JIntersection(targetInterval, interval, &intersection) &&
			intersection.GetLength() > 1);
		if (overlaps &&
			((horizontal && rect.right  < targetRect.left) ||
			 (vertical   && rect.bottom < targetRect.top )) &&
			(minBlocker == NULL ||
			 (horizontal && minBlocker->GetFrameForExpandToFitContent().right  < rect.right ) ||
			 (vertical   && minBlocker->GetFrameForExpandToFitContent().bottom < rect.bottom)))
			{
			minBlocker = obj;

			if (theDebugFTCFlag)
				{
				std::cout << "min block: " << obj->GetFrameForExpandToFitContent() << ' ' << obj->ToString() << std::endl;
				}
			}
		else if (overlaps &&
				 ((horizontal && targetRect.right  < rect.left) ||
				  (vertical   && targetRect.bottom < rect.top )) &&
				 (maxBlocker == NULL ||
				  (horizontal && rect.left < maxBlocker->GetFrameForExpandToFitContent().left) ||
				  (vertical   && rect.top  < maxBlocker->GetFrameForExpandToFitContent().top )))
			{
			maxBlocker = obj;

			if (theDebugFTCFlag)
				{
				std::cout << "max block: " << obj->GetFrameForExpandToFitContent() << ' ' << obj->ToString() << std::endl;
				}
			}
		}

	// ignore cells before minBlocker or after maxBlocker

	const JCoordinate min = (minBlocker == NULL ? 0 :
							 horizontal ? minBlocker->GetFrameForExpandToFitContent().right :
										  minBlocker->GetFrameForExpandToFitContent().bottom),
					  max = (maxBlocker == NULL ? 1e6 :
							 horizontal ? maxBlocker->GetFrameForExpandToFitContent().left :
										  maxBlocker->GetFrameForExpandToFitContent().top);

	JPtrArrayIterator<JXFTCCell> cellIter(cellList);
	JXFTCCell* cell;
	while (cellIter.Next(&cell))
		{
		const JRect rect = cell->GetFrameForExpandToFitContent();
		if ((horizontal && (rect.right  < min || max < rect.left)) ||
			(vertical   && (rect.bottom < min || max < rect.top )))
			{
			if (theDebugFTCFlag)
				{
				std::cout << "blocked: " << cell->GetFrameForExpandToFitContent() << ' ' << cell->ToString() << std::endl;
				}

			cellIter.RemovePrev();
			if (cell->GetWidget() != NULL)
				{
				jdelete cell;	// delete cell for widget that needs to be re-processed
				}
			}
		}
}

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

	By default, return zero, which means no change.

 ******************************************************************************/

JCoordinate
JXContainer::GetFTCMinContentSize
	(
	const JBoolean horizontal
	)
	const
{
	return 0;
}

/******************************************************************************
 ExpandToFTCMinContentSize (virtual protected)

	By default, run fit-to-content on contained objects, if any.

	Returns the amount by which it expanded.

 ******************************************************************************/

JCoordinate
JXContainer::ExpandToFTCMinContentSize
	(
	const JBoolean horizontal
	)
{
	JXFTCCell* root = FTCBuildLayout(horizontal);

	JCoordinate v = 0;
	if (root != NULL)
		{
		v = root->Expand(horizontal);
		jdelete root;
		}
	return v;
}

/******************************************************************************
 ExpandForFTC (virtual protected)

	By default, run on contained objects, if any.

 ******************************************************************************/

void
JXContainer::ExpandForFTC
	(
	const JCoordinate	delta,
	const JBoolean		horizontal
	)
{
	JXContainer* root = FTCBuildLayout(horizontal);
	if (root != NULL)
		{
		root->ExpandForFTC(delta, horizontal);
		jdelete root;
		}
}

/******************************************************************************
 GetFrameForExpandToFitContent (virtual protected)

	By default, returns the frame.

 ******************************************************************************/

JRect
JXContainer::GetFrameForExpandToFitContent()
	const
{
	return GetFrameGlobal();
}

/******************************************************************************
 FTC comparison functions (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXContainer::FTCCompareHorizontally
	(
	JXContainer* const & w1,
	JXContainer* const & w2
	)
{
	return JCompareCoordinates(
		w1->GetFrameForExpandToFitContent().left,
		w2->GetFrameForExpandToFitContent().left);
}

JOrderedSetT::CompareResult
JXContainer::FTCCompareVertically
	(
	JXContainer* const & w1,
	JXContainer* const & w2
	)
{
	return JCompareCoordinates(
		w1->GetFrameForExpandToFitContent().top,
		w2->GetFrameForExpandToFitContent().top);
}
