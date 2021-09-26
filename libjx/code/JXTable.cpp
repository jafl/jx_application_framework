/******************************************************************************
 JXTable.cpp

	We implement the routines required by JTable that only depend on JX.

	BASE CLASS = JXScrollableWidget, JTable

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXTable.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXWindowPainter.h"
#include "jx-af/jx/JXScrollbar.h"
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXTable::JXTable
	(
	const JCoordinate	defRowHeight,
	const JCoordinate	defColWidth,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JTable(defRowHeight, defColWidth,
		   JColorManager::GetGrayColor(50),
		   JColorManager::GetDefaultSelectionColor())
{
	const JColorID gray50Color = JColorManager::GetGrayColor(50);
	SetRowBorderInfo(1, gray50Color);	// calls TableSetScrollSteps()
	SetColBorderInfo(1, gray50Color);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTable::~JXTable()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	TableDraw(p, rect, true);
}

/******************************************************************************
 HitSamePart (virtual protected)

	This default implementation returns true if the two points are
	in the same cell.

 ******************************************************************************/

bool
JXTable::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	JPoint cell1, cell2;
	return GetCell(pt1, &cell1) &&
							  GetCell(pt2, &cell2) &&
							  cell1 == cell2;
}

/******************************************************************************
 TableRefresh (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTable::TableRefresh()
{
	Refresh();
}

/******************************************************************************
 TableRefreshRect (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTable::TableRefreshRect
	(
	const JRect& rect
	)
{
	RefreshRect(rect);
}

/******************************************************************************
 TableSetGUIBounds (virtual protected)

 ******************************************************************************/

void
JXTable::TableSetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	SetBounds(w,h);
}

/******************************************************************************
 TableSetScrollSteps (virtual protected)

 ******************************************************************************/

void
JXTable::TableSetScrollSteps
	(
	const JCoordinate hStep,
	const JCoordinate vStep
	)
{
	SetHorizStepSize(hStep);
	SetHorizPageStepContext(hStep);
	SetVertStepSize(vStep);
	SetVertPageStepContext(vStep);
}

/******************************************************************************
 Adjusting scrolltabs (virtual protected)

 ******************************************************************************/

void
JXTable::TableHeightChanged
	(
	const JCoordinate y,
	const JCoordinate delta
	)
{
	JXScrollbar *hScrollbar, *vScrollbar;
	if (GetScrollbars(&hScrollbar, &vScrollbar))
	{
		if (delta > 0)
		{
			vScrollbar->PrepareForHigherMaxValue(y, delta);
		}
		else if (delta < 0)
		{
			vScrollbar->PrepareForLowerMaxValue(y, -delta);
		}
	}
}

void
JXTable::TableHeightScaled
	(
	const JFloat scaleFactor
	)
{
	JXScrollbar *hScrollbar, *vScrollbar;
	if (GetScrollbars(&hScrollbar, &vScrollbar))
	{
		vScrollbar->PrepareForScaledMaxValue(scaleFactor);
	}
}

void
JXTable::TableRowMoved
	(
	const JCoordinate	origY,
	const JSize			height,
	const JCoordinate	newY
	)
{
	JXScrollbar *hScrollbar, *vScrollbar;
	if (GetScrollbars(&hScrollbar, &vScrollbar))
	{
		vScrollbar->StripMoved(origY, height, newY);
	}
}

void
JXTable::TableWidthChanged
	(
	const JCoordinate x,
	const JCoordinate delta
	)
{
	JXScrollbar *hScrollbar, *vScrollbar;
	if (GetScrollbars(&hScrollbar, &vScrollbar))
	{
		if (delta > 0)
		{
			hScrollbar->PrepareForHigherMaxValue(x, delta);
		}
		else if (delta < 0)
		{
			hScrollbar->PrepareForLowerMaxValue(x, -delta);
		}
	}
}

void
JXTable::TableWidthScaled
	(
	const JFloat scaleFactor
	)
{
	JXScrollbar *hScrollbar, *vScrollbar;
	if (GetScrollbars(&hScrollbar, &vScrollbar))
	{
		hScrollbar->PrepareForScaledMaxValue(scaleFactor);
	}
}

void
JXTable::TableColMoved
	(
	const JCoordinate	origX,
	const JSize			width,
	const JCoordinate	newX
	)
{
	JXScrollbar *hScrollbar, *vScrollbar;
	if (GetScrollbars(&hScrollbar, &vScrollbar))
	{
		hScrollbar->StripMoved(origX, width, newX);
	}
}

/******************************************************************************
 TableScrollToCellRect (virtual protected)

 ******************************************************************************/

bool
JXTable::TableScrollToCellRect
	(
	const JRect&	cellRect,
	const bool	centerInDisplay
	)
{
	if (centerInDisplay)
	{
		return ScrollToRectCentered(cellRect, false);
	}
	else
	{
		return ScrollToRect(cellRect);
	}
}

/******************************************************************************
 TableGetApertureWidth (virtual)

 ******************************************************************************/

JCoordinate
JXTable::TableGetApertureWidth()
	const
{
	return GetApertureWidth();
}

/******************************************************************************
 BeginSelectionDrag (protected)

	This is a convenience to translate between X mouse buttons & modifier keys
	and JTable flags.

 ******************************************************************************/

void
JXTable::BeginSelectionDrag
	(
	const JPoint&			cell,
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers
	)
{
	if (button != kJXMiddleButton)
	{
		const bool extendSelection =
			(button == kJXLeftButton && modifiers.shift()) ||
				 button == kJXRightButton;

		const bool selectDiscont =
//			GetDisplay()->IsOSX() ? modifiers.meta() :	// meta-dbl-click used in too many places
			modifiers.control();

		JTable::BeginSelectionDrag(cell, extendSelection, selectDiscont);
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

	This is a convenience to perform the default action.

 ******************************************************************************/

void
JXTable::ContinueSelectionDrag
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers	// included to distinguish from JTable function
	)
{
	if (IsDraggingSelection())
	{
		ScrollForDrag(pt);

		JPoint cell;
		const bool ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
		assert( ok );

		JTable::ContinueSelectionDrag(cell);
	}
}

/******************************************************************************
 HandleSelectionKeyPress (protected)

	This is a convenience to translate between X modifier keys and JTable flags.

 ******************************************************************************/

bool
JXTable::HandleSelectionKeyPress
	(
	const JUtf8Character&	c,
	const JXKeyModifiers&	modifiers
	)
{
	return JTable::HandleSelectionKeyPress(c, modifiers.shift());
}

/******************************************************************************
 Receive (virtual protected)

	Because of multiple inheritance, we have to route the messages manually.

 ******************************************************************************/

void
JXTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXScrollableWidget::Receive(sender, message);
	JTable::Receive(sender, message);
}
