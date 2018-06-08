/******************************************************************************
 MDRecordTable.cc

	BASE CLASS = JXEditTable

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDRecordTable.h"
#include "MDRecordList.h"
#include "MDRecord.h"
#include "MDRecordDataField.h"
#include "mdGlobals.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXStaticText.h>
#include <JXColorManager.h>
#include <JXDeleteObjectTask.h>
#include <JPainter.h>
#include <JPagePrinter.h>
#include <JTableSelection.h>
#include <jASCIIConstants.h>
#include <jTime.h>
#include <jAssert.h>

const JCoordinate kDefColWidth	= 100;
const JCoordinate kDefRowHeight	= 20;
const JCoordinate kHMarginWidth	= 5;

/******************************************************************************
 Constructor

	We take ownership of the record list.

 *****************************************************************************/

MDRecordTable::MDRecordTable
	(
	MDRecordList*		list,
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
	AppendCols(MDRecordList::kColumnCount);
	SetColWidth(MDRecordList::kRecordState, 20);
	SetColWidth(MDRecordList::kRecordFile,  200);
	SetColWidth(MDRecordList::kRecordLine,  50);
	SetColWidth(MDRecordList::kRecordSize,  50);
	SetColWidth(MDRecordList::kRecordArray, 20);

	SetRowBorderInfo(0, GetColormap()->GetBlackColor());
	SetColBorderInfo(0, GetColormap()->GetBlackColor());

	ListenTo(itsRecordList);
	UpdateTable();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDRecordTable::~MDRecordTable()
{
	jdelete itsRecordList;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MDRecordTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRecordList && message.Is(MDRecordList::kPrepareForUpdate))
		{
		StopListening(itsSelectedRecord);
		if (GetSelectedRecord(&itsSelectedRecord))
			{
			ClearWhenGoingAway(itsSelectedRecord, &itsSelectedRecord);
			}
		}

	else if (sender == itsRecordList && message.Is(MDRecordList::kListChanged))
		{
		UpdateTable();
		}

	else
		{
		JXEditTable::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateTable (private)

 ******************************************************************************/

void
MDRecordTable::UpdateTable()
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
MDRecordTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	DrawRowBackground(p, cell, rect, (p.GetColormap())->GetGrayColor(95));
	HilightIfSelected(p, cell, rect);

	const MDRecord& record = *(itsRecordList->GetRecord(cell.y));

	JString str;
	JPainter::HAlignment halign = JPainter::kHAlignLeft;
	if (cell.x == MDRecordList::kRecordState)
		{
		DrawRecordState(record, p, rect);
		return;
		}
	else if (cell.x == MDRecordList::kRecordFile)
		{
		str    = record.GetNewFile();
		halign = JPainter::kHAlignLeft;
		}
	else if (cell.x == MDRecordList::kRecordLine)
		{
		str	   = JString((JUInt64) record.GetNewLine());
		halign = JPainter::kHAlignRight;
		}
	else if (cell.x == MDRecordList::kRecordSize)
		{
		str    = JString((JUInt64) record.GetSize());
		halign = JPainter::kHAlignRight;
		}
	else if (cell.x == MDRecordList::kRecordArray)
		{
		str    = record.IsArrayNew() ? "[]" : "";
		halign = JPainter::kHAlignCenter;
		}
	else if (cell.x == MDRecordList::kRecordData)
		{
		str    = record.GetData();
		halign = JPainter::kHAlignLeft;

		const JCoordinate w  = p.GetStringWidth(str) + 2*kHMarginWidth;
		const JCoordinate dw = rect.left - GetColLeft(MDRecordList::kRecordData);
		if (w > GetColWidth(MDRecordList::kRecordData) - dw)
			{
			AdjustDataColWidth(w + dw);
			}
		}

	JRect r = rect;
	r.Shrink(kHMarginWidth, 0);
	p.JPainter::String(r, str, halign, JPainter::kVAlignCenter);
}

/******************************************************************************
 DrawRowBackground (private)

 ******************************************************************************/

void
MDRecordTable::DrawRowBackground
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
		p.SetFilling(kJTrue);
		p.Rect(rect);
		p.SetFilling(kJFalse);
		}
}

/******************************************************************************
 DrawRecordState (private)

 ******************************************************************************/

