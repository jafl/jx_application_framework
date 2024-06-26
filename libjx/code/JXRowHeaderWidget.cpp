/******************************************************************************
 JXRowHeaderWidget.cpp

	Maintains a column to match the rows of a JTable.

	The "minimum margin width" is the minimum allowed space on either side of
	the number in each cell.  When the number becomes too wide, we
	broadcast NeedsToBeWidened.  We cannot automatically resize, because
	the other widgets that share our enclosure also need to be adjusted.

	BASE CLASS = JXEditTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXRowHeaderWidget.h"
#include "JXWindow.h"
#include "JXScrollbarSet.h"
#include "JXScrollbar.h"
#include "JXDragPainter.h"
#include "JXFontManager.h"
#include "JXColorManager.h"
#include "jXPainterUtil.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

const JSize kCellFrameWidth            = kJXDefaultBorderWidth;
const JCoordinate kDragRegionHalfWidth = 2;

// JBroadcaster messages

const JUtf8Byte* JXRowHeaderWidget::kNeedsToBeWidened =
	"NeedsToBeWidened::JXRowHeaderWidget";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRowHeaderWidget::JXRowHeaderWidget
	(
	JXTable*			table,
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
	JXEditTable(1,w, nullptr, enclosure, hSizing,vSizing, x,y, w,h)
{
	assert( table != nullptr && scrollbarSet != nullptr );

	itsTable = table;
	itsTable->SetRowHeader(this);
	ListenTo(itsTable);

	itsVScrollbar = scrollbarSet->GetVScrollbar();
	ListenTo(itsVScrollbar);

	itsTitles = nullptr;

	itsAllowRowResizingFlag = false;
	itsMinRowHeight         = 1;
	itsHMarginWidth         = 2*kCellFrameWidth;
	itsMaxBcastWidth        = 0;

	itsDragType = kInvalidDrag;

	itsDragLineCursor    = JXGetDragHorizLineCursor(GetDisplay());
	itsDragAllLineCursor = JXGetDragAllHorizLineCursor(GetDisplay());

	SetColBorderInfo(0, JColorManager::GetBlackColor());

	// override JXEditTable

	WantInput(false);
	SetBackColor(JColorManager::GetDefaultBackColor());

	AppendCols(1, GetApertureWidth());
	AdjustToTable();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRowHeaderWidget::~JXRowHeaderWidget()
{
	jdelete itsTitles;
}

/******************************************************************************
 GetRowTitle

	Returns true if there is a title for the specified row.

 ******************************************************************************/

bool
JXRowHeaderWidget::GetRowTitle
	(
	const JIndex	index,
	JString*		title
	)
	const
{
	if (itsTitles != nullptr)
	{
		const JString* str = itsTitles->GetItem(index);
		if (str != nullptr)
		{
			*title = *str;
			return true;
		}
	}

	title->Clear();
	return false;
}

/******************************************************************************
 SetRowTitle

 ******************************************************************************/

void
JXRowHeaderWidget::SetRowTitle
	(
	const JIndex	index,
	const JString&	title
	)
{
	if (itsTitles == nullptr)
	{
		itsTitles = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

		const JSize rowCount = GetRowCount();
		for (JIndex i=1; i<=rowCount; i++)
		{
			itsTitles->Append(nullptr);
		}
	}

	itsTitles->SetItem(index, title, JPtrArrayT::kDelete);
}

/******************************************************************************
 ClearRowTitle

 ******************************************************************************/

void
JXRowHeaderWidget::ClearRowTitle
	(
	const JIndex index
	)
{
	if (itsTitles != nullptr)
	{
		itsTitles->SetToNull(index, JPtrArrayT::kDelete);
	}
}

/******************************************************************************
 TablePrepareToDrawCol (virtual protected)

 ******************************************************************************/

void
JXRowHeaderWidget::TablePrepareToDrawCol
	(
	const JIndex colIndex,
	const JIndex firstRow,
	const JIndex lastRow
	)
{
	itsMaxBcastWidth = GetColWidth(colIndex);
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
JXRowHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXDrawUpFrame(p, rect, kCellFrameWidth);

	JString str;
	bool hasTitle = false;
	if (itsTitles != nullptr)
	{
		const JString* title = itsTitles->GetItem(cell.y);
		if (title != nullptr)
		{
			str      = *title;
			hasTitle = true;
		}
	}
	if (!hasTitle)
	{
		str = JString(cell.y);
	}

	const JFont font = JFontManager::GetFont(
			JFontManager::GetDefaultFontName(),
			JFontManager::GetDefaultRowColHeaderFontSize(),
			JFontStyle(true, false, 0, false));
	p.SetFont(font);
	p.String(rect, str, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);

	const JCoordinate wmin = p.GetStringWidth(str) + 2*itsHMarginWidth;
	if (rect.width() < wmin && itsMaxBcastWidth < wmin)
	{
		Broadcast(NeedsToBeWidened(wmin - itsMaxBcastWidth));
		itsMaxBcastWidth = wmin;
	}
}

