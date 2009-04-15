/******************************************************************************
 JXWidget.cpp

	Frame is the bounding rect that the user sees in the window.  Aperture
	is the bounding rect of the area that can be drawn to in the window and
	is smaller than the frame.  Bounds is the actual size of the Widget,
	which can be larger or smaller than Aperture.  If it is smaller, it is
	stuck in the upper left corner of Aperture.  If it is larger, it can
	scroll, but Aperture must always be entirely inside Bounds.  The default
	is Bounds locked equal to Aperture.  Use UnlockBounds() to change this.

	Global coordinates are defined by the upper left corner of the window.
	Local coordinates are defined by the upper left corner of Bounds.
	Enclosure coordinates are the local coordinates of the enclosing Widget.

	All rectangles are stored in global coordinates.  Frame can be gotten in
	enclosure or global coordinates.  Bounds and Aperture can be gotten in
	local or global coordinates.

	Moving the Frame (Place,Move) is done in enclosure coordinates.
	Changing the Frame size (SetSize,AdjustSize) and incremental scrolling
	(Scroll) are obviously independent of the coordinate system. Absolute
	scrolling (ScrollTo, ScrollToRect) uses local coordinates.

	This class also implements the keypress dispatcher and default event
	handling (do nothing) so derived classes only have to implement handlers
	for the events that they support.

	If the contents of the object must be validated before focus can shift
	to something else, override OKToUnfocus().  If this returns kJFalse,
	you will retain the focus.

	HandleKeyPress is public because key press events have to propagate from
	leaf to root of the enclosure tree, unlike mouse events that propagate
	from root to leaf.  HandleShortcut is public because JXWindow needs to
	call it, and there is no harm in letting others call it.

	BASE CLASS = JXContainer

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWidget.h>
#include <JXWindow.h>
#include <JXDisplay.h>
#include <JXDragPainter.h>
#include <JXDNDManager.h>
#include <JXColormap.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JXWidget::kGotFocus  = "GotFocus::JXWidget";
const JCharacter* JXWidget::kLostFocus = "LostFocus::JXWidget";

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXWidget::JXWidget
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXContainer(enclosure->GetWindow(), enclosure)
{
	assert( enclosure != NULL );
	assert( w > 0 && h > 0 );

	itsHSizing     = hSizing;
	itsVSizing     = vSizing;
	itsBorderWidth = 0;
	itsBackColor   = (GetColormap())->GetDefaultBackColor();
	itsFocusColor  = (GetColormap())->GetDefaultFocusColor();
	itsDragPainter = NULL;

	itsWantInputFlag  = kJFalse;
	itsWantTabFlag    = kJFalse;
	itsWantModTabFlag = kJFalse;

	const JPoint pt = enclosure->LocalToGlobal(x,y);
	itsFrameG       = JRect(pt.y, pt.x, pt.y+h, pt.x+w);

	itsApertureBoundedFlag = kJTrue;
	itsBoundsG             = GetApertureGlobal();

	Refresh();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWidget::~JXWidget()
{
	JXWindow* window = GetWindow();
	if (itsWantInputFlag)
		{
		window->UnregisterFocusWidget(this);
		}
	if (IsVisible())
		{
		window->RefreshRect(itsFrameG);
		}

	delete itsDragPainter;
}

/******************************************************************************
 Refresh (virtual)

 ******************************************************************************/

void
JXWidget::Refresh()
	const
{
	JRect visRectG;
	if (IsVisible() && (GetEnclosure())->GetVisibleRectGlobal(itsFrameG, &visRectG))
		{
		(GetWindow())->RefreshRect(visRectG);
		}
}

/******************************************************************************
 RefreshRect

 ******************************************************************************/

void
JXWidget::RefreshRect
	(
	const JRect& rect
	)
	const
{
	const JRect rectG = JXContainer::LocalToGlobal(rect);
	JRect visRectG;
	if (IsVisible() && GetVisibleRectGlobal(rectG, &visRectG))
		{
		(GetWindow())->RefreshRect(visRectG);
		}
}

