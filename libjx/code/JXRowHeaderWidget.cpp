/******************************************************************************
 JXRowHeaderWidget.cpp

	Maintains a column to match the rows of a JTable.

	The "minimum margin width" is the minimum allowed space on either side of
	the number in each cell.  When the number becomes too wide, we
	broadcast NeedsToBeWidened.  We cannot automatically resize, because
	the other widgets that share our enclosure also need to be adjusted.

	BASE CLASS = JXEditTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXRowHeaderWidget.h>
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXDragPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

const JSize kFontSize                  = 10;
const JSize kCellFrameWidth            = kJXDefaultBorderWidth;
const JCoordinate kDragRegionHalfWidth = 2;

// JBroadcaster messages

const JCharacter* JXRowHeaderWidget::kNeedsToBeWidened =
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
	JXEditTable(1,w, NULL, enclosure, hSizing,vSizing, x,y, w,h)
{
	assert( table != NULL && scrollbarSet != NULL );

	itsTable = table;
	itsTable->SetRowHeader(this);
	ListenTo(itsTable);

	itsVScrollbar = scrollbarSet->GetVScrollbar();
	ListenTo(itsVScrollbar);

	itsTitles = NULL;

	itsAllowRowResizingFlag = kJFalse;
	itsMinRowHeight         = 1;
	itsHMarginWidth         = 2*kCellFrameWidth;
	itsMaxBcastWidth        = 0;

	itsDragType = kInvalidDrag;

	itsDragLineCursor    = JXGetDragHorizLineCursor(GetDisplay());
	itsDragAllLineCursor = JXGetDragAllHorizLineCursor(GetDisplay());

	SetColBorderInfo(0, (GetColormap())->GetBlackColor());

	// override JXEditTable

	WantInput(kJFalse);
	SetBackColor((GetColormap())->GetDefaultBackColor());

	AppendCols(1, GetApertureWidth());
	AdjustToTable();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRowHeaderWidget::~JXRowHeaderWidget()
{
	delete itsTitles;
}

/******************************************************************************
 GetRowTitle

	Returns kJTrue if there is a title for the specified row.

 ******************************************************************************/

JBoolean
JXRowHeaderWidget::GetRowTitle
	(
	const JIndex	index,
	JString*		title
	)
	const
{
	if (itsTitles != NULL)
		{
		const JString* str = itsTitles->NthElement(index);
		if (str != NULL)
			{
			*title = *str;
			return kJTrue;
			}
		}

	title->Clear();
	return kJFalse;
}

/******************************************************************************
 SetRowTitle

 ******************************************************************************/

