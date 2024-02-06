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
	should override WillAcceptDrop() and return true if they will accept
	at least one of the given data types.  If the function returns false,
	the object will not receive any DND messages.

	Since Widgets are the ones that actually own data, only Widgets can
	initiate DND (via BeginDND()).  Containers can accept drops, however.
	This is why all the DND functions other than BeginDND() are defined
	by JXContainer.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-2009 by John Lindal.

 ******************************************************************************/

#include "JXContainer.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXWindowPainter.h"
#include "JXCursorAnimator.h"
#include "JXFunctionTask.h"
#include "JXDNDManager.h"
#include "JXHintManager.h"
#include "JXFTCCell.h"
#include "jXUtil.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JListUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

bool JXContainer::theDebugFTCFlag                         = false;
bool JXContainer::theDebugHorizFTCFlag                    = false;
bool JXContainer::theDebugVertFTCFlag                     = false;
bool JXContainer::theDebugFTCNoopExaminations             = false;
bool JXContainer::theDebugFTCWillOverlapNonincludedWidget = false;
std::ostream* JXContainer::theDebugFTCLogBuffer           = nullptr;

/******************************************************************************
 Constructor (protected)

	If enclosure is nullptr, we are a window.

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
	assert( display != nullptr && window != nullptr );

	itsWindow          = window;
	itsEnclosure       = enclosure;
	itsEnclosedObjs    = nullptr;
	itsActiveFlag      = false;
	itsWasActiveFlag   = true;
	itsVisibleFlag     = false;
	itsWasVisibleFlag  = true;
	itsSuspendCount    = 0;
	itsIsDNDSourceFlag = false;
	itsIsDNDTargetFlag = false;
	itsGoingAwayFlag   = false;

	if (enclosure != nullptr)
	{
		// inherit settings from enclosure

		itsActiveFlag   = enclosure->itsActiveFlag;
		itsVisibleFlag  = enclosure->itsVisibleFlag;
		itsSuspendCount = enclosure->itsSuspendCount;
	}

	itsIsShowingFlag      = false;
	itsIsHidingFlag       = false;
	itsIsActivatingFlag   = false;
	itsIsDeactivatingFlag = false;

	itsDefCursor         = kJXDefaultCursor;
	itsInvisibleCursor   = JXGetInvisibleCursor(display);
	itsCursorVisibleFlag = true;
	itsCurrCursor        = kJXDefaultCursor;
	itsCursorAnim        = nullptr;
	itsCursorAnimTask    = nullptr;

	itsHintMgr = nullptr;

	if (itsEnclosure != nullptr)
	{
		itsEnclosure->AddEnclosedObject(this);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXContainer::~JXContainer()
{
	if (itsEnclosure != nullptr)
	{
		itsEnclosure->RemoveEnclosedObject(this);
	}

	JXContainer* c;
	if (itsWindow->GetMouseContainer(&c) && c == this)
	{
		itsWindow->SetMouseContainer(nullptr, JPoint(0,0), 0);
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
	itsWindow->BufferDrawing(true);
}

/******************************************************************************
 IsAncestor

	Returns true if we are an ancestor of the given object.

 ******************************************************************************/

bool
JXContainer::IsAncestor
	(
	const JXContainer* obj
	)
	const
{
	const JXContainer* encl = obj->itsEnclosure;
	while (true)
	{
		if (encl == this)
		{
			return true;
		}
		else if (encl == nullptr)
		{
			return false;
		}

		encl = encl->itsEnclosure;
	}
}

/******************************************************************************
 SetEnclosure

	Returns false if we are a JXWindow or obj is in a different JXWindow.

	Before we can implement this, we need to define pure virtual
	EnclosureChanged() so derived classes can update their state, e.g.,
	JXRadioButton needs to know its JXRadioGroup.  This may be very
	difficult if said state is application specific and can only be
	computed at a higher level, e.g., JXDirector or JXApplication.

 ******************************************************************************/