/******************************************************************************
 Redraw (virtual)

 ******************************************************************************/

void
JXWidget::Redraw()
	const
{
	JRect visRectG;
	if (IsVisible() && (GetEnclosure())->GetVisibleRectGlobal(itsFrameG, &visRectG))
		{
		(GetWindow())->RedrawRect(visRectG);
		}
}

/******************************************************************************
 RedrawRect

 ******************************************************************************/

void
JXWidget::RedrawRect
	(
	const JRect& rect
	)
	const
{
	const JRect rectG = JXContainer::LocalToGlobal(rect);
	JRect visRectG;
	if (IsVisible() && GetVisibleRectGlobal(rectG, &visRectG))
		{
		(GetWindow())->RedrawRect(visRectG);
		}
}

/******************************************************************************
 DrawBackground (virtual protected)

 ******************************************************************************/

void
JXWidget::DrawBackground
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.SetPenColor(GetCurrBackColor());
	p.SetFilling(kJTrue);
	p.JPainter::Rect(frame);
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXWidget::Deactivate()
{
	JXContainer::Deactivate();
	if (!IsActive() && HasFocus())
		{
		(GetWindow())->SwitchFocusToFirstWidget();
		}
}

/******************************************************************************
 AcceptDrag (virtual protected)

 ******************************************************************************/

JBoolean
JXWidget::AcceptDrag
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers
	)
{
	return JConvertToBoolean(
			JXContainer::AcceptDrag(pt, button, modifiers) &&
			(!itsWantInputFlag || (GetWindow())->SwitchFocusToWidget(this)));
}

/******************************************************************************
 WantInput (protected)

	wantInput:        everything else
	wantTab:          tab and Shift-tab
	wantModifiedTab:  Meta-tab, Ctrl-tab, etc.

	If there is more than one input area in a window, do not grab both tab
	and modified tab, because otherwise the user won't be able to switch
	focus from one input area to another.

 ******************************************************************************/

void
JXWidget::WantInput
	(
	const JBoolean wantInput,
	const JBoolean wantTab,
	const JBoolean wantModifiedTab
	)
{
	itsWantInputFlag  = wantInput;
	itsWantTabFlag    = wantTab;
	itsWantModTabFlag = wantModifiedTab;

	if (itsWantInputFlag)
		{
		(GetWindow())->RegisterFocusWidget(this);
		}
	else
		{
		(GetWindow())->UnregisterFocusWidget(this);
		}
}

/******************************************************************************
 Focus

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXWidget::Focus()
{
	return (GetWindow())->SwitchFocusToWidget(this);
}

// private -- for use by JXWindow

void
JXWidget::Focus
	(
	const int x
	)
{
	HandleFocusEvent();
	if (itsFocusColor != itsBackColor)
		{
		Refresh();
		}
	Broadcast(GotFocus());
}

/******************************************************************************
 Unfocus

	Returns kJTrue if we had focus and were able to lose it.

 ******************************************************************************/

JBoolean
JXWidget::Unfocus()
{
	if (HasFocus())
		{
		return (GetWindow())->UnfocusCurrentWidget();
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HasFocus

 ******************************************************************************/

JBoolean
JXWidget::HasFocus()
	const
{
	JXWidget* focusWidget;
	return JConvertToBoolean(
			(GetWindow())->GetFocusWidget(&focusWidget) &&
			focusWidget == this);
}

/******************************************************************************
 NotifyFocusLost (private)

	Should only be called by JXWindow.

 ******************************************************************************/

void
JXWidget::NotifyFocusLost()
{
	HandleUnfocusEvent();
	if (itsFocusColor != itsBackColor)
		{
		Refresh();
		}
	Broadcast(LostFocus());
}

/******************************************************************************
 OKToUnfocus (virtual)

	Returns whether or not the object is willing to give up focus.

 ******************************************************************************/

JBoolean
JXWidget::OKToUnfocus()
{
	return kJTrue;
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXWidget::HandleFocusEvent()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXWidget::HandleUnfocusEvent()
{
}

/******************************************************************************
 HandleWindowFocusEvent (virtual protected)

 ******************************************************************************/

void
JXWidget::HandleWindowFocusEvent()
{
}

/******************************************************************************
 HandleWindowUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXWidget::HandleWindowUnfocusEvent()
{
}

/******************************************************************************
 HandleShortcut (virtual)

 ******************************************************************************/

void
JXWidget::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 GlobalToLocal (virtual)

 ******************************************************************************/

JPoint
JXWidget::GlobalToLocal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x - itsBoundsG.left, y - itsBoundsG.top);
}

/******************************************************************************
 LocalToGlobal (virtual)

 ******************************************************************************/

JPoint
JXWidget::LocalToGlobal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x + itsBoundsG.left, y + itsBoundsG.top);
}

