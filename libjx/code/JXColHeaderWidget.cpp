/******************************************************************************
 JXColHeaderWidget.cpp

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXEditTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXColHeaderWidget.h>
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXDragPainter.h>
#include <jXPainterUtil.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

const JSize kFontSize                  = 10;
const JSize kCellFrameWidth            = kJXDefaultBorderWidth;
const JCoordinate kDragRegionHalfWidth = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXColHeaderWidget::JXColHeaderWidget
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
	JXEditTable(h,1, NULL, enclosure, hSizing,vSizing, x,y, w,h)
{
	assert( table != NULL && scrollbarSet != NULL );

	itsTable = table;
	itsTable->SetColHeader(this);
	ListenTo(itsTable);

	itsHScrollbar = scrollbarSet->GetHScrollbar();
	ListenTo(itsHScrollbar);

	itsTitles = NULL;

	itsAllowColResizingFlag = kJFalse;
	itsMinColWidth          = 1;

	itsDragType = kInvalidDrag;

	itsDragLineCursor    = JXGetDragVertLineCursor(GetDisplay());
	itsDragAllLineCursor = JXGetDragAllVertLineCursor(GetDisplay());

	SetDrawOrder(kDrawByRow);
	SetRowBorderInfo(0, (GetColormap())->GetBlackColor());

	// override JXEditTable

	WantInput(kJFalse);
	SetBackColor((GetColormap())->GetDefaultBackColor());

	AppendRows(1, GetApertureHeight());
	AdjustToTable();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXColHeaderWidget::~JXColHeaderWidget()
{
	delete itsTitles;
}

/******************************************************************************
 GetColTitle

	Returns kJTrue if there is a title for the specified column.

 ******************************************************************************/

JBoolean
JXColHeaderWidget::GetColTitle
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
 SetColTitle

 ******************************************************************************/

void
JXColHeaderWidget::SetColTitle
	(
	const JIndex		index,
	const JCharacter*	title
	)
{
	if (itsTitles == NULL)
		{
		itsTitles = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( itsTitles != NULL );

		const JSize colCount = GetColCount();
		for (JIndex i=1; i<=colCount; i++)
			{
			itsTitles->Append(NULL);
			}
		}

	itsTitles->SetElement(index, title, JPtrArrayT::kDelete);
}

/******************************************************************************
 ClearColTitle

 ******************************************************************************/

void
JXColHeaderWidget::ClearColTitle
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
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
JXColHeaderWidget::TableDrawCell
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
		const JString* title = itsTitles->NthElement(cell.x);
		if (title != NULL)
			{
			str      = *title;
			hasTitle = kJTrue;
			}
		}
	if (!hasTitle)
		{
		str = JString(cell.x, 0);
		}

	p.SetFont(JGetDefaultFontName(), kFontSize,
			  JFontStyle(kJTrue, kJFalse, 0, kJFalse, (p.GetColormap())->GetBlackColor()));
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 TurnOnColResizing

	Not inline because it uses assert.

 ******************************************************************************/