void
JXRowHeaderWidget::SetRowTitle
	(
	const JIndex		index,
	const JCharacter*	title
	)
{
	if (itsTitles == NULL)
		{
		itsTitles = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( itsTitles != NULL );

		const JSize rowCount = GetRowCount();
		for (JIndex i=1; i<=rowCount; i++)
			{
			itsTitles->Append(NULL);
			}
		}

	itsTitles->SetElement(index, title, JPtrArrayT::kDelete);
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
	if (itsTitles != NULL)
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
	JBoolean hasTitle = kJFalse;
	if (itsTitles != NULL)
		{
		const JString* title = itsTitles->NthElement(cell.y);
		if (title != NULL)
			{
			str      = *title;
			hasTitle = kJTrue;
			}
		}
	if (!hasTitle)
		{
		str = JString(cell.y, 0);
		}

	p.SetFont(JGetDefaultFontName(), kFontSize,
			  JFontStyle(kJTrue, kJFalse, 0, kJFalse, (p.GetColormap())->GetBlackColor()));
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);

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

	itsAllowRowResizingFlag = kJTrue;
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

	const JBoolean inDragRegion = InDragRegion(pt, &itsDragCell);

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

		const JRect enclAp = JXContainer::GlobalToLocal((GetEnclosure())->GetApertureGlobal());
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
			JPainter* p = NULL;
			const JBoolean ok = GetDragPainter(&p);
			assert( ok );

			const JRect enclApG = (GetEnclosure())->GetApertureGlobal();
			JRect enclAp = JXContainer::GlobalToLocal(enclApG);

			// scroll, if necessary

			const JPoint ptG    = JXContainer::LocalToGlobal(pt);
			const JPoint ptT    = JPinInRect(itsTable->JXContainer::GlobalToLocal(ptG),
											 itsTable->GetBounds());
			const JRect tableAp = itsTable->GetAperture();
			const JCoordinate x = tableAp.xcenter();
			const JRect tableRect(ptT.y-1, x-1, ptT.y+1, x+1);
			if (itsTable->ScrollToRect(tableRect))
				{
				(GetWindow())->Update();
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

		JPainter* p = NULL;
		const JBoolean ok = GetDragPainter(&p);
		assert( ok );

		const JRect enclAp = JXContainer::GlobalToLocal((GetEnclosure())->GetApertureGlobal());
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
	const JBoolean inDragRegion = InDragRegion(pt, &cell);
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

JBoolean
JXRowHeaderWidget::InDragRegion
	(
	const JPoint&	pt,
	JPoint*			cell
	)
	const
{
	JCoordinate width;
	JColorIndex color;
	GetRowBorderInfo(&width, &color);
	const JCoordinate halfWidth = kDragRegionHalfWidth + (width+1)/2;

	JPoint virtualPt(pt.x, pt.y - halfWidth);
	if (GetCell(virtualPt, cell))
		{
		JRect cellRect = GetCellRect(*cell);
		if (cellRect.bottom - halfWidth <= pt.y &&
			pt.y <= cellRect.bottom + halfWidth)
			{
			return kJTrue;
			}
		}

	return kJFalse;
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
		const JTable::RowHeightChanged* info =
			dynamic_cast(const JTable::RowHeightChanged*, &message);
		assert( info != NULL );
		SetRowHeight(info->GetIndex(), info->GetNewRowHeight());
		}

	else if (sender == itsTable && message.Is(JTable::kAllRowHeightsChanged))
		{
		const JTable::AllRowHeightsChanged* info =
			dynamic_cast(const JTable::AllRowHeightsChanged*,& message);
		assert( info != NULL );
		SetAllRowHeights(info->GetNewRowHeight());
		}

	else if (sender == itsTable && message.Is(JTable::kRowsInserted))
		{
		const JTable::RowsInserted* info =
			dynamic_cast(const JTable::RowsInserted*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			for (JIndex i=1; i<=info->GetCount(); i++)
				{
				itsTitles->InsertAtIndex(info->GetFirstIndex(), (JString*) NULL);
				}
			}
		InsertRows(info->GetFirstIndex(), info->GetCount(), info->GetRowHeight());
		}

	else if (sender == itsTable && message.Is(JTable::kRowsRemoved))
		{
		const JTable::RowsRemoved* info =
			dynamic_cast(const JTable::RowsRemoved*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			for (JIndex i=1; i<=info->GetCount(); i++)
				{
				itsTitles->DeleteElement(info->GetFirstIndex());
				}
			}
		RemoveNextRows(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == itsTable && message.Is(JTable::kRowMoved))
		{
		const JTable::RowMoved* info =
			dynamic_cast(const JTable::RowMoved*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			itsTitles->MoveElementToIndex(info->GetOrigIndex(), info->GetNewIndex());
			}
		MoveRow(info->GetOrigIndex(), info->GetNewIndex());
		}

	else if (sender == itsTable && message.Is(JTable::kRowBorderWidthChanged))
		{
		const JTable::RowBorderWidthChanged* info =
			dynamic_cast(const JTable::RowBorderWidthChanged*, &message);
		assert( info != NULL );
		SetRowBorderInfo(info->GetNewBorderWidth(), (GetColormap())->GetDefaultBackColor());
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
	assert( itsTable != NULL && itsTitles == NULL );

	JCoordinate width;
	JColorIndex color;
	itsTable->GetRowBorderInfo(&width, &color);
	SetRowBorderInfo(width, (GetColormap())->GetDefaultBackColor());

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
	assert( 0 /* The programmer forgot to override JXEditTable::CreateXInputField() */ );
	return NULL;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
JXRowHeaderWidget::PrepareDeleteXInputField()
{
	assert( 0 /* The programmer forgot to override JXEditTable::PrepareDeleteXInputField() */ );
}
