/******************************************************************************
 CMEditCommandsTable.cpp

	BASE CLASS = public JXEditTable

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include "CMEditCommandsTable.h"
#include "CMEditCommandsDialog.h"
#include "CMUpdateCommandsTableWidth.h"
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXWindowPainter.h>
#include <JXFontManager.h>
#include <JTableSelection.h>
#include <jXGlobals.h>
#include <jAssert.h>

const JCoordinate kDefRowHeight	= 15;
const JCoordinate kDefColWidth	= 100;
const JCoordinate kHMarginWidth = 5;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 *****************************************************************************/

CMEditCommandsTable::CMEditCommandsTable
	(
	CMEditCommandsDialog*	dialog,
	JXTextButton*			removeButton,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXEditTable(kDefRowHeight, kDefColWidth, scrollbarSet,
				enclosure, hSizing, vSizing, x, y, w, h),
	itsDialog(dialog),
	itsRemoveButton(removeButton)
{
	itsMinColWidth = 10;

	itsRemoveButton->Deactivate();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMEditCommandsTable::~CMEditCommandsTable()
{
}

/******************************************************************************
 SyncWithData (public)

 ******************************************************************************/

void
CMEditCommandsTable::SyncWithData()
{
	const JSize count = itsDialog->GetStringCount();
	if (GetColCount() == 0)
		{
		AppendCols(1, kDefColWidth);
		}
	RemoveAllRows();
	AppendRows(count, kDefRowHeight);

	const JSize fontHeight = JFontManager::GetDefaultFont().GetLineHeight(GetFontManager());
	const JCoordinate rowHeight = fontHeight + 2*kVMarginWidth;
	SetDefaultRowHeight(rowHeight);
	SetAllRowHeights(rowHeight);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CMEditCommandsTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	AdjustColWidth();
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
CMEditCommandsTable::AdjustColWidth()
{
	JCoordinate w = GetApertureWidth();
	if (w < itsMinColWidth)
		{
		w = itsMinColWidth;
		}
	SetColWidth(1,w);
}

/******************************************************************************
 TableDrawCell (protected)

 ******************************************************************************/

void
CMEditCommandsTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);
	const JString& str	= itsDialog->GetString(cell.y);

	const JCoordinate w = p.GetStringWidth(str) + 2*kHMarginWidth;
	if (w > GetColWidth(1))
		{
		itsMinColWidth = w;
		auto* task = jnew CMUpdateCommandsTableWidth(this);
		assert( task != nullptr );
		task->Go();
		}

	// draw string

	JRect r = rect;
	r.left += kHMarginWidth;
	p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMEditCommandsTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (GetCell(pt, &cell) && button == kJXLeftButton)
		{
		BeginEditing(cell);
		}
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
CMEditCommandsTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	itsDialog->SetString(cell.y, itsCmdInput->GetText()->GetText());
	return true;
}

/******************************************************************************
 CreateXInputField (virtual protected))

 ******************************************************************************/

JXInputField*
CMEditCommandsTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsCmdInput =
		jnew JXInputField(this, kFixedLeft, kFixedTop,
			x+kHMarginWidth, y,
			w - kHMarginWidth, h);
	assert(itsCmdInput != nullptr);

	itsCmdInput->GetText()->SetText(itsDialog->GetString(cell.y));
	itsRemoveButton->Activate();

	return itsCmdInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CMEditCommandsTable::PrepareDeleteXInputField()
{
	itsCmdInput	= nullptr;
	itsRemoveButton->Deactivate();
}

/******************************************************************************
 NewStringAppended

 ******************************************************************************/

void
CMEditCommandsTable::NewStringAppended()
{
	AppendRows(1, kDefRowHeight);
	BeginEditing(JPoint(1, itsDialog->GetStringCount()));
}

/******************************************************************************
 RemoveCurrent

 ******************************************************************************/

void
CMEditCommandsTable::RemoveCurrent()
{
	JPoint cell;
	if (GetEditedCell(&cell))
		{
		RemoveRow(cell.y);
		itsDialog->DeleteString(cell.y);
		return;
		}
	JTableSelection& s	= GetTableSelection();
	JTableSelectionIterator iter(&s);
	while (iter.Next(&cell))
		{
		RemoveRow(cell.y);
		itsDialog->DeleteString(cell.y);
		}
}
