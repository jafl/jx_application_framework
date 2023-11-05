/******************************************************************************
 RecordTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "RecordTable.h"
#include "RecordList.h"
#include "Record.h"
#include "RecordDataField.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kDefColWidth	= 100;
const JCoordinate kDefRowHeight	= 20;
const JCoordinate kHMarginWidth	= 5;

/******************************************************************************
 Constructor

	We take ownership of the record list.

 *****************************************************************************/

RecordTable::RecordTable
	(
	RecordList*		list,
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
	JXEditTable(kDefRowHeight, kDefColWidth, scrollbarSet,
				enclosure, hSizing, vSizing, x,y, w,h),
	itsRecordList(list),
	itsSelectedRecord(nullptr),
	itsMinDataWidth(kDefColWidth)
{
	AppendCols(RecordList::kColumnCount);
	SetColWidth(RecordList::kRecordState, 20);
	SetColWidth(RecordList::kRecordFile,  200);
	SetColWidth(RecordList::kRecordLine,  50);
	SetColWidth(RecordList::kRecordSize,  50);
	SetColWidth(RecordList::kRecordArray, 20);

	SetRowBorderInfo(0, JColorManager::GetBlackColor());
	SetColBorderInfo(0, JColorManager::GetBlackColor());

	ListenTo(itsRecordList, std::function([this](const RecordList::PrepareForUpdate&)
	{
		StopListening(itsSelectedRecord);
		if (GetSelectedRecord(&itsSelectedRecord))
		{
			ClearWhenGoingAway(itsSelectedRecord, &itsSelectedRecord);
		}
	}));

	ListenTo(itsRecordList, std::function([this](const RecordList::ListChanged&)
	{
		UpdateTable();
	}));

	UpdateTable();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

RecordTable::~RecordTable()
{
	jdelete itsRecordList;
}

/******************************************************************************
 UpdateTable (private)

 ******************************************************************************/

void
RecordTable::UpdateTable()
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();

	const JSize oldCount = GetRowCount();
	const JSize newCount = itsRecordList->GetElementCount();
	if (newCount > oldCount)
	{
		AppendRows(newCount - oldCount);
	}
	else if (oldCount > newCount)
	{
		RemoveNextRows(1, oldCount - newCount);
	}

	JIndex index;
	if (itsSelectedRecord != nullptr)
	{
		if (itsRecordList->GetRecordIndex(itsSelectedRecord, &index))
		{
			s.SelectRow(index);
		}

		StopListening(itsSelectedRecord);
		itsSelectedRecord = nullptr;
	}

	Refresh();
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
RecordTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	DrawRowBackground(p, cell, rect, JColorManager::GetGrayColor(95));
	HilightIfSelected(p, cell, rect);

	const Record& record = *(itsRecordList->GetRecord(cell.y));

	JString str;
	JPainter::HAlign halign = JPainter::HAlign::kLeft;
	if (cell.x == RecordList::kRecordState)
	{
		DrawRecordState(record, p, rect);
		return;
	}
	else if (cell.x == RecordList::kRecordFile)
	{
		str    = record.GetNewFile();
		halign = JPainter::HAlign::kLeft;
	}
	else if (cell.x == RecordList::kRecordLine)
	{
		str	   = JString((JUInt64) record.GetNewLine());
		halign = JPainter::HAlign::kRight;
	}
	else if (cell.x == RecordList::kRecordSize)
	{
		str    = JString((JUInt64) record.GetSize());
		halign = JPainter::HAlign::kRight;
	}
	else if (cell.x == RecordList::kRecordArray)
	{
		str    = record.IsArrayNew() ? "[]" : "";
		halign = JPainter::HAlign::kCenter;
	}
	else if (cell.x == RecordList::kRecordData)
	{
		str    = record.GetData();
		halign = JPainter::HAlign::kLeft;

		const JCoordinate w  = p.GetStringWidth(str) + 2*kHMarginWidth;
		const JCoordinate dw = rect.left - GetColLeft(RecordList::kRecordData);
		if (w > GetColWidth(RecordList::kRecordData) - dw)
		{
			AdjustDataColWidth(w + dw);
		}
	}

	JRect r = rect;
	r.Shrink(kHMarginWidth, 0);
	p.String(r, str, halign, JPainter::VAlign::kCenter);
}

/******************************************************************************
 DrawRowBackground (private)

 ******************************************************************************/

void
RecordTable::DrawRowBackground
	(
	JPainter&			p,
	const JPoint&		cell,
	const JRect&		rect,
	const JColorID	color
	)
{
	if (cell.y % 2 == 1)
	{
		p.SetPenColor(color);
		p.SetFilling(true);
		p.Rect(rect);
		p.SetFilling(false);
	}
}

/******************************************************************************
 DrawRecordState (private)

 ******************************************************************************/

void
RecordTable::DrawRecordState
	(
	const Record&	record,
	JPainter&		p,
	const JRect&	rect
	)
{
	if (!record.IsValid())
	{
		JRect r(rect.ycenter()-3, rect.xcenter()-3,
				rect.ycenter()+4, rect.xcenter()+4);
		p.SetPenColor(JColorManager::GetRedColor());
		p.SetFilling(true);
		p.Ellipse(r);
		p.SetPenColor(JColorManager::GetBlackColor());
		p.SetFilling(false);
		p.Ellipse(r);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
RecordTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (ScrollForWheel(button, modifiers))
	{
		return;
	}

	JTableSelection& s = GetTableSelection();
	itsKeyBuffer.Clear();

	JPoint cell;
	if (!GetCell(pt, &cell))
	{
		s.ClearSelection();
		return;
	}

	if (clickCount == 1)
	{
		s.ClearSelection();
		s.SelectRow(cell.y);
	}
	else if (clickCount == 2 && cell.x == RecordList::kRecordData)
	{
		BeginEditing(cell);
	}
	else if (clickCount == 2)
	{
		OpenSelectedFiles();
	}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
RecordTable::HandleFocusEvent()
{
	JXEditTable::HandleFocusEvent();
	itsKeyBuffer.Clear();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
RecordTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const bool hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (c == ' ' || c == kJEscapeKey)
	{
		itsKeyBuffer.Clear();
		GetTableSelection().ClearSelection();
	}

	else if (c == kJReturnKey)
	{
		OpenSelectedFiles();
	}

	else if (c == kJUpArrow || c == kJDownArrow)
	{
		itsKeyBuffer.Clear();
		if (!hadSelection && c == kJUpArrow && GetRowCount() > 0)
		{
			s.SelectRow(GetRowCount());
		}
		else
		{
			HandleSelectionKeyPress(c, modifiers);
			if (s.GetFirstSelectedCell(&topSelCell))
			{
				s.SelectRow(topSelCell.y);
			}
		}
	}

	// incremental search

	else if (c.IsPrint() && !modifiers.control() && !modifiers.meta())
	{
		itsKeyBuffer.Append(JUtf8Character(c));

		Record* record;
		JIndex index;
		if (itsRecordList->ClosestMatch(itsKeyBuffer, &record) &&
			itsRecordList->GetRecordIndex(record, &index))
		{
			(GetTableSelection()).ClearSelection();
			(GetTableSelection()).SelectRow(index);
			TableScrollToCell(JPoint(1, index));
		}
		else
		{
			(GetTableSelection()).ClearSelection();
		}
	}

	else
	{
		if (c.IsPrint())
		{
			itsKeyBuffer.Clear();
		}
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 OpenSelectedFiles

 ******************************************************************************/

void
RecordTable::OpenSelectedFiles()
	const
{
	itsKeyBuffer.Clear();

	const JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	JPoint cell;
	while (iter.Next(&cell) && cell.x == 1)
	{
		const Record* record = itsRecordList->GetRecord(cell.y);
		(GetApplication())->OpenFile(record->GetNewFile(), record->GetNewLine());
	}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

bool
RecordTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return cell.x == RecordList::kRecordData;
}

/******************************************************************************
 CreateInputField (virtual protected)

 ******************************************************************************/

JXInputField*
RecordTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	const Record* record = itsRecordList->GetRecord(cell.y);
	const JString& text    = record->GetData();

	JXInputField* field =
		jnew RecordDataField(text, this, JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  x,y, w,h);

	return field;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
RecordTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	return true;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
RecordTable::PrepareDeleteXInputField()
{
}

/******************************************************************************
 GetSelectedRecord

 ******************************************************************************/

bool
RecordTable::GetSelectedRecord
	(
	const Record** entry
	)
	const
{
	JPoint cell;
	if (GetTableSelection().GetFirstSelectedCell(&cell))
	{
		*entry = itsRecordList->GetRecord(cell.y);
		return true;
	}
	else
	{
		*entry = nullptr;
		return false;
	}
}

/******************************************************************************
 AdjustDataColWidth (private)

 ******************************************************************************/

void
RecordTable::AdjustDataColWidth
	(
	const JCoordinate w
	)
{
	UpdateScrollbars();
	itsMinDataWidth = JMax(itsMinDataWidth, w);
	SetColWidth(RecordList::kRecordData, itsMinDataWidth);
}

/******************************************************************************
 Print header and footer (virtual protected)

 ******************************************************************************/

JCoordinate
RecordTable::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight();
}

JCoordinate
RecordTable::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
RecordTable::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	p.String(pageRect.left, pageRect.top, GetWindow()->GetTitle());
	const JString dateStr = JGetTimeStamp();
	p.String(pageRect.left, pageRect.top, dateStr,
			 pageRect.width(), JPainter::HAlign::kRight);
}

void
RecordTable::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	const JString pageNumberStr = "Page " + JString((JUInt64) p.GetPageIndex());
	p.String(pageRect.left, pageRect.bottom - footerHeight, pageNumberStr,
			 pageRect.width(), JPainter::HAlign::kCenter,
			 footerHeight, JPainter::VAlign::kBottom);
}
