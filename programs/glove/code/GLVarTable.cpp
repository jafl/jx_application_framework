/******************************************************************************
 GLVarTable.cpp

	BASE CLASS = JXEditTable

	Copyright © 2000 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GLVarTable.h"
#include "GVarList.h"
#include <JXExprInput.h>
#include <jXConstants.h>
#include <JFunction.h>
#include <JFontManager.h>
#include <JPainter.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

const JIndex kUserParmsOffset	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLVarTable::GLVarTable
	(
	GVarList*			varList,
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
		GetFontManager()->GetLineHeight(JGetDefaultFontName(), kJDefaultFontSize, JFontStyle());
	SetDefaultRowHeight(rowHeight);

	itsVarList   = varList;
	itsTextInput = NULL;

	itsOrigText = new JString;
	assert( itsOrigText != NULL );

	AppendCols(1);

	const JSize count = itsVarList->GetVariableCount() - kUserParmsOffset;
	AppendRows(count);

	FitToEnclosure();	// make sure SetColWidth() won't fail
	ListenTo(this);		// adjust fn col width

	SetColWidth(kNameColumn, GetApertureWidth());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLVarTable::~GLVarTable()
{
	delete itsOrigText;
}

/******************************************************************************
 NewConstant

 ******************************************************************************/

void
GLVarTable::NewConstant()
{
	if (!EndEditing())
		{
		return;
		}
	JIndex parmIndex	= GetRowCount() + 1;
	JString parm;
	JIndex index;
	do
		{
		parm	= "a" + JString(parmIndex, JString::kBase10);
		parmIndex ++;
		}
	while (itsVarList->ParseVariableName(parm, parm.GetLength(), &index));

	if (itsVarList->AddVariable(parm, 0))
		{
		AppendRows(1);
		BeginEditing(JPoint(1,GetRowCount()));
		}
}

/******************************************************************************
 RemoveSelectedConstant

 ******************************************************************************/

void
GLVarTable::RemoveSelectedConstant()
{
	JPoint cell;
	if (GetEditedCell(&cell))
		{
		CancelEditing();
		RemoveRow(cell.y);
		itsVarList->RemoveVariable(cell.y + kUserParmsOffset);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GLVarTable::HandleMouseDown
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
GLVarTable::TableDrawCell
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

	const JIndex varIndex = cell.y + kUserParmsOffset;
	p.String(r, itsVarList->GetVariableName(varIndex),
				 JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GLVarTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == NULL );

	itsTextInput = new JXExprInput(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsTextInput != NULL );

	const JIndex varIndex = cell.y + kUserParmsOffset;
	itsTextInput->SetVarName(itsVarList->GetVariableName(varIndex));

	*itsOrigText = itsTextInput->GetVarName();

	itsTextInput->SetIsRequired();
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GLVarTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != NULL );

	const JString s = itsTextInput->GetVarName();
	if (s == *itsOrigText)
		{
		return kJTrue;
		}
	else if (itsTextInput->InputValid())
		{
		const JIndex varIndex = cell.y + kUserParmsOffset;
		JBoolean ok = itsVarList->SetVariableName(varIndex, s);
		return ok;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GLVarTable::PrepareDeleteXInputField()
{
	itsTextInput = NULL;
}

/******************************************************************************
 Receive (virtual protected)

	We don't have to listen for changes to itsVarList because we are
	the only ones who change it.

 ******************************************************************************/

void
GLVarTable::Receive
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
GLVarTable::ApertureResized
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
GLVarTable::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();
	SetColWidth(kNameColumn, apWidth);
}