/******************************************************************************
 TurnOnRowResizing

	Not inline because it uses assert.

 ******************************************************************************/

void
JXRowHeaderWidget::TurnOnRowResizing
	(
	const JCoordinate minRowHeight
	)
{
	assert( minRowHeight > 0 );

	itsAllowRowResizingFlag = true;
	itsMinRowHeight         = minRowHeight;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXRowHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDragType = kInvalidDrag;
	if (!itsAllowRowResizingFlag)
	{
		return;
	}

	const bool inDragRegion = InDragRegion(pt, &itsDragCell);

	if (inDragRegion && button == kJXLeftButton)
	{
		itsDragCellRect = GetCellRect(itsDragCell);
		if (modifiers.meta())
		{
			itsDragType = kDragAllRows;
		}
		else
		{
			itsDragType = kDragOneRow;
		}

		JPainter* p = CreateDragOutsidePainter();
		JRect defClipRect = p->GetDefaultClipRect();
		JRect apG = GetApertureGlobal();
		defClipRect.top    = apG.top;
		defClipRect.bottom = apG.bottom;
		p->SetDefaultClipRect(defClipRect);

		const JRect enclAp = JXContainer::GlobalToLocal(GetEnclosure()->GetApertureGlobal());
		p->Line(enclAp.left, pt.y, enclAp.right, pt.y);
		itsPrevPt = pt;
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXRowHeaderWidget::HandleMouseDrag
	(
	const JPoint&			origPt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType != kInvalidDrag)
	{
		JPoint pt = origPt;

		// keep col width larger than minimum

		if (pt.y < itsDragCellRect.top + itsMinRowHeight)
		{
			pt.y = itsDragCellRect.top + itsMinRowHeight;
		}

		// check if we have moved

		if (pt.y != itsPrevPt.y)
		{
			JPainter* p = nullptr;
			const bool ok = GetDragPainter(&p);
			assert( ok );

			const JRect enclApG = GetEnclosure()->GetApertureGlobal();
			JRect enclAp = JXContainer::GlobalToLocal(enclApG);

			// scroll, if necessary

			const JPoint ptG    = JXContainer::LocalToGlobal(pt);
			const JPoint ptT    = JPinInRect(itsTable->GlobalToLocal(ptG),
											 itsTable->GetBounds());
			const JRect tableAp = itsTable->GetAperture();
			const JCoordinate x = tableAp.xcenter();
			const JRect tableRect(ptT.y-1, x-1, ptT.y+1, x+1);
			if (itsTable->ScrollToRect(tableRect))
			{
				GetWindow()->Update();
				enclAp = JXContainer::GlobalToLocal(enclApG);	// local coords changed
			}
			else
			{
				// erase the old line

				p->Line(enclAp.left, itsPrevPt.y, enclAp.right, itsPrevPt.y);
			}

			// draw the new line

			p->Line(enclAp.left, pt.y, enclAp.right, pt.y);

			// ready for next call

			itsPrevPt = pt;
		}
	}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

	We set the row height to itsPrevPt because that is what the user saw.

 ******************************************************************************/

void
JXRowHeaderWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType != kInvalidDrag)
	{
		// erase the line

		JPainter* p = nullptr;
		const bool ok = GetDragPainter(&p);
		assert( ok );

		const JRect enclAp = JXContainer::GlobalToLocal(GetEnclosure()->GetApertureGlobal());
		p->Line(enclAp.left, itsPrevPt.y, enclAp.right, itsPrevPt.y);

		DeleteDragPainter();

		// set the row height(s)

		const JCoordinate rowHeight = itsPrevPt.y - itsDragCellRect.top;
		if (itsDragType == kDragOneRow)
		{
			itsTable->SetRowHeight(itsDragCell.y, rowHeight);
		}
		else
		{
			assert( itsDragType == kDragAllRows );
			itsTable->SetDefaultRowHeight(rowHeight);
			itsTable->SetAllRowHeights(rowHeight);
		}
	}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Show the drag cursor when near a row boundary.

 ******************************************************************************/

void
JXRowHeaderWidget::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	const bool inDragRegion = InDragRegion(pt, &cell);
	if (itsAllowRowResizingFlag && inDragRegion && modifiers.meta())
	{
		DisplayCursor(itsDragAllLineCursor);
	}
	else if (itsAllowRowResizingFlag && inDragRegion)
	{
		DisplayCursor(itsDragLineCursor);
	}
	else
	{
		JXEditTable::AdjustCursor(pt, modifiers);
	}
}

