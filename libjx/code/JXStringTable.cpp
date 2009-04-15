/******************************************************************************
 JXStringTable.cpp

	Draws a table of strings stored in a JStringTableData object.

	BASE CLASS = JXStyleTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStringTable.h>
#include <JXInputField.h>
#include <JStringTableData.h>
#include <JPainter.h>
#include <JString.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStringTable::JXStringTable
	(
	JStringTableData*	data,
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
	JXStyleTable(scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	assert( data != NULL );

	itsStringData = data;
	SetTableData(data);

	itsStringInputField = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStringTable::~JXStringTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXStringTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (!GetEditedCell(&editCell) || cell != editCell)
		{
		HilightIfSelected(p, cell, rect);

		JSize fontSize;
		const JString& fontName = GetFont(&fontSize);
		p.SetFont(fontName, fontSize, GetCellStyle(cell));

		const JString& str = itsStringData->GetString(cell);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JXStringTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsStringInputField == NULL );

	itsStringInputField =
		CreateStringTableInput(cell, this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsStringInputField != NULL );

	JSize fontSize;
	const JString& fontName = GetFont(&fontSize);

	itsStringInputField->SetFont(fontName, fontSize, GetCellStyle(cell));
	itsStringInputField->SetText(itsStringData->GetString(cell));
	return itsStringInputField;
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

	Derived class can override this to instantiate a derived class of
	JXInputField.

 ******************************************************************************/

JXInputField*
JXStringTable::CreateStringTableInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JXInputField* obj = new JXInputField(enclosure, hSizing, vSizing, x,y, w,h);
	assert( obj != NULL );
	return obj;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it,
	and delete the input field if successful.

	Returns kJTrue if the data is valid and the process succeeded.

 ******************************************************************************/

JBoolean
JXStringTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsStringInputField != NULL );

	if (itsStringInputField->InputValid())
		{
		itsStringData->SetString(cell, itsStringInputField->GetText());
		return kJTrue;
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
JXStringTable::PrepareDeleteXInputField()
{
	itsStringInputField = NULL;
}