/******************************************************************************
 Place (virtual)

 ******************************************************************************/

void
JXWidget::Place
	(
	const JCoordinate enclX,
	const JCoordinate enclY
	)
{
	const JPoint oldPt = (GetEnclosure())->GlobalToLocal(itsFrameG.topLeft());
	Move(enclX - oldPt.x, enclY - oldPt.y);
}

/******************************************************************************
 Move (virtual)

 ******************************************************************************/

void
JXWidget::Move
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	if (dx != 0 || dy != 0)
		{
		Refresh();		// refresh orig location

		itsBoundsG.Shift(dx,dy);
		itsFrameG.Shift(dx,dy);
		NotifyBoundsMoved(dx,dy);

		Refresh();		// refresh new location
		}
}

/******************************************************************************
 SetSize (virtual)

 ******************************************************************************/

void
JXWidget::SetSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	AdjustSize(w - itsFrameG.width(), h - itsFrameG.height());
}

/******************************************************************************
 AdjustSize (virtual)

 ******************************************************************************/

void
JXWidget::AdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (dw != 0 || dh != 0)
		{
		assert( itsFrameG.width() + dw > 0 && itsFrameG.height() + dh > 0 );

		Refresh();		// refresh orig size

		itsFrameG.bottom += dh;
		itsFrameG.right  += dw;
		ApertureResized(dw,dh);

		Refresh();		// refresh new size
		}
}

/******************************************************************************
 CenterWithinEnclosure

 ******************************************************************************/

void
JXWidget::CenterWithinEnclosure
	(
	const JBoolean adjustHoriz,
	const JBoolean adjustVert
	)
{
	const JRect frame      = GetFrame();
	const JPoint oldPt     = frame.topLeft();
	const JRect enclBounds = (GetEnclosure())->GetBounds();

	JCoordinate dx=0, dy=0;
	if (adjustHoriz)
		{
		dx = (enclBounds.xcenter() - frame.width()/2) - oldPt.x;
		}
	if (adjustVert)
		{
		dy = (enclBounds.ycenter() - frame.height()/2) - oldPt.y;
		}

	Move(dx,dy);
}

/******************************************************************************
 FitToEnclosure

 ******************************************************************************/

void
JXWidget::FitToEnclosure
	(
	const JBoolean fitHoriz,
	const JBoolean fitVert
	)
{
	const JRect frame      = GetFrame();
	const JPoint oldPt     = frame.topLeft();
	const JRect enclBounds = (GetEnclosure())->GetBounds();

	JCoordinate dx=0, dy=0, dw=0, dh=0;
	if (fitHoriz)
		{
		dx = enclBounds.left - oldPt.x;
		dw = enclBounds.width() - frame.width();
		}
	if (fitVert)
		{
		dy = enclBounds.top - oldPt.y;
		dh = enclBounds.height() - frame.height();
		}

	Move(dx,dy);
	AdjustSize(dw,dh);
}

/******************************************************************************
 BoundsMoved (virtual protected)

	Update our DragPainter.

 ******************************************************************************/