/*
bool
JXContainer::SetEnclosure
	(
	JXContainer* obj
	)
{
	if (itsEnclosure != nullptr && obj->GetWindow() == itsWindow)
	{
		itsEnclosure->RemoveEnclosedObject(this);
		itsEnclosure = obj;
		itsEnclosure->AddEnclosedObject(this);
		itsWindow->Refresh();
		return true;
	}
	else
	{
		return false;
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
	const bool apVisible =
		JIntersection(apClipRectG, origFrameG, &apClipRectG);

	// draw enclosed objects first, if visible

	Region visRegion                      = nullptr;
	JPtrArray<JXFTCCell>* ftcRootCellList = nullptr;
	if (apVisible && itsEnclosedObjs != nullptr)
	{
		XRectangle xClipRect = JXJToXRect(apClipRectG);
		visRegion            = JXRectangleRegion(&xClipRect);

		// draw visible objects in reverse order so all the
		// other routines can check them in the normal order

		const JSize objCount = itsEnclosedObjs->GetItemCount();
		for (JIndex i=objCount; i>=1; i--)
		{
			JXContainer* obj = itsEnclosedObjs->GetItem(i);
			if (theDebugFTCFlag &&
				dynamic_cast<JXFTCCell*>(obj) != nullptr)
			{
				if (ftcRootCellList == nullptr)
				{
					ftcRootCellList = jnew JPtrArray<JXFTCCell>(JPtrArrayT::kForgetAll);
					assert( ftcRootCellList != nullptr );
				}

				ftcRootCellList->AppendItem(dynamic_cast<JXFTCCell*>(obj));
				continue;	// let JXWindow draw the background around real widgets
			}

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

	const JRect frameRectG = GetFrameGlobal();
	if (frameRectG != apClipRectG)
	{
		XRectangle xFrameRect    = JXJToXRect(frameRectG);
		Region borderRegion      = JXRectangleRegion(&xFrameRect);
		XRectangle xApertureRect = JXJToXRect(apClipRectG);
		JXSubtractRectFromRegion(borderRegion, &xApertureRect, borderRegion);
		p.Reset(clipRectG, borderRegion);
		DrawBorder(p, GetFrameLocal());
		XDestroyRegion(borderRegion);
	}

	// draw background and contents, if visible

	if (!theDebugFTCFlag && visRegion != nullptr && XEmptyRegion(visRegion))
	{
		XRectangle xFrameRect = JXJToXRect(frameRectG);
		Region borderRegion   = JXRectangleRegion(&xFrameRect);
		p.Reset(clipRectG, borderRegion);
		DrawOver(p, GlobalToLocal(frameRectG));
		XDestroyRegion(borderRegion);

		// If nothing else is visible, we can quit now.

		XDestroyRegion(visRegion);
		return;
	}

	if (apVisible)
	{
		Region origDefClipRegion = nullptr;
		if (visRegion != nullptr)
		{
			Region region;
			if (p.GetDefaultClipRegion(&region))
			{
				origDefClipRegion = JXCopyRegion(region);
			}
			p.SetDefaultClipRegion(visRegion);	// in case derived class calls Reset()
			p.Reset(apClipRectG, visRegion);
		}
		else
		{
			p.Reset(apClipRectG);
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

		if (origDefClipRegion != nullptr)
		{
			p.SetDefaultClipRegion(origDefClipRegion);
			XDestroyRegion(origDefClipRegion);
		}

		// enable drawing over contained widgets

		XRectangle xFrameRect = JXJToXRect(frameRectG);
		Region borderRegion   = JXRectangleRegion(&xFrameRect);
		p.Reset(clipRectG, borderRegion);
		DrawOver(p, GlobalToLocal(frameRectG));
		XDestroyRegion(borderRegion);
	}

	if (ftcRootCellList != nullptr)	// overlay FTC on top of window contents
	{
		JPtrArrayIterator<JXFTCCell> iter(ftcRootCellList);
		JXFTCCell* root;
		while (iter.Next(&root))
		{
			root->DrawAll(p, apClipRectG);
		}

		jdelete ftcRootCellList;
		ftcRootCellList = nullptr;
	}

	// clean up

	if (visRegion != nullptr)
	{
		XDestroyRegion(visRegion);
	}
}

/******************************************************************************
 DrawOver (virtual protected)

	Allows drawing over enclosed widgets

 ******************************************************************************/

void
JXContainer::DrawOver
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 GetVisibleRectGlobal

	Calculates the portion of the given rectangle that is actually visible,
	in global coords.  Returns true if the visible rectangle is not empty.

 ******************************************************************************/

bool
JXContainer::GetVisibleRectGlobal
	(
	const JRect&	origRectG,
	JRect*			visRectG
	)
	const
{
	*visRectG = origRectG;

	const JXContainer* encl = this;
	while (encl != nullptr)
	{
		JRect enclRect = encl->GetApertureGlobal();
		if (!JIntersection(*visRectG, enclRect, visRectG))
		{
			return false;
		}
		encl = encl->GetEnclosure();
	}

	// it isn't empty because JIntersection never returned false

	return true;
}

/******************************************************************************
 ForEach

	Calls the given function for each enclosed object.  If recurse,
	also calls ForEach() on each enclosed object.

 ******************************************************************************/

void
JXContainer::ForEach
	(
	const std::function<void(JXContainer*)>&	f,
	const bool									recurse
	)
{
	if (itsEnclosedObjs != nullptr)
	{
		for (auto* obj : *itsEnclosedObjs)
		{
			f(obj);
			if (recurse)
			{
				obj->ForEach(f, recurse);
			}
		}
	}
}

void
JXContainer::ForEach
	(
	const std::function<void(const JXContainer*)>&	f,
	const bool										recurse
	)
	const
{
	if (itsEnclosedObjs != nullptr)
	{
		for (const auto* obj : *itsEnclosedObjs)
		{
			f(obj);
			if (recurse)
			{
				obj->ForEach(f, recurse);
			}
		}
	}
}

/******************************************************************************
 AnyOf

	Calls the given function for each enclosed object.  If recurse,
	also calls AnyOf() on each enclosed object.

	If the function returns true, immediately returns true.  Otherwise,
	returns false.

 ******************************************************************************/

