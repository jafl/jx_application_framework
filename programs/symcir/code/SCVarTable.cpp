/******************************************************************************
 SCVarTable.cpp

	BASE CLASS = JXTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCVarTable.h"
#include "SCCircuitVarList.h"
#include "SCVarListDirector.h"
#include "SCEditFnDialog.h"
#include "SCEditVarValueDialog.h"

#include <JXWindowDirector.h>
#include <JXWindow.h>
#include <JXColHeaderWidget.h>
#include <jXConstants.h>
#include <jXGlobals.h>
#include <jXKeysym.h>

#include <JFunction.h>
#include <JPainter.h>
#include <JString.h>
#include <JFontManager.h>
#include <JTableSelection.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JSize kColCount = 3;

const JCoordinate kMinColWidth = 50;

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

SCVarTable::SCVarTable
	(
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
	JXTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsVarList = NULL;

	itsVisibleList = new JArray<JIndex>;
	assert( itsVisibleList != NULL );

	const JSize fontHeight =
		(GetFontManager())->GetLineHeight(JGetDefaultFontName(), kJDefaultFontSize,
										  JFontStyle());
	const JCoordinate rowHeight = fontHeight + 2*kVMarginWidth;
	SetDefaultRowHeight(rowHeight);

	AppendCols(2, kMinColWidth);
	AppendCols(1, 2*kMinColWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCVarTable::~SCVarTable()
{
	delete itsVisibleList;
}

/******************************************************************************
 SetVarList

 ******************************************************************************/

void
SCVarTable::SetVarList
	(
	SCCircuitVarList* varList
	)
{
	if (itsVarList != NULL)
		{
		StopListening(itsVarList);
		}
	itsVisibleList->RemoveAll();
	RemoveAllRows();

	itsVarList = varList;

	if (itsVarList != NULL)
		{
		const JSize count = itsVarList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (itsVarList->IsVisible(i))
				{
				itsVisibleList->AppendElement(i);
				AppendRows(1);
				}
			}

		ListenTo(itsVarList);
		}
}

/******************************************************************************
 ConfigureColHeader

 ******************************************************************************/

void
SCVarTable::ConfigureColHeader
	(
	JXColHeaderWidget* colHeader
	)
{
	colHeader->TurnOnColResizing(kMinColWidth);
	colHeader->SetColTitle(1, "Name");
	colHeader->SetColTitle(2, "Value");
	colHeader->SetColTitle(3, "Function");
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
SCVarTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	assert( itsVarList != NULL );

	HilightIfSelected(p, cell, rect);

	p.SetFont(JGetDefaultFontName(), kJDefaultFontSize, JFontStyle());

	const JIndex varIndex = RowToVarIndex(cell.y);

	JString str;
	if (cell.x == 1)
		{
		str = itsVarList->GetVariableName(varIndex);
		}
	else if (cell.x == 2)
		{
		JComplex value;
		if (itsVarList->GetNumericValue(varIndex,1, &value))
			{
			str = JPrintComplexNumber(value);
			}
		else
			{
			str = "error";
			}
		}
	else if (cell.x == 3)
		{
		const JFunction* f;
		if (itsVarList->GetFunction(varIndex, &f))
			{
			str = f->Print();
			}
		}

	JRect r = rect;
	r.left += kHMarginWidth;
	p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
SCVarTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button != kJXLeftButton)
		{
		ScrollForWheel(button, modifiers);
		return;
		}
	else if (!GetCell(pt, &cell))
		{
		JTableSelection& s = GetTableSelection();
		s.ClearSelection();
		return;
		}

	if (itsVarList != NULL && clickCount == 1)
		{
		SelectRow(cell.y);
		}
	else if (itsVarList != NULL && clickCount == 2)
		{
		EditFunction(RowToVarIndex(cell.y));
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
SCVarTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJReturnKey)
		{
		EditFunction();
		}
	else if (key == kJUpArrow)
		{
		MoveSelection(-1);
		}
	else if (key == kJDownArrow)
		{
		MoveSelection(+1);
		}
	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 EditFunction

 ******************************************************************************/