/******************************************************************************
 InDragRegion (protected)

 ******************************************************************************/

bool
JXRowHeaderWidget::InDragRegion
	(
	const JPoint&	pt,
	JPoint*			cell
	)
	const
{
	JCoordinate width;
	JColorID color;
	GetRowBorderInfo(&width, &color);
	const JCoordinate halfWidth = kDragRegionHalfWidth + (width+1)/2;

	JPoint virtualPt(pt.x, pt.y - halfWidth);
	if (GetCell(virtualPt, cell))
	{
		JRect cellRect = GetCellRect(*cell);
		if (cellRect.bottom - halfWidth <= pt.y &&
			pt.y <= cellRect.bottom + halfWidth)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXRowHeaderWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	SetColWidth(1, GetApertureWidth());
}

/******************************************************************************
 Receive (virtual protected)

	Listen for adjustments in vertical scrollbar.

 ******************************************************************************/

void
JXRowHeaderWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// scrollbar moved

	if (sender == const_cast<JXScrollbar*>(itsVScrollbar) &&
		message.Is(JXScrollbar::kScrolled))
	{
		ScrollTo(0, itsVScrollbar->GetValue());
	}

	// rows changed

	else if (sender == itsTable && message.Is(JTable::kRowHeightChanged))
	{
		auto& info = dynamic_cast<const JTable::RowHeightChanged&>(message);
		SetRowHeight(info.GetIndex(), info.GetNewRowHeight());
	}

	else if (sender == itsTable && message.Is(JTable::kAllRowHeightsChanged))
	{
		auto& info = dynamic_cast<const JTable::AllRowHeightsChanged&>( message);
		SetAllRowHeights(info.GetNewRowHeight());
	}

	else if (sender == itsTable && message.Is(JTable::kRowsInserted))
	{
		auto& info = dynamic_cast<const JTable::RowsInserted&>(message);
		if (itsTitles != nullptr)
		{
			for (JIndex i=1; i<=info.GetCount(); i++)
			{
				itsTitles->InsertAtIndex(info.GetFirstIndex(), (JString*) nullptr);
			}
		}
		InsertRows(info.GetFirstIndex(), info.GetCount(), info.GetRowHeight());
	}

	else if (sender == itsTable && message.Is(JTable::kRowsRemoved))
	{
		auto& info = dynamic_cast<const JTable::RowsRemoved&>(message);
		if (itsTitles != nullptr)
		{
			for (JIndex i=1; i<=info.GetCount(); i++)
			{
				itsTitles->DeleteItem(info.GetFirstIndex());
			}
		}
		RemoveNextRows(info.GetFirstIndex(), info.GetCount());
	}

	else if (sender == itsTable && message.Is(JTable::kRowMoved))
	{
		auto& info = dynamic_cast<const JTable::RowMoved&>(message);
		if (itsTitles != nullptr)
		{
			itsTitles->MoveItemToIndex(info.GetOrigIndex(), info.GetNewIndex());
		}
		MoveRow(info.GetOrigIndex(), info.GetNewIndex());
	}

	else if (sender == itsTable && message.Is(JTable::kRowBorderWidthChanged))
	{
		auto& info = dynamic_cast<const JTable::RowBorderWidthChanged&>(message);
		SetRowBorderInfo(info.GetNewBorderWidth(), JColorManager::GetDefaultBackColor());
	}

	// something else

	else
	{
		JXEditTable::Receive(sender, message);
	}
}

/******************************************************************************
 AdjustToTable (private)

 ******************************************************************************/

void
JXRowHeaderWidget::AdjustToTable()
{
	assert( itsTable != nullptr && itsTitles == nullptr );

	JCoordinate width;
	JColorID color;
	itsTable->GetRowBorderInfo(&width, &color);
	SetRowBorderInfo(width, JColorManager::GetDefaultBackColor());

	RemoveAllRows();

	const JSize rowCount = itsTable->GetRowCount();
	for (JIndex i=1; i<=rowCount; i++)
	{
		InsertRows(i, 1, itsTable->GetRowHeight(i));
	}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JXRowHeaderWidget::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert_msg( 0, "The programmer forgot to override JXEditTable::CreateXInputField()" );
	return nullptr;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
JXRowHeaderWidget::PrepareDeleteXInputField()
{
	assert_msg( 0, "The programmer forgot to override JXEditTable::PrepareDeleteXInputField()" );
}