void
MDRecordTable::DrawRecordState
	(
	const MDRecord&	record,
	JPainter&		p,
	const JRect&	rect
	)
{
	if (!record.IsValid())
		{
		JRect r(rect.ycenter()-3, rect.xcenter()-3,
				rect.ycenter()+4, rect.xcenter()+4);
		p.SetPenColor(JColorManager::GetRedColor());
		p.SetFilling(kJTrue);
		p.Ellipse(r);
		p.SetPenColor(JColorManager::GetBlackColor());
		p.SetFilling(kJFalse);
		p.Ellipse(r);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
MDRecordTable::HandleMouseDown
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
	else if (clickCount == 2 && cell.x == MDRecordList::kRecordData)
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
MDRecordTable::HandleFocusEvent()
{
	JXEditTable::HandleFocusEvent();
	itsKeyBuffer.Clear();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
MDRecordTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const JBoolean hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (key == ' ' || key == kJEscapeKey)
		{
		itsKeyBuffer.Clear();
		GetTableSelection().ClearSelection();
		}

	else if (key == kJReturnKey)
		{
		OpenSelectedFiles();
		}

	else if (key == kJUpArrow || key == kJDownArrow)
		{
		itsKeyBuffer.Clear();
		if (!hadSelection && key == kJUpArrow && GetRowCount() > 0)
			{
			s.SelectRow(GetRowCount());
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			if (s.GetFirstSelectedCell(&topSelCell))
				{
				s.SelectRow(topSelCell.y);
				}
			}
		}

	// incremental search

	else if (JXIsPrint(key) && !modifiers.control() && !modifiers.meta())
		{
		itsKeyBuffer.AppendCharacter(key);

		MDRecord* record;
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
		if (JXIsPrint(key))
			{
			itsKeyBuffer.Clear();
			}
		JXEditTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 OpenSelectedFiles

 ******************************************************************************/

void
MDRecordTable::OpenSelectedFiles()
	const
{
	itsKeyBuffer.Clear();

	const JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	JPoint cell;
	while (iter.Next(&cell) && cell.x == 1)
		{
		const MDRecord* record = itsRecordList->GetRecord(cell.y);
		(MDGetApplication())->OpenFile(record->GetNewFile(), record->GetNewLine());
		}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

JBoolean
MDRecordTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return JI2B(cell.x == MDRecordList::kRecordData);
}

/******************************************************************************
 CreateInputField (virtual protected)

 ******************************************************************************/

JXInputField*
MDRecordTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	const MDRecord* record = itsRecordList->GetRecord(cell.y);
	const JString& text    = record->GetData();

	JXInputField* field =
		jnew MDRecordDataField(text, this, JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  x,y, w,h);
	assert( field != nullptr );

	return field;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
MDRecordTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	return kJTrue;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
MDRecordTable::PrepareDeleteXInputField()
{
}

/******************************************************************************
 GetSelectedRecord

 ******************************************************************************/

JBoolean
MDRecordTable::GetSelectedRecord
	(
	const MDRecord** entry
	)
	const
{
	JPoint cell;
	if (GetTableSelection().GetFirstSelectedCell(&cell))
		{
		*entry = itsRecordList->GetRecord(cell.y);
		return kJTrue;
		}
	else
		{
		*entry = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 AdjustDataColWidth (private)

 ******************************************************************************/

void
MDRecordTable::AdjustDataColWidth
	(
	const JCoordinate w
	)
{
	UpdateScrollbars();
	itsMinDataWidth = JMax(itsMinDataWidth, w);
	SetColWidth(MDRecordList::kRecordData, itsMinDataWidth);
}

/******************************************************************************
 Print header and footer (virtual protected)

 ******************************************************************************/

JCoordinate
MDRecordTable::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight();
}

JCoordinate
MDRecordTable::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
MDRecordTable::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	p.String(pageRect.left, pageRect.top, GetWindow()->GetTitle());
	const JString dateStr = JGetTimeStamp();
	p.String(pageRect.left, pageRect.top, dateStr,
			 pageRect.width(), JPainter::kHAlignRight);
}

void
MDRecordTable::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	const JString pageNumberStr = "Page " + JString(p.GetPageIndex());
	p.String(pageRect.left, pageRect.bottom - footerHeight, pageNumberStr,
			 pageRect.width(), JPainter::kHAlignCenter,
			 footerHeight, JPainter::kVAlignBottom);
}