void
SCVarTable::EditFunction
	(
	const JIndex origVarIndex
	)
{
	assert( itsVarList != NULL );

	JIndex varIndex = origVarIndex;
	if (varIndex == 0 && !GetSelectedVariable(&varIndex))
		{
		return;
		}

	JXWindowDirector* dir = (GetWindow())->GetDirector();
	if (itsVarList->IsVariable(varIndex))
		{
		JXDialogDirector* dlog =
			new SCEditVarValueDialog(dir, itsVarList, varIndex);
		assert( dlog != NULL );
		dlog->BeginDialog();
		}
	else
		{
		JXDialogDirector* dlog =
			new SCEditFnDialog(dir, itsVarList, varIndex);
		assert( dlog != NULL );
		dlog->BeginDialog();
		}
}

/******************************************************************************
 GetSelectedRow (private)

 ******************************************************************************/

JBoolean
SCVarTable::GetSelectedRow
	(
	JIndex* rowIndex
	)
	const
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
		{
		*rowIndex = cell.y;
		return kJTrue;
		}
	else
		{
		*rowIndex = 0;
		return kJFalse;
		}
}

/******************************************************************************
 GetSelectedVariable (private)

 ******************************************************************************/

JBoolean
SCVarTable::GetSelectedVariable
	(
	JIndex* varIndex
	)
	const
{
	if (GetSelectedRow(varIndex))
		{
		*varIndex = RowToVarIndex(*varIndex);
		return kJTrue;
		}
	else
		{
		*varIndex = 0;
		return kJFalse;
		}
}

/******************************************************************************
 SelectRow (private)

 ******************************************************************************/

void
SCVarTable::SelectRow
	(
	const JIndex rowIndex
	)
{
	if (RowIndexValid(rowIndex))
		{
		JTableSelection& s = GetTableSelection();
		s.ClearSelection();
		s.SelectRow(rowIndex);

		TableScrollToCell(JPoint(1, rowIndex));
		}
}

/******************************************************************************
 MoveSelection (private)

 ******************************************************************************/

void
SCVarTable::MoveSelection
	(
	const JCoordinate delta
	)
{
	JIndex rowIndex;
	if (GetSelectedRow(&rowIndex))
		{
		SelectRow(rowIndex + delta);
		}
	else if (delta < 0)
		{
		SelectRow(1);
		}
	else if (delta > 0)
		{
		SelectRow(GetRowCount());
		}
}

/******************************************************************************
 Receive (protected)

	static const JCharacter* kVarNameChanged;
	static const JCharacter* kVarValueChanged;

 ******************************************************************************/

void
SCVarTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsVarList && message.Is(JVariableList::kVarInserted))
		{
		const JVariableList::VarInserted* info =
			dynamic_cast(const JVariableList::VarInserted*, &message);
		assert( info != NULL );
		SetVarList(itsVarList);
		}
	else if (sender == itsVarList && message.Is(JVariableList::kVarNameChanged))
		{
		const JVariableList::VarNameChanged* info =
			dynamic_cast(const JVariableList::VarNameChanged*, &message);
		assert( info != NULL );
		Refresh();
		}
	else if (sender == itsVarList && message.Is(JVariableList::kVarValueChanged))
		{
		const JVariableList::VarValueChanged* info =
			dynamic_cast(const JVariableList::VarValueChanged*, &message);
		assert( info != NULL );
		Refresh();
		}

	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SCVarTable::ReadSetup
	(
	istream&			input,
	const JFileVersion	vers
	)
{
	for (JIndex i=1; i<=kColCount; i++)
		{
		JCoordinate w;
		input >> w;
		SetColWidth(i, w);
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
SCVarTable::WriteSetup
	(
	ostream& output
	)
	const
{
	for (JIndex i=1; i<=kColCount; i++)
		{
		output << ' ' << GetColWidth(i);
		}

	output << ' ';
}