void
JXWidget::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	if (itsDragPainter != NULL)
		{
		itsDragPainter->ShiftOrigin(dx,dy);
		}
}

/******************************************************************************
 EnclosingBoundsMoved (virtual protected)

	Move ourselves so we end up in the same place relative to our
	enclosure.

 ******************************************************************************/

void
JXWidget::EnclosingBoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	Move(dx,dy);
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXWidget::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (!itsApertureBoundedFlag)
		{
		JCoordinate dx,dy;
		if (KeepApertureInsideBounds(&dx, &dy))
			{
			itsBoundsG.Shift(dx,dy);
			NotifyBoundsMoved(dx,dy);
			}
		}
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

 ******************************************************************************/

void
JXWidget::EnclosingBoundsResized
	(
	const JCoordinate dwb,
	const JCoordinate dhb
	)
{
	JCoordinate dx = 0;
	JCoordinate dw = 0;
	if (itsHSizing == kFixedRight)
		{
		dx = dwb;
		}
	else if (itsHSizing == kHElastic)
		{
		dw = dwb;
		}

	JCoordinate dy = 0;
	JCoordinate dh = 0;
	if (itsVSizing == kFixedBottom)
		{
		dy = dhb;
		}
	else if (itsVSizing == kVElastic)
		{
		dh = dhb;
		}

	Move(dx,dy);
	AdjustSize(dw,dh);
}

/******************************************************************************
 ApertureMoved (virtual protected)

 ******************************************************************************/

void
JXWidget::ApertureMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	if (itsApertureBoundedFlag)
		{
		itsBoundsG.Shift(dx,dy);
		NotifyBoundsMoved(dx,dy);
		}
	else
		{
		JCoordinate dx1,dy1;
		if (KeepApertureInsideBounds(&dx1, &dy1))
			{
			itsBoundsG.Shift(dx1,dy1);
			NotifyBoundsMoved(dx1,dy1);
			}
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (dw != 0 || dh != 0)
		{
		JCoordinate dx,dy;
		if (itsApertureBoundedFlag)
			{
			itsBoundsG.right  += dw;
			itsBoundsG.bottom += dh;
			NotifyBoundsResized(dw,dh);
			}
		else if (KeepApertureInsideBounds(&dx, &dy))
			{
			itsBoundsG.Shift(dx,dy);
			NotifyBoundsMoved(dx,dy);
			}
		}
}

/******************************************************************************
 KeepApertureInsideBounds (private)

	Calculates how much Bounds needs to shift to keep Aperture inside.
	If Bounds is smaller than Aperture, calculates how much Bounds needs to
	shift to put Bounds at topLeft of Aperture.

 ******************************************************************************/

JBoolean
JXWidget::KeepApertureInsideBounds
	(
	JCoordinate* dx,
	JCoordinate* dy
	)
	const
{
	*dx=0;
	*dy=0;

	const JRect ap = GetApertureGlobal();

	if (itsBoundsG.left > ap.left)
		{
		*dx = ap.left - itsBoundsG.left;
		}
	else if (itsBoundsG.right < ap.right)
		{
		*dx = ap.right - itsBoundsG.right;
		if (itsBoundsG.left + *dx > ap.left)
			{
			*dx = ap.left - itsBoundsG.left;
			}
		}

	if (itsBoundsG.top > ap.top)
		{
		*dy = ap.top - itsBoundsG.top;
		}
	else if (itsBoundsG.bottom < ap.bottom)
		{
		*dy = ap.bottom - itsBoundsG.bottom;
		if (itsBoundsG.top + *dy > ap.top)
			{
			*dy = ap.top - itsBoundsG.top;
			}
		}

	return JConvertToBoolean(*dx != 0 || *dy != 0);
}

/******************************************************************************
 SetBounds (protected)

 ******************************************************************************/