bool
JXContainer::AnyOf
	(
	const std::function<bool(const JXContainer*)>&	f,
	const bool										recurse
	)
	const
{
	if (itsEnclosedObjs != nullptr)
	{
		for (const auto* obj : *itsEnclosedObjs)
		{
			if (f(obj) || (recurse && obj->AnyOf(f, recurse)))
			{
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 FindContainer

	Returns the JXContainer that contains the given point.

 ******************************************************************************/

bool
JXContainer::FindContainer
	(
	const JPoint&	ptG,
	JXContainer**	container
	)
	const
{
	if (!IsVisible() || !IsActive() || !(GetFrameGlobal()).Contains(ptG))
	{
		*container = nullptr;
		return false;
	}

	// check if enclosed object contains it

	if (itsEnclosedObjs != nullptr)
	{
		for (const auto* obj : *itsEnclosedObjs)
		{
			if (obj->FindContainer(ptG, container))
			{
				return true;
			}
		}
	}

	// we contain it

	*container = const_cast<JXContainer*>(this);
	return true;
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

	if (IsActive() && itsEnclosedObjs != nullptr &&
		!StealMouse(eventType, ptG, button, state))
	{
		for (auto* obj : *itsEnclosedObjs)
		{
			if (obj->IsVisible() &&
				obj->GetFrameGlobal().Contains(ptG))
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
		const bool wantDrag = AcceptDrag(pt, button, modifiers);
		if (wantDrag)
		{
			itsWindow->SetWantDrag(true);
			const JSize clickCount = itsWindow->CountClicks(this, pt);
			MouseDown(pt, button, clickCount,
					  JXButtonStates(state), modifiers);	// can delete us
		}
	}

	else if (eventType == MotionNotify)
	{
		const JPoint pt = GlobalToLocal(ptG);
		const JXKeyModifiers modifiers(GetDisplay(), state);
		if (IsActive() && itsCursorAnim == nullptr)
		{
			AdjustCursor(pt, modifiers);
		}
		MouseHere(pt, modifiers);
	}
}

/******************************************************************************
 StealMouse (virtual protected)

	Derived classes can override to grab a mouse down, preventing enclosed
	objects from getting it.

 ******************************************************************************/

bool
JXContainer::StealMouse
	(
	const int			eventType,
	const JPoint&		ptG,
	const JXMouseButton	button,
	const unsigned int	state
	)
{
	return false;
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

 ******************************************************************************/

bool
JXContainer::GetHint
	(
	JString* text
	)
	const
{
	if (itsHintMgr != nullptr)
	{
		*text = itsHintMgr->GetText();
		return true;
	}
	else
	{
		text->Clear();
		return false;
	}
}

/******************************************************************************
 SetHint

 ******************************************************************************/

void
JXContainer::SetHint
	(
	const JString& text
	)
{
	if (text.IsEmpty())
	{
		ClearHint();
	}
	else if (itsHintMgr == nullptr)
	{
		itsHintMgr = jnew JXHintManager(this, text);
	}
	else
	{
		itsHintMgr->SetText(text);
	}
}

/******************************************************************************
 ClearHint

 ******************************************************************************/

void
JXContainer::ClearHint()
{
	jdelete itsHintMgr;
	itsHintMgr = nullptr;
}

/******************************************************************************
 MouseEnter (private)

 ******************************************************************************/

void
JXContainer::MouseEnter()
{
	if (itsHintMgr != nullptr)
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
	if (itsHintMgr != nullptr)
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
	if (itsHintMgr != nullptr)
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

bool
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
	if (itsHintMgr != nullptr)
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

	This default implementation always returns true.

 ******************************************************************************/

bool
JXContainer::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	return true;
}

/******************************************************************************
 BecomeDNDSource (private)

 ******************************************************************************/

void
JXContainer::BecomeDNDSource()
{
	itsIsDNDSourceFlag = true;
}

/******************************************************************************
 FinishDNDSource (private)

 ******************************************************************************/

void
JXContainer::FinishDNDSource()
{
	itsIsDNDSourceFlag = false;
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Derived classes that accept drops should override this function and
	return true if they will accept the current drop.  If they return false,
	they will not receive any DND messages.

	source is non-nullptr if the drag is between widgets in the same program.
	This provides a way for compound documents to identify drags between their
	various parts.

 ******************************************************************************/

bool
JXContainer::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	return false;
}

/******************************************************************************
 DNDEnter (private)

 ******************************************************************************/

void
JXContainer::DNDEnter()
{
	if (!itsIsDNDTargetFlag)
	{
		itsIsDNDTargetFlag = true;
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
		itsIsDNDTargetFlag = false;
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
		itsIsDNDTargetFlag = false;
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
	who wants.  If somebody returns true, then it means that it was for
	them, so we stop passing it around.

 ******************************************************************************/

bool
JXContainer::DispatchClientMessage
	(
	const XClientMessageEvent& clientMessage
	)
{
	if (HandleClientMessage(clientMessage))
	{
		return true;
	}

	if (itsEnclosedObjs != nullptr)
	{
		for (auto* obj : *itsEnclosedObjs)
		{
			if (obj->DispatchClientMessage(clientMessage))
			{
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 HandleClientMessage (virtual protected)

	Derived class should return true if message was only for it.

 ******************************************************************************/

bool
JXContainer::HandleClientMessage
	(
	const XClientMessageEvent& clientMessage
	)
{
	return false;
}

/******************************************************************************
 Show (virtual)

 ******************************************************************************/

void
JXContainer::Show()
{
	itsIsShowingFlag = true;

	if (itsEnclosure != nullptr &&
		(!itsEnclosure->IsVisible() || itsEnclosure->itsIsHidingFlag))
	{
//		assert( !itsVisibleFlag );
		itsWasVisibleFlag = true;
	}
	else if (!itsVisibleFlag)
	{
		itsVisibleFlag = true;
		if (itsEnclosedObjs != nullptr)
		{
			for (auto* obj : *itsEnclosedObjs)
			{
				if (obj->itsWasVisibleFlag)
				{
					obj->Show();
				}
			}
		}

		if (itsEnclosure == nullptr ||
			!(itsEnclosure->itsIsShowingFlag ||
			  itsEnclosure->itsIsHidingFlag))
		{
			Refresh();
			itsWindow->RecalcMouseContainer();
		}
	}

	itsIsShowingFlag = false;
}

/******************************************************************************
 Hide (virtual)

 ******************************************************************************/

void
JXContainer::Hide()
{
	itsIsHidingFlag = true;

	if (itsEnclosure != nullptr && !itsEnclosure->IsVisible())
	{
//		assert( !itsVisibleFlag );
		itsWasVisibleFlag = false;
	}
	else if (itsVisibleFlag)
	{
		// Refresh() only works while visible

		if (itsEnclosure == nullptr ||
			!(itsEnclosure->itsIsShowingFlag ||
			  itsEnclosure->itsIsHidingFlag))
		{
			Refresh();
		}

		if (itsEnclosedObjs != nullptr)
		{
			for (auto* obj : *itsEnclosedObjs)
			{
				const bool wasVisible = obj->IsVisible();
				if (wasVisible)
				{
					obj->Hide();
				}
				obj->itsWasVisibleFlag = wasVisible;	// must set this after hiding
			}
		}

		// we have to set our status after everybody else
		// to avoid the first if condition

		itsVisibleFlag    = false;
		itsWasVisibleFlag = true;

		if (itsEnclosure == nullptr ||
			!(itsEnclosure->itsIsShowingFlag ||
			  itsEnclosure->itsIsHidingFlag))
		{
			itsWindow->RecalcMouseContainer();
		}
	}

	itsIsHidingFlag = false;
}

/******************************************************************************
 WouldBeVisible

 ******************************************************************************/

bool
JXContainer::WouldBeVisible()
	const
{
	if (itsEnclosure == nullptr || itsEnclosure->IsVisible())
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
	itsIsActivatingFlag = true;

	if (itsEnclosure != nullptr &&
		(!itsEnclosure->itsActiveFlag || itsEnclosure->itsIsDeactivatingFlag))
	{
//		assert( !itsActiveFlag );
		itsWasActiveFlag = true;
	}
	else if (!itsActiveFlag)
	{
		itsActiveFlag = true;
		if (itsEnclosedObjs != nullptr)
		{
			for (auto* obj : *itsEnclosedObjs)
			{
				if (obj->itsWasActiveFlag)
				{
					obj->Activate();
				}
			}
		}

		if (itsEnclosure == nullptr ||
			!(itsEnclosure->itsIsActivatingFlag ||
			  itsEnclosure->itsIsDeactivatingFlag))
		{
			Refresh();
			itsWindow->RecalcMouseContainer();
		}
	}

	itsIsActivatingFlag = false;
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXContainer::Deactivate()
{
	itsIsDeactivatingFlag = true;

	if (itsEnclosure != nullptr && !itsEnclosure->itsActiveFlag)
	{
//		assert( !itsActiveFlag );
		itsWasActiveFlag = false;
	}
	else if (itsActiveFlag)
	{
		if (itsEnclosedObjs != nullptr)
		{
			for (auto* obj : *itsEnclosedObjs)
			{
				const bool wasActive = obj->itsActiveFlag;
				if (wasActive)
				{
					obj->Deactivate();
				}
				obj->itsWasActiveFlag = wasActive;	// must set this after deactivating
			}
		}

		// we have to set our status after everybody else
		// to avoid the first if condition

		itsActiveFlag    = false;
		itsWasActiveFlag = true;

		if (itsEnclosure == nullptr ||
			!(itsEnclosure->itsIsActivatingFlag ||
			  itsEnclosure->itsIsDeactivatingFlag))
		{
			Refresh();
			itsWindow->RecalcMouseContainer();
		}
	}

	itsIsDeactivatingFlag = false;
}

/******************************************************************************
 WouldBeActive

 ******************************************************************************/

bool
JXContainer::WouldBeActive()
	const
{
	if (itsEnclosure == nullptr || itsEnclosure->itsActiveFlag)
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
	if (itsEnclosedObjs != nullptr)
	{
		for (auto* obj : *itsEnclosedObjs)
		{
			obj->Suspend();
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

	if (itsEnclosedObjs != nullptr)
	{
		for (auto* obj : *itsEnclosedObjs)
		{
			obj->Resume();
		}
	}

	if (itsSuspendCount == 0)
	{
		Refresh();
		if (itsEnclosure == nullptr)
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
	itsCursorVisibleFlag = true;
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
	itsCursorVisibleFlag = false;
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

	if (IsActive() && itsEnclosedObjs != nullptr)
	{
		for (auto* obj : *itsEnclosedObjs)
		{
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

	const bool isActive = IsActive();
	if (isActive && itsCursorAnim != nullptr)
	{
		itsCursorAnim->Activate();
		assert( itsCursorAnimTask != nullptr );
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
	if (itsCursorAnim != nullptr)
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

	jdelete itsCursorAnimTask;
	itsCursorAnimTask = jnew JXFunctionTask(JXCursorAnimator::kmsecPerFrame, std::bind(&JXCursorAnimator::NextFrame, itsCursorAnim));
	assert( itsCursorAnimTask != nullptr );

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
	if (itsCursorAnim != nullptr)
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
	itsCursorAnim = nullptr;

	jdelete itsCursorAnimTask;
	itsCursorAnimTask = nullptr;

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
 GetFontManager

 ******************************************************************************/

JFontManager*
JXContainer::GetFontManager()
	const
{
	return itsWindow->GetDisplay()->GetFontManager();
}

/******************************************************************************
 GetXFontManager

 ******************************************************************************/

JXFontManager*
JXContainer::GetXFontManager()
	const
{
	return itsWindow->GetDisplay()->GetXFontManager();
}

/******************************************************************************
 GetSelectionManager

 ******************************************************************************/

JXSelectionManager*
JXContainer::GetSelectionManager()
	const
{
	return itsWindow->GetDisplay()->GetSelectionManager();
}

/******************************************************************************
 GetDNDManager

 ******************************************************************************/

JXDNDManager*
JXContainer::GetDNDManager()
	const
{
	return itsWindow->GetDisplay()->GetDNDManager();
}

/******************************************************************************
 GetMenuManager

 ******************************************************************************/

JXMenuManager*
JXContainer::GetMenuManager()
	const
{
	return itsWindow->GetDisplay()->GetMenuManager();
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

		if (itsEnclosedObjs != nullptr)
		{
			for (auto* obj : *itsEnclosedObjs)
			{
				obj->EnclosingBoundsMoved(dx,dy);
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

		if (itsEnclosedObjs != nullptr)
		{
			for (auto* obj : *itsEnclosedObjs)
			{
				obj->EnclosingBoundsResized(dw,dh);
			}
		}
	}
}

/******************************************************************************
 GetEnclosedObjects (protected)

	Returns false if there are no enclosed objects.

	Caller must jdelete the iterator.

 ******************************************************************************/

bool
JXContainer::GetEnclosedObjects
	(
	JPtrArrayIterator<JXContainer>** iter
	)
	const
{
	if (itsEnclosedObjs != nullptr)
	{
		*iter = jnew JPtrArrayIterator<JXContainer>(*itsEnclosedObjs);
	}
	else
	{
		*iter = nullptr;
	}

	return *iter != nullptr;
}

/******************************************************************************
 DeleteEnclosedObjects (protected)

 ******************************************************************************/

void
JXContainer::DeleteEnclosedObjects()
{
	if (itsEnclosedObjs != nullptr)
	{
		itsGoingAwayFlag = true;		// ignore RemoveEnclosedObject messages
		itsEnclosedObjs->DeleteAll();
		itsGoingAwayFlag = false;
		jdelete itsEnclosedObjs;
		itsEnclosedObjs = nullptr;
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
	if (itsEnclosedObjs == nullptr)
	{
		itsEnclosedObjs = jnew JPtrArray<JXContainer>(JPtrArrayT::kForgetAll);
		assert( itsEnclosedObjs != nullptr );
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
	if (itsEnclosedObjs != nullptr && !itsGoingAwayFlag)
	{
		itsEnclosedObjs->Remove(theObject);
		if (itsEnclosedObjs->IsEmpty())
		{
			jdelete itsEnclosedObjs;
			itsEnclosedObjs = nullptr;
		}
	}
}

/******************************************************************************
 Menu ID routines (virtual)

 ******************************************************************************/

bool
JXContainer::IsMenu()
	const
{
	return false;
}

bool
JXContainer::IsMenuTable()
	const
{
	return false;
}

#include "JXFTCCell.h"
#include <jx-af/jcore/JRange.h>

/******************************************************************************
 DebugExpandToFitContent (static)

 ******************************************************************************/

void
JXContainer::DebugExpandToFitContent
	(
	const bool horiz
	)
{
	if (horiz)
	{
		theDebugHorizFTCFlag = true;
	}
	else
	{
		theDebugVertFTCFlag = true;
	}

	theDebugFTCFlag = true;
}

/******************************************************************************
 DebugExpandToFitContentExtras (static)

 ******************************************************************************/

void
JXContainer::DebugExpandToFitContentExtras
	(
	const bool noop,
	const bool overlap
	)
{
	theDebugFTCNoopExaminations             = noop;
	theDebugFTCWillOverlapNonincludedWidget = overlap;
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
		GetFTCLog() << "==========" << std::endl;
		GetFTCLog() << "ExpandToFitContent: " << GetFrameForFTC() << ' ' << GetWindow()->GetTitle() << std::endl;
	}

	JCoordinate dw=0, dh=0;

	// expand horizontally - translations

	JXFTCCell* root;
	if (FTCBuildLayout(true, &root))
	{
		const JCoordinate w = GetBoundsWidth(),
						  b = w - root->GetBoundsWidth(),
						  v = root->Expand(true) + b;

		if (v != w)
		{
			if (theDebugFTCFlag)
			{
				GetFTCLog() << "Will resize window width: " << w << " -> " << v << std::endl;
			}

			dw = v - w;
		}
	}

	FTCAdjustSize(dw, 0);
	if (theDebugHorizFTCFlag)
	{
		return;
	}
	jdelete root;

	// expand vertically - translation font size

	if (FTCBuildLayout(false, &root))
	{
		const JCoordinate h = GetBoundsHeight(),
						  b = h - root->GetBoundsHeight(),
						  v = root->Expand(false) + b;

		if (v != h)
		{
			if (theDebugFTCFlag)
			{
				GetFTCLog() << "Will resize window height: " << h << " -> " << v << std::endl;
			}

			dh = v - h;
		}
	}

	FTCAdjustSize(0, dh);
	if (theDebugVertFTCFlag)
	{
		return;
	}
	jdelete root;
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

bool
JXContainer::FTCBuildLayout
	(
	const bool	expandHorizontally,
	JXFTCCell**	root
	)
	const
{
	if (itsEnclosedObjs == nullptr)
	{
		*root = nullptr;
		return false;
	}

	if (theDebugFTCFlag)
	{
		GetFTCLog() << "----------" << std::endl;
		GetFTCLog() << "BuildLayout:" << std::endl;
		GetFTCLog() << "(expand " << (expandHorizontally ? "horiz" : "vert") << ")" << std::endl;
	}

	JPtrArray<JXContainer> objList(*itsEnclosedObjs, JPtrArrayT::kForgetAll),
						   fullObjList(JPtrArrayT::kForgetAll),
						   cellList(JPtrArrayT::kForgetAll);

	JPtrArrayIterator<JXContainer> iter(&objList);
	JXContainer* obj;
	while (iter.Next(&obj))
	{
		if (!obj->IncludeInFTC())
		{
			if (theDebugFTCFlag)
			{
				GetFTCLog() << "IGNORED " << obj->ToString() << std::endl << std::endl;
			}
			iter.RemovePrev();
		}
	}

	bool horizontal = !expandHorizontally, exact = true, first = true;
	JSize count = 0, noChangeCount = 0;
	do
	{
		if (!exact)
		{
			objList.SetCompareFunction(horizontal ? FTCCompareHorizontally : FTCCompareVertically);
			objList.Sort();
		}

		const JSize objCount = objList.GetItemCount();
		fullObjList.CopyPointers(objList, JPtrArrayT::kForgetAll, false);

		if (theDebugFTCFlag)
		{
			GetFTCLog() << "+++ " << (horizontal ? "Horizontal" : "Vertical")
				<< " (iter: " << count
				<< "; cells: " << objCount
				<< "; exact: " << JBoolToString(exact) << ")" << std::endl;
		}

		iter.MoveTo(JListT::kStartAtBeginning, 0);
		while (iter.Next(&obj))
		{
			if (theDebugFTCFlag)
			{
				if (!theDebugFTCNoopExaminations)
				{
					theDebugFTCLogBuffer = jnew std::ostringstream;
					assert( theDebugFTCLogBuffer != nullptr );
				}

				GetFTCLog() << "examine: " << obj->GetFrameForFTC() << ' ' << obj->ToString() << std::endl;
			}

			iter.RemovePrev();	// do not process it

			JXContainer* cell = FTCGroupAlignedObjects(obj, &objList, &fullObjList, horizontal, exact, first);
			cellList.AppendItem(cell);

			if (theDebugFTCFlag)
			{
				GetFTCLog() << "cell: " << cell->GetFrameForFTC() << ' ' << cell->ToString() << std::endl << std::endl;

				if (theDebugFTCLogBuffer != nullptr && cell != obj)
				{
					std::cout << dynamic_cast<std::ostringstream*>(theDebugFTCLogBuffer)->str();
				}

				jdelete theDebugFTCLogBuffer;
				theDebugFTCLogBuffer = nullptr;
			}
		}
		objList.CopyPointers(cellList, JPtrArrayT::kForgetAll, false);	// resets iter
		cellList.RemoveAll();

		count++;
		horizontal = ! horizontal;
		first      = false;

		const bool noChange = objCount == objList.GetItemCount();
		if (noChange)
		{
			noChangeCount++;
			if (exact && noChangeCount >= 2)
			{
				exact         = false;
				horizontal    = !expandHorizontally;	// reset to original direction
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
		while (objList.GetItemCount() > 1 || count >= 100);	// checking count is paranoia

	if (objList.GetItemCount() == 1)
	{
		*root = dynamic_cast<JXFTCCell*>(objList.GetFirstItem());
		return true;
	}
	else
	{
		GetFTCLog() << "FTCBuildLayout failed with " << objList.GetItemCount() << " roots" << std::endl;
		if (theDebugFTCFlag)
		{
			JPtrArrayIterator iter(objList);
			JXContainer* obj;
			while (iter.Next(&obj))
			{
				GetFTCLog() << "  " << obj->ToString() << " -- " << obj->GetEnclosure() << std::endl;
			}
		}
		else
		{
			objList.DeleteAll();
		}

		*root = nullptr;	// unable to enclose everything in a single table
		return false;
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
	const bool	horizontal
	)
{
	return JIntRange(
		horizontal ? r.top    : r.left,
		horizontal ? r.bottom : r.right);
}

// must be macro, because function cannot access member data
#define ftcReparentCell(cell,newParent) \
	cell->itsEnclosure->RemoveEnclosedObject(cell); \
	cell->itsEnclosure = newParent; \
	cell->itsEnclosure->AddEnclosedObject(cell)

JXFTCCell*
JXContainer::FTCGroupAlignedObjects
	(
	JXContainer*			target,
	JPtrArray<JXContainer>*	objList,
	JPtrArray<JXContainer>*	fullObjList,
	const bool			horizontal,
	const bool			exact,
	const bool			first
	)
	const
{
	const JRect targetRect         = target->GetFrameForFTC();
	const JIntRange targetInterval = ftcGetInterval(targetRect, horizontal);

	auto* container =
		jnew JXFTCCell(nullptr, target->GetEnclosure(),
					   horizontal ? JXFTCCell::kHorizontal : JXFTCCell::kVertical,
					   exact);
	assert( container != nullptr );

	JPtrArray<JXFTCCell> cellList(JPtrArrayT::kForgetAll);

	auto* cell = dynamic_cast<JXFTCCell*>(target);
	if (cell != nullptr)
	{
		ftcReparentCell(cell, container);
	}
	else
	{
		cell = jnew JXFTCCell(target, container, JXFTCCell::kNoDirection, exact);
	}
	cellList.AppendItem(cell);

	JRect covering = cell->GetFrameForFTC();

	JXContainer* obj;
	JRect rect;
	JIntRange interval, intersection;
	JPtrArray<JXContainer> matchedList(JPtrArrayT::kForgetAll);

	JPtrArrayIterator<JXContainer> objIter(objList);
	while (objIter.Next(&obj))
	{
		rect     = obj->GetFrameForFTC();
		interval = ftcGetInterval(rect, horizontal);

		if ((exact && targetInterval != interval) ||
			(!exact &&
			 (!JIntersection(targetInterval, interval, &intersection) ||
			  intersection.GetCount() == 1 ||
			  FTCWillOverlapNonincludedWidget(target, obj, *fullObjList, matchedList))))
		{
			continue;
		}

		auto* cellObj = dynamic_cast<JXFTCCell*>(obj);
		if (cellObj != nullptr)
		{
			cell = cellObj;
		}
		else	// only create new cell to wrap real widget
		{
			cell = jnew JXFTCCell(obj, container, JXFTCCell::kNoDirection, exact);
		}
		cellList.AppendItem(cell);
		matchedList.AppendItem(obj);

		if (theDebugFTCFlag)
		{
			GetFTCLog() << "match: " << cell->GetFrameForFTC() << ' ' << obj->ToString() << std::endl;
		}
	}

	// exact matches must only count if there are no intervening, unmatched objects

	if (exact)
	{
		FTCTrimBlockedMatches(target, *fullObjList, matchedList, horizontal, first, &cellList);
	}

	// short-circuit if no additional cells found

	if (cellList.GetItemCount() == 1)
	{
		auto* targetCell = dynamic_cast<JXFTCCell*>(target);
		if (targetCell == nullptr)
		{
			targetCell = cellList.GetFirstItem();
		}
		ftcReparentCell(targetCell, container->GetEnclosure());		// before deleting container
		jdelete container;
		return targetCell;
	}

	// unwrap contained cells if same direction

	JPtrArrayIterator<JXFTCCell> cellIter(&cellList);
	if (!exact)
	{
		while (cellIter.Next(&cell))
		{
			if (!cell->IsExact() && cell->GetDirection() == container->GetDirection())
			{
				if (theDebugFTCFlag)
				{
					GetFTCLog() << "unwrapping: " << cell->ToString() << std::endl;
				}

				JPtrArrayIterator<JXContainer> i2(cell->itsEnclosedObjs);
				while (i2.Next(&obj))
				{
					auto* c2 = dynamic_cast<JXFTCCell*>(obj);
					assert( c2 != nullptr );
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

	cellIter.MoveTo(JListT::kStartAtBeginning, 0);
	while (cellIter.Next(&cell))
	{
		covering = JCovering(covering, cell->GetFrameForFTC());

		ftcReparentCell(cell, container);

		// try to remove both, in case cell containing widget was matched
		objList->Remove(cell);
		objList->Remove(cell->GetWidget());
	}

	// return cell containing remaining cells

	const JPoint topLeft = container->GetEnclosure()->GlobalToLocal(covering.left, covering.top);
	container->Place(topLeft.x, topLeft.y);
	container->SetSize(covering.width(), covering.height());

	return container;
}

/******************************************************************************
 FTCWillOverlapNonincludedWidget (private)

	Computes the covering for obj1 & obj2 and checks if any unmatched obj
	intersects this covering.

 ******************************************************************************/

bool
JXContainer::FTCWillOverlapNonincludedWidget
	(
	const JXContainer*				obj1,
	const JXContainer*				obj2,
	const JPtrArray<JXContainer>&	fullObjList,
	const JPtrArray<JXContainer>&	matchedList
	)
	const
{
	JRect covering = JCovering(
		obj1->GetFrameForFTC(),
		obj2->GetFrameForFTC());

	JPtrArrayIterator matchedIter(matchedList);
	JXContainer* obj;
	while (matchedIter.Next(&obj))
	{
		covering = JCovering(covering, obj->GetFrameForFTC());
	}

	if (theDebugFTCWillOverlapNonincludedWidget && theDebugFTCFlag)
	{
		GetFTCLog() << "-----" << std::endl;
		GetFTCLog() << "covering: " << covering << std::endl;
	}

	JPtrArrayIterator iter(fullObjList);
	JRect r;
	while (iter.Next(&obj))
	{
		const bool check = obj != obj1 && obj != obj2 && !matchedList.Includes(obj);

		if (theDebugFTCWillOverlapNonincludedWidget && theDebugFTCFlag && check)
		{
			GetFTCLog() << "check: " << obj->GetFrameForFTC() << ' ' << obj->ToString() << std::endl;
		}

		if (check && JIntersection(covering, obj->GetFrameForFTC(), &r))
		{
			if (theDebugFTCWillOverlapNonincludedWidget && theDebugFTCFlag)
			{
				GetFTCLog() << "----- T" << std::endl;
			}

			return true;
		}
	}

	if (theDebugFTCWillOverlapNonincludedWidget && theDebugFTCFlag)
	{
		GetFTCLog() << "----- F" << std::endl;
	}
	return false;
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
	const bool						horizontal,
	const bool						deleteBlockedWidgetCells,
	JPtrArray<JXFTCCell>*			cellList
	)
	const
{
	const bool vertical = ! horizontal;

	const JRect targetRect         = target->GetFrameForFTC();
	const JIntRange targetInterval = ftcGetInterval(targetRect, horizontal);

	JXContainer *obj, *minBlocker = nullptr, *maxBlocker = nullptr;

	// find blockers

	JPtrArrayIterator allObjIter(fullObjList);
	JIntRange intersection;
	while (allObjIter.Next(&obj))
	{
		if (obj == target || matchedList.Includes(obj))
		{
			continue;
		}

		const JRect rect         = obj->GetFrameForFTC();
		const JIntRange interval = ftcGetInterval(rect, horizontal);

		const bool overlaps = JIntersection(targetInterval, interval, &intersection) &&
			intersection.GetCount() > 1;

		if (overlaps &&
			((horizontal && rect.right  <= targetRect.left) ||
			 (vertical   && rect.bottom <= targetRect.top )) &&
			(minBlocker == nullptr ||
			 (horizontal && minBlocker->GetFrameForFTC().right  < rect.right ) ||
			 (vertical   && minBlocker->GetFrameForFTC().bottom < rect.bottom)))
		{
			minBlocker = obj;

			if (theDebugFTCFlag)
			{
				GetFTCLog() << "min block: " << rect << ' ' << obj->ToString() << std::endl;
			}
		}
		else if (overlaps &&
				 ((horizontal && targetRect.right  <= rect.left) ||
				  (vertical   && targetRect.bottom <= rect.top )) &&
				 (maxBlocker == nullptr ||
				  (horizontal && rect.left < maxBlocker->GetFrameForFTC().left) ||
				  (vertical   && rect.top  < maxBlocker->GetFrameForFTC().top )))
		{
			maxBlocker = obj;

			if (theDebugFTCFlag)
			{
				GetFTCLog() << "max block: " << rect << ' ' << obj->ToString() << std::endl;
			}
		}
	}

	// ignore cells before minBlocker or after maxBlocker

	const JCoordinate min = (minBlocker == nullptr ? 0 :
							 horizontal ? minBlocker->GetFrameForFTC().right :
										  minBlocker->GetFrameForFTC().bottom),
					  max = (maxBlocker == nullptr ? 1e6 :
							 horizontal ? maxBlocker->GetFrameForFTC().left :
										  maxBlocker->GetFrameForFTC().top);

	JPtrArrayIterator<JXFTCCell> cellIter(cellList);
	JXFTCCell* cell;
	while (cellIter.Next(&cell))
	{
		const JRect rect = cell->GetFrameForFTC();
		if ((horizontal && (rect.right  < min || max < rect.left)) ||
			(vertical   && (rect.bottom < min || max < rect.top )))
		{
			if (theDebugFTCFlag)
			{
				GetFTCLog() << "blocked: " << cell->GetFrameForFTC() << ' ' << cell->ToString() << std::endl;
			}

			cellIter.RemovePrev();
			if (cell->GetWidget() != nullptr && deleteBlockedWidgetCells)
			{
				jdelete cell;	// delete cell for widget that needs to be re-processed
			}
		}
	}
}

/******************************************************************************
 IncludeInFTC (virtual protected)

	Return false to be ignored.

 ******************************************************************************/

bool
JXContainer::IncludeInFTC()
	const
{
	JRect r;
	return JIntersection(itsWindow->GetFrameGlobal(), GetFrameGlobal(), &r);
}

/******************************************************************************
 NeedsInternalFTC (virtual protected)

	Return true if the contents are a set of widgets that need to expand.

 ******************************************************************************/

bool
JXContainer::NeedsInternalFTC()
	const
{
	return false;
}

/******************************************************************************
 RunInternalFTC (virtual)

	Expand and return new size.

 ******************************************************************************/

bool
JXContainer::RunInternalFTC
	(
	const bool		horizontal,
	JCoordinate*	newSize
	)
{
	JXFTCCell* root;
	if (FTCBuildLayout(horizontal, &root))
	{
		*newSize = root->Expand(horizontal);
		jdelete root;
		return true;
	}
	else
	{
		*newSize = 0;
		return false;
	}
}

/******************************************************************************
 ComputePaddingForInternalFTC

 ******************************************************************************/

JRect
JXContainer::ComputePaddingForInternalFTC()
	const
{
	if (itsEnclosedObjs == nullptr || itsEnclosedObjs->IsEmpty())
	{
		return JRect();
	}

	JXContainer* obj = itsEnclosedObjs->GetFirstItem();
	JRect covering   = obj->GetFrameForFTC();

	const JSize count = itsEnclosedObjs->GetItemCount();
	for (JIndex i=2; i<=count; i++)
	{
		covering = JCovering(covering, itsEnclosedObjs->GetItem(i)->GetFrameForFTC());
	}

	const JRect r = GetApertureGlobal();

	return JRect(
		covering.top - r.top,
		covering.left - r.left,
		r.bottom - covering.bottom,
		r.right - covering.right);
}

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

 ******************************************************************************/

JCoordinate
JXContainer::GetFTCMinContentSize
	(
	const bool horizontal
	)
	const
{
	const JRect apG = GetApertureGlobal();
	return (horizontal ? apG.width() : apG.height());
}

/******************************************************************************
 GetFrameForFTC (virtual protected)

	By default, returns the frame.

 ******************************************************************************/

JRect
JXContainer::GetFrameForFTC()
	const
{
	return GetFrameGlobal();
}

/******************************************************************************
 FTCAdjustSize (virtual)

 ******************************************************************************/

void
JXContainer::FTCAdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
}

/******************************************************************************
 FTC comparison functions (static private)

 ******************************************************************************/

std::weak_ordering
JXContainer::FTCCompareHorizontally
	(
	JXContainer* const & w1,
	JXContainer* const & w2
	)
{
	return w1->GetFrameForFTC().left <=> w2->GetFrameForFTC().left;
}

std::weak_ordering
JXContainer::FTCCompareVertically
	(
	JXContainer* const & w1,
	JXContainer* const & w2
	)
{
	return w1->GetFrameForFTC().top <=> w2->GetFrameForFTC().top;
}
