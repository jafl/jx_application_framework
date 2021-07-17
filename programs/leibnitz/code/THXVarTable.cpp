/******************************************************************************
 THXVarTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THXVarTable.h"
#include "THXVarList.h"
#include <JXExprInput.h>
#include <jXConstants.h>
#include <JFunction.h>
#include <JFontManager.h>
#include <JPainter.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

THXVarTable::THXVarTable
	(
	THXVarList*			varList,
	JXTextMenu*			fontMenu,
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
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	const JSize rowHeight = 2*kVMarginWidth +
		JFontManager::GetDefaultFont().GetLineHeight(GetFontManager());
	SetDefaultRowHeight(rowHeight);

	itsVarList   = varList;
	itsTextInput = nullptr;
	itsFontMenu  = fontMenu;

	AppendCols(2);	// name, function

	const JSize count = itsVarList->GetElementCount() - THXVarList::kUserFnOffset;
	AppendRows(count);

	FitToEnclosure();	// make sure SetColWidth() won't fail
	ListenTo(this);		// adjust fn col width

	SetColWidth(kNameColumn, GetApertureWidth()/3);
	// kFnColumn width set automatically
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXVarTable::~THXVarTable()
{
}

/******************************************************************************
 NewConstant

 ******************************************************************************/

void
THXVarTable::NewConstant()
{
	const JIndex rowIndex = itsVarList->NewFunction() - THXVarList::kUserFnOffset;
	InsertRows(rowIndex, 1);
	BeginEditing(JPoint(1,rowIndex));
}

/******************************************************************************
 OKToRemoveSelectedConstant

 ******************************************************************************/

bool
THXVarTable::OKToRemoveSelectedConstant()
	const
{
	JPoint cell;
	return GetEditedCell(&cell) &&
				 itsVarList->OKToRemoveFunction(cell.y + THXVarList::kUserFnOffset);
}

/******************************************************************************
 RemoveSelectedConstant

 ******************************************************************************/

void
THXVarTable::RemoveSelectedConstant()
{
	JPoint cell;
	if (GetEditedCell(&cell))
		{
		CancelEditing();
		RemoveRow(cell.y);
		itsVarList->RemoveFunction(cell.y + THXVarList::kUserFnOffset);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
THXVarTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
		{
		BeginEditing(cell);
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
THXVarTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
		{
		return;
		}

	JRect r = rect;
	r.left += kHMarginWidth;

	const JIndex varIndex = cell.y + THXVarList::kUserFnOffset;
	if (cell.x == kNameColumn)
		{
		p.String(r, itsVarList->GetVariableName(varIndex),
				 JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
	else if (cell.x == kFnColumn)
		{
		const JString s = (itsVarList->GetFunction(varIndex))->Print();
		p.String(r, s, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
THXVarTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr );

	itsTextInput = jnew JXExprInput(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsTextInput != nullptr );

	const JIndex varIndex = cell.y + THXVarList::kUserFnOffset;
	if (cell.x == kNameColumn)
		{
		itsTextInput->SetVarName(itsVarList->GetVariableName(varIndex));
		}
	else
		{
		assert( cell.x == kFnColumn );
		const JString s = (itsVarList->GetFunction(varIndex))->Print();
		itsTextInput->SetVarName(s);
		}

	itsOrigText = itsTextInput->GetVarName();

	itsTextInput->SetIsRequired();
	itsTextInput->SetFontMenu(itsFontMenu);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
THXVarTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != nullptr );

	const JString s = itsTextInput->GetVarName();
	if (s == itsOrigText)
		{
		return true;
		}
	else if (itsTextInput->InputValid())
		{
		bool ok;
		const JIndex varIndex = cell.y + THXVarList::kUserFnOffset;
		if (cell.x == kNameColumn)
			{
			ok = itsVarList->SetVariableName(varIndex, s);
			}
		else
			{
			assert( cell.x == kFnColumn );
			ok = itsVarList->SetFunction(varIndex, s);
			}

		return ok;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
THXVarTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
}

/******************************************************************************
 Receive (virtual protected)

	We don't have to listen for changes to itsVarList because we are
	the only ones who change it.

 ******************************************************************************/

void
THXVarTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(kColWidthChanged))
		{
		// do it regardless of which column changed so they can't shrink fn column
		AdjustColWidths();
		}

	JXEditTable::Receive(sender, message);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
THXVarTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
THXVarTable::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();

	JCoordinate lineWidth;
	JColorID color;
	GetColBorderInfo(&lineWidth, &color);
	const JSize usedWidth = GetColWidth(kNameColumn) + lineWidth;

	if (apWidth > usedWidth)
		{
		SetColWidth(kFnColumn, apWidth - usedWidth);
		}
	else
		{
		const JSize nameWidth = apWidth/3;
		SetColWidth(kNameColumn, nameWidth);
		SetColWidth(kFnColumn, apWidth - nameWidth - lineWidth);
		}
}
