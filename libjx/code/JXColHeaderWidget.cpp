/******************************************************************************
 JXColHeaderWidget.cpp

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXEditTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXColHeaderWidget.h"
#include "JXWindow.h"
#include "JXScrollbarSet.h"
#include "JXScrollbar.h"
#include "JXDragPainter.h"
#include "jXPainterUtil.h"
#include "JXFontManager.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

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
	JXEditTable(h,1, nullptr, enclosure, hSizing,vSizing, x,y, w,h)
{
	assert( table != nullptr && scrollbarSet != nullptr );

	itsTable = table;
	itsTable->SetColHeader(this);
	ListenTo(itsTable);

	itsHScrollbar = scrollbarSet->GetHScrollbar();
	ListenTo(itsHScrollbar);

	itsTitles = nullptr;

	itsAllowColResizingFlag = false;
	itsMinColWidth          = 1;

	itsDragType = kInvalidDrag;

	itsDragLineCursor    = JXGetDragVertLineCursor(GetDisplay());
	itsDragAllLineCursor = JXGetDragAllVertLineCursor(GetDisplay());

	SetDrawOrder(kDrawByRow);
	SetRowBorderInfo(0, JColorManager::GetBlackColor());

	// override JXEditTable

	WantInput(false);
	SetBackColor(JColorManager::GetDefaultBackColor());

	AppendRows(1, GetApertureHeight());
	AdjustToTable();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXColHeaderWidget::~JXColHeaderWidget()
{
	jdelete itsTitles;
}

/******************************************************************************
 GetColTitle

	Returns true if there is a title for the specified column.

 ******************************************************************************/

bool
JXColHeaderWidget::GetColTitle
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
 SetColTitle

 ******************************************************************************/

void
JXColHeaderWidget::SetColTitle
	(
	const JIndex	index,
	const JString&	title
	)
{
	if (itsTitles == nullptr)
	{
		itsTitles = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

		const JSize colCount = GetColCount();
		for (JIndex i=1; i<=colCount; i++)
		{
			itsTitles->Append(nullptr);
		}
	}

	itsTitles->SetItem(index, title, JPtrArrayT::kDelete);
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
	if (itsTitles != nullptr)
	{
		itsTitles->SetToNull(index, JPtrArrayT::kDelete);
	}
}

/******************************************************************************
 CBSetColumnTitles

 ******************************************************************************/

void
JXColHeaderWidget::SetColumnTitles
	(
	const JUtf8Byte*	className,
	const JSize			count
	)
{
	for (JIndex i=1; i<=count; i++)
	{
		const JString id = "Column" + JString(i) + "::" + className;
		SetColTitle(i, JGetString(id.GetBytes()));
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
	bool hasTitle = false;
	if (itsTitles != nullptr)
	{
		const JString* title = itsTitles->GetItem(cell.x);
		if (title != nullptr)
		{
			str      = *title;
			hasTitle = true;
		}
	}
	if (!hasTitle)
	{
		str = JString(cell.x);
	}

	const JFont font = JFontManager::GetFont(
			JFontManager::GetDefaultFontName(),
			JFontManager::GetDefaultRowColHeaderFontSize(),
			JFontStyle(true, false, 0, false));
	p.SetFont(font);
	p.String(rect, str, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
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

	itsAllowColResizingFlag = true;
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

	const bool inDragRegion = InDragRegion(pt, &itsDragCell);

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

		const JRect enclAp = JXContainer::GlobalToLocal(GetEnclosure()->GetApertureGlobal());
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
			const JCoordinate y = tableAp.ycenter();
			const JRect tableRect(y-1, ptT.x-1, y+1, ptT.x+1);
			if (itsTable->ScrollToRect(tableRect))
			{
				GetWindow()->Update();
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

		JPainter* p = nullptr;
		const bool ok = GetDragPainter(&p);
		assert( ok );

		const JRect enclAp = JXContainer::GlobalToLocal(GetEnclosure()->GetApertureGlobal());
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
	const bool inDragRegion = InDragRegion(pt, &cell);
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

bool
JXColHeaderWidget::InDragRegion
	(
	const JPoint&	pt,
	JPoint*			cell
	)
	const
{
	JCoordinate width;
	JColorID color;
	GetColBorderInfo(&width, &color);
	const JCoordinate halfWidth = kDragRegionHalfWidth + (width+1)/2;

	JPoint virtualPt(pt.x - halfWidth, pt.y);
	if (GetCell(virtualPt, cell))
	{
		JRect cellRect = GetCellRect(*cell);
		if (cellRect.right - halfWidth <= pt.x &&
			pt.x <= cellRect.right + halfWidth)
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
		auto& info = dynamic_cast<const JTable::ColWidthChanged&>(message);
		SetColWidth(info.GetIndex(), info.GetNewColWidth());
	}

	else if (sender == itsTable && message.Is(JTable::kAllColWidthsChanged))
	{
		auto& info = dynamic_cast<const JTable::AllColWidthsChanged&>(message);
		SetAllColWidths(info.GetNewColWidth());
	}

	else if (sender == itsTable && message.Is(JTable::kColsInserted))
	{
		auto& info = dynamic_cast<const JTable::ColsInserted&>(message);
		if (itsTitles != nullptr)
		{
			for (JIndex i=1; i<=info.GetCount(); i++)
			{
				itsTitles->InsertAtIndex(info.GetFirstIndex(), (JString*) nullptr);
			}
		}
		InsertCols(info.GetFirstIndex(), info.GetCount(), info.GetColWidth());
	}

	else if (sender == itsTable && message.Is(JTable::kColsRemoved))
	{
		auto& info = dynamic_cast<const JTable::ColsRemoved&>(message);
		if (itsTitles != nullptr)
		{
			for (JIndex i=1; i<=info.GetCount(); i++)
			{
				itsTitles->DeleteItem(info.GetFirstIndex());
			}
		}
		RemoveNextCols(info.GetFirstIndex(), info.GetCount());
	}

	else if (sender == itsTable && message.Is(JTable::kColMoved))
	{
		auto& info = dynamic_cast<const JTable::ColMoved&>(message);
		if (itsTitles != nullptr)
		{
			itsTitles->MoveItemToIndex(info.GetOrigIndex(), info.GetNewIndex());
		}
		MoveCol(info.GetOrigIndex(), info.GetNewIndex());
	}

	else if (sender == itsTable && message.Is(JTable::kColBorderWidthChanged))
	{
		auto& info = dynamic_cast<const JTable::ColBorderWidthChanged&>(message);
		SetColBorderInfo(info.GetNewBorderWidth(), JColorManager::GetDefaultBackColor());
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
	assert( itsTable != nullptr && itsTitles == nullptr );

	JCoordinate width;
	JColorID color;
	itsTable->GetColBorderInfo(&width, &color);
	SetColBorderInfo(width, JColorManager::GetDefaultBackColor());

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
	assert_msg( 0, "The programmer forgot to override JXEditTable::CreateXInputField()" );
	return nullptr;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
JXColHeaderWidget::PrepareDeleteXInputField()
{
	assert_msg( 0, "The programmer forgot to override JXEditTable::PrepareDeleteXInputField()" );
}