void
JXColHeaderWidget::TurnOnColResizing
	(
	const JCoordinate minColWidth
	)
{
	assert( minColWidth > 0 );

	itsAllowColResizingFlag = kJTrue;
	itsMinColWidth          = minColWidth;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXColHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDragType = kInvalidDrag;
	if (!itsAllowColResizingFlag)
		{
		return;
		}

	const JBoolean inDragRegion = InDragRegion(pt, &itsDragCell);

	if (inDragRegion && button == kJXLeftButton)
		{
		itsDragCellRect = GetCellRect(itsDragCell);
		if (modifiers.meta())
			{
			itsDragType = kDragAllCols;
			}
		else
			{
			itsDragType = kDragOneCol;
			}

		JPainter* p = CreateDragOutsidePainter();
		JRect defClipRect = p->GetDefaultClipRect();
		JRect apG = GetApertureGlobal();
		defClipRect.left  = apG.left;
		defClipRect.right = apG.right;
		p->SetDefaultClipRect(defClipRect);

		const JRect enclAp = JXContainer::GlobalToLocal((GetEnclosure())->GetApertureGlobal());
		p->Line(pt.x, enclAp.top, pt.x, enclAp.bottom);
		itsPrevPt = pt;
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXColHeaderWidget::HandleMouseDrag
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

		if (pt.x < itsDragCellRect.left + itsMinColWidth)
			{
			pt.x = itsDragCellRect.left + itsMinColWidth;
			}

		// check if we have moved

		if (pt.x != itsPrevPt.x)
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
			const JCoordinate y = tableAp.ycenter();
			const JRect tableRect(y-1, ptT.x-1, y+1, ptT.x+1);
			if (itsTable->ScrollToRect(tableRect))
				{
				(GetWindow())->Update();
				enclAp = JXContainer::GlobalToLocal(enclApG);	// local coords changed
				}
			else
				{
				// erase the old line

				p->Line(itsPrevPt.x, enclAp.top, itsPrevPt.x, enclAp.bottom);
				}

			// draw the new line

			p->Line(pt.x, enclAp.top, pt.x, enclAp.bottom);

			// ready for next call

			itsPrevPt = pt;
			}
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

	We set the column width to itsPrevPt because that is what the user saw.

 ******************************************************************************/

void
JXColHeaderWidget::HandleMouseUp
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
		p->Line(itsPrevPt.x, enclAp.top, itsPrevPt.x, enclAp.bottom);

		DeleteDragPainter();

		// set the column width(s)

		const JCoordinate colWidth = itsPrevPt.x - itsDragCellRect.left;
		if (itsDragType == kDragOneCol)
			{
			itsTable->SetColWidth(itsDragCell.x, colWidth);
			}
		else
			{
			assert( itsDragType == kDragAllCols );
			itsTable->SetDefaultColWidth(colWidth);
			itsTable->SetAllColWidths(colWidth);
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Show the drag cursor when near a column boundary.

 ******************************************************************************/

void
JXColHeaderWidget::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	const JBoolean inDragRegion = InDragRegion(pt, &cell);
	if (itsAllowColResizingFlag && inDragRegion && modifiers.meta())
		{
		DisplayCursor(itsDragAllLineCursor);
		}
	else if (itsAllowColResizingFlag && inDragRegion)
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
JXColHeaderWidget::InDragRegion
	(
	const JPoint&	pt,
	JPoint*			cell
	)
	const
{
	JCoordinate width;
	JColorIndex color;
	GetColBorderInfo(&width, &color);
	const JCoordinate halfWidth = kDragRegionHalfWidth + (width+1)/2;

	JPoint virtualPt(pt.x - halfWidth, pt.y);
	if (GetCell(virtualPt, cell))
		{
		JRect cellRect = GetCellRect(*cell);
		if (cellRect.right - halfWidth <= pt.x &&
			pt.x <= cellRect.right + halfWidth)
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
JXColHeaderWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	SetRowHeight(1, GetApertureHeight());
}

/******************************************************************************
 Receive (virtual protected)

	Listen for adjustments in table and vertical scrollbar.

 ******************************************************************************/

void
JXColHeaderWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// scrollbar moved

	if (sender == const_cast<JXScrollbar*>(itsHScrollbar) &&
		message.Is(JXScrollbar::kScrolled))
		{
		ScrollTo(itsHScrollbar->GetValue(), 0);
		}

	// columns changed

	else if (sender == itsTable && message.Is(JTable::kColWidthChanged))
		{
		const JTable::ColWidthChanged* info =
			dynamic_cast(const JTable::ColWidthChanged*, &message);
		assert( info != NULL );
		SetColWidth(info->GetIndex(), info->GetNewColWidth());
		}

	else if (sender == itsTable && message.Is(JTable::kAllColWidthsChanged))
		{
		const JTable::AllColWidthsChanged* info =
			dynamic_cast(const JTable::AllColWidthsChanged*, &message);
		assert( info != NULL );
		SetAllColWidths(info->GetNewColWidth());
		}

	else if (sender == itsTable && message.Is(JTable::kColsInserted))
		{
		const JTable::ColsInserted* info =
			dynamic_cast(const JTable::ColsInserted*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			for (JIndex i=1; i<=info->GetCount(); i++)
				{
				itsTitles->InsertAtIndex(info->GetFirstIndex(), (JString*) NULL);
				}
			}
		InsertCols(info->GetFirstIndex(), info->GetCount(), info->GetColWidth());
		}

	else if (sender == itsTable && message.Is(JTable::kColsRemoved))
		{
		const JTable::ColsRemoved* info =
			dynamic_cast(const JTable::ColsRemoved*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			for (JIndex i=1; i<=info->GetCount(); i++)
				{
				itsTitles->DeleteElement(info->GetFirstIndex());
				}
			}
		RemoveNextCols(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == itsTable && message.Is(JTable::kColMoved))
		{
		const JTable::ColMoved* info =
			dynamic_cast(const JTable::ColMoved*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			itsTitles->MoveElementToIndex(info->GetOrigIndex(), info->GetNewIndex());
			}
		MoveCol(info->GetOrigIndex(), info->GetNewIndex());
		}

	else if (sender == itsTable && message.Is(JTable::kColBorderWidthChanged))
		{
		const JTable::ColBorderWidthChanged* info =
			dynamic_cast(const JTable::ColBorderWidthChanged*, &message);
		assert( info != NULL );
		SetColBorderInfo(info->GetNewBorderWidth(), (GetColormap())->GetDefaultBackColor());
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
JXColHeaderWidget::AdjustToTable()
{
	assert( itsTable != NULL && itsTitles == NULL );

	JCoordinate width;
	JColorIndex color;
	itsTable->GetColBorderInfo(&width, &color);
	SetColBorderInfo(width, (GetColormap())->GetDefaultBackColor());

	RemoveAllCols();

	const JSize colCount = itsTable->GetColCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		InsertCols(i, 1, itsTable->GetColWidth(i));
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JXColHeaderWidget::CreateXInputField
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
JXColHeaderWidget::PrepareDeleteXInputField()
{
	assert( 0 /* The programmer forgot to override JXEditTable::PrepareDeleteXInputField() */ );
}