void
JXWidget::SetBounds
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	assert( !itsApertureBoundedFlag );
	assert( w >= 0 && h >= 0 );

	const JCoordinate dw = w - itsBoundsG.width();
	const JCoordinate dh = h - itsBoundsG.height();
	if (dw != 0 || dh != 0)
		{
		itsBoundsG = JXContainer::LocalToGlobal(JRect(0,0,h,w));
		NotifyBoundsResized(dw,dh);
		Refresh();
		}
}

/******************************************************************************
 AdjustBounds (protected)

 ******************************************************************************/

void
JXWidget::AdjustBounds
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	SetBounds(itsBoundsG.width() + dw, itsBoundsG.height() + dh);
}

/******************************************************************************
 Scroll

	Positive values scroll up and to the left.
	Negative values scroll down and to the right.

	Unlike on the Mac, we can't optimize scrolling via XCopyArea() because
	PointToFocus means that part of the window could be obscured.

	Returns kJTrue if Bounds had to be scrolled.

 ******************************************************************************/

JBoolean
JXWidget::Scroll
	(
	const JCoordinate userdx,
	const JCoordinate userdy
	)
{
	assert( !itsApertureBoundedFlag );

	if (userdx != 0 || userdy != 0)
		{
		const JRect apG = GetApertureGlobal();

		JCoordinate dx = userdx;
		if (itsBoundsG.width() <= apG.width())
			{
			dx = 0;
			}
		else if (dx > 0 && itsBoundsG.left + dx > apG.left)
			{
			dx = apG.left - itsBoundsG.left;
			}
		else if (dx < 0 && itsBoundsG.right + dx < apG.right)
			{
			dx = apG.right - itsBoundsG.right;
			}

		JCoordinate dy = userdy;
		if (itsBoundsG.height() <= apG.height())
			{
			dy = 0;
			}
		else if (dy > 0 && itsBoundsG.top + dy > apG.top)
			{
			dy = apG.top - itsBoundsG.top;
			}
		else if (dy < 0 && itsBoundsG.bottom + dy < apG.bottom)
			{
			dy = apG.bottom - itsBoundsG.bottom;
			}

		if (dx != 0 || dy != 0)
			{
//			const JRect origAp = GetAperture();

			itsBoundsG.Shift(dx,dy);
			NotifyBoundsMoved(dx,dy);

//			const JRect newAp = GetAperture();
//			JRect dest;
//			if (JIntersection(origAp, newAp, &dest))
//				{
//				dest      = JXContainer::LocalToGlobal(dest);
//				JRect src = dest;
//				src.Shift(-dx, -dy);
//				(GetWindow())->UpdateForScroll(JXContainer::LocalToGlobal(newAp), src, dest);
//				}
//			else
//				{
				Refresh();
//				}

			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 ScrollTo

	Scroll to place the given point (in local coords) at the top left
	corner of the aperture.

	Returns kJTrue if Bounds had to be scrolled.

 ******************************************************************************/

JBoolean
JXWidget::ScrollTo
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	const JRect ap = GetAperture();
	return Scroll(ap.left - x, ap.top  - y);
}

/******************************************************************************
 ScrollToRect

	Scroll the minimum distance to make the given rectangle visible in Aperture.
	If rectangle is larger than Aperture, top left corner has priority.

	Returns kJTrue if Bounds had to be scrolled.

	To scroll a rectangle that is below the aperture to the top of the
	aperture, extend the rectangle's bottom to the Bounds' bottom and scroll
	to that.  Similarly for the other 3 cases.

 ******************************************************************************/

JBoolean
JXWidget::ScrollToRect
	(
	const JRect& r
	)
{
	JRect rG = JXContainer::LocalToGlobal(r);
	if (!JIntersection(rG, itsBoundsG, &rG))
		{
		return kJFalse;
		}

	const JRect ap = GetApertureGlobal();

	JCoordinate dx=0;
	if (rG.left < ap.left)
		{
		dx = ap.left - rG.left;
		}
	else if (rG.right > ap.right)
		{
		dx = ap.right - rG.right;
		if (rG.left + dx < ap.left)
			{
			dx = ap.left - rG.left;
			}
		}

	JCoordinate dy=0;
	if (rG.top < ap.top)
		{
		dy = ap.top - rG.top;
		}
	else if (rG.bottom > ap.bottom)
		{
		dy = ap.bottom - rG.bottom;
		if (rG.top + dy < ap.top)
			{
			dy = ap.top - rG.top;
			}
		}

	return Scroll(dx,dy);
}

/******************************************************************************
 ScrollToRectCentered

	Scroll to center the given rectangle in Aperture.  If rectangle
	is larger than Aperture, top left corner of rectangle has priority.

	If the given rectangle is smaller than the aperture, we simply expand
	it symmetrically to fill the aperture.  Otherwise, we leave it alone.
	Then we can use ScrollToRect().

	Returns kJTrue if Bounds had to be scrolled.

 ******************************************************************************/

JBoolean
JXWidget::ScrollToRectCentered
	(
	const JRect&	origRect,
	const JBoolean	forceScroll
	)
{
	const JRect ap = GetAperture();
	if (!forceScroll && ap.Contains(origRect))
		{
		return kJFalse;
		}

	JRect r = origRect;
	const JCoordinate dw = ap.width() - r.width();
	if (dw > 0)
		{
		r.Shrink(-dw/2, 0);
		}

	const JCoordinate dh = ap.height() - r.height();
	if (dh > 0)
		{
		r.Shrink(0, -dh/2);
		}

	return ScrollToRect(r);
}

/******************************************************************************
 GetBoundsGlobal (virtual)

	Returns the bounds in global coordinates.

 ******************************************************************************/

JRect
JXWidget::GetBoundsGlobal()
	const
{
	return itsBoundsG;
}

/******************************************************************************
 GetFrameGlobal (virtual)

	Returns the frame in global coordinates.

 ******************************************************************************/

JRect
JXWidget::GetFrameGlobal()
	const
{
	return itsFrameG;
}

/******************************************************************************
 GetApertureGlobal (virtual)

	Returns the aperture in global coordinates.

 ******************************************************************************/

JRect
JXWidget::GetApertureGlobal()
	const
{
	JRect apG = itsFrameG;
	apG.Shrink(itsBorderWidth, itsBorderWidth);
	return apG;
}

/******************************************************************************
 SetBorderWidth

	Reserves the specified area inside the frame for drawing a border.
	The aperture is inside this area.

 ******************************************************************************/

void
JXWidget::SetBorderWidth
	(
	const JSize width
	)
{
	if (width != itsBorderWidth)
		{
		const JRect origApG = GetApertureGlobal();
		itsBorderWidth = width;
		const JRect newApG = GetApertureGlobal();

		ApertureMoved(newApG.left - origApG.left, newApG.top - origApG.top);
		ApertureResized(newApG.width()  - origApG.width(),
						newApG.height() - origApG.height());
		}
}

/******************************************************************************
 CreateDragPainter (private)

	Create a DragPainter that works inside the given widget's aperture.

 ******************************************************************************/

JXDragPainter*
JXWidget::CreateDragPainter
	(
	const JXContainer* widget
	)
{
	assert( (GetWindow())->IsVisible() );
	assert( itsDragPainter == NULL );

	JRect clipRect;
	const JBoolean visible =
		widget->GetVisibleRectGlobal(widget->GetApertureGlobal(), &clipRect);
	assert( visible );

	itsDragPainter = new JXDragPainter(GetDisplay(), GetWindow(), clipRect);
	assert( itsDragPainter != NULL );

	itsDragPainter->SetOrigin(itsBoundsG.left, itsBoundsG.top);
	itsDragPainter->ResetClipRect();	// do this last so clipRect matches with new origin
	return itsDragPainter;
}

/******************************************************************************
 GetDragPainter (protected)

	Not inline to avoid including JXDragPainter.h in header file.

 ******************************************************************************/

JBoolean
JXWidget::GetDragPainter
	(
	JPainter** p
	)
	const
{
	*p = itsDragPainter;
	return JI2B(itsDragPainter != NULL);
}

/******************************************************************************
 DeleteDragPainter (protected)

 ******************************************************************************/

void
JXWidget::DeleteDragPainter()
{
	delete itsDragPainter;
	itsDragPainter = NULL;
}

/******************************************************************************
 BeginDND (protected)

	Call this to begin the DND process.  If this returns kJTrue, you will
	not get any more HandleMouseDrag() messages, nor will you get a
	HandleMouseUp() message.

	data should be constructed using the delayed evaluation constructor
	unless the copying is always very fast because one should never make
	the user wait when the drag begins.

	*** The caller should not delete data even if this function returns kJFalse.

	*** The caller retains ownership of targetFinder.

 ******************************************************************************/

JBoolean
JXWidget::BeginDND
	(
	const JPoint&				pt,
	const JXButtonStates&		buttonStates,
	const JXKeyModifiers&		modifiers,
	JXSelectionData*			data,
	JXDNDManager::TargetFinder*	targetFinder
	)
{
	return (GetDNDManager())->BeginDND(this, pt, buttonStates, modifiers, data,
									   targetFinder);
}

/******************************************************************************
 DNDInit (virtual protected)

	This is called when DND is initiated.

 ******************************************************************************/

void
JXWidget::DNDInit
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
JXWidget::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
}

/******************************************************************************
 DNDFinish (virtual protected)

	This is called when DND is terminated, but before the actual data
	transfer.

	If it is not a drop, or the drop target is not within the same
	application, target is NULL.

 ******************************************************************************/

void
JXWidget::DNDFinish
	(
	const JBoolean		isDrop,
	const JXContainer*	target
	)
{
}

/******************************************************************************
 DNDCompletelyFinished (virtual protected)

	This *may* be called when DND is terminated.  It is guaranteed to be
	called *after* the data has been transferred.

	If the drop is within the application, this is guaranteed to be called.
	Otherwise, don't count on it.  And don't blame me -- it's a result of
	safety concerns in the underlying protocol.  The reason that this
	message is not completely useless is that it is only likely to be
	useful for cleaning up after highly optimized, intra-app drops.

	It is not called simply DNDFinished() because that is too close to the
	more frequently used DNDFinish().

 ******************************************************************************/

void
JXWidget::DNDCompletelyFinished()
{
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly on the drag source so the drop action can be
	changed based on the current target, buttons, and modifier keys.

	If the drop target is not within the same application, target is NULL.

	The default is to return the default action: copy.

 ******************************************************************************/

Atom
JXWidget::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return (GetDNDManager())->GetDNDActionCopyXAtom();
}

/******************************************************************************
 GetDNDAskActions (virtual protected)

	This is called when the value returned by GetDNDAction() changes to
	XdndActionAsk.  If GetDNDAction() repeatedly returns XdndActionAsk,
	this function is not called again because it is assumed that the
	actions are the same within a single DND session.

	This function must place at least 2 elements in askActionList and
	askDescriptionList.

	The first element should be the default action.

	The default is to do nothing, so classes that never return XdndActionAsk
	don't have to implement it and classes that forget to implement it
	will force an assert() in JXDNDManager by not returning at least 2 elements.

 ******************************************************************************/

void
JXWidget::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	assert( 0 /* The programmer forgot to override JXWidget::GetDNDAskActions() */ );
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

	This is called on the source when the target indicates whether or not
	it will accept the drop.  If !dropAccepted, the action is undefined.
	If the drop target is not within the same application, target is NULL.

	The default implementation is to display the appropriate default
	cursor provided by JXDNDManager.

	Note that part of the cursor should always be the standard arrow.  This
	way, the user will feel like they really are dragging something.

 ******************************************************************************/

void
JXWidget::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor((GetDNDManager())->GetDefaultDNDCursor(dropAccepted, action));
}

#define JTemplateType JXWidget
#include <JPtrArray.tmpls>
#undef JTemplateType
