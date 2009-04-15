/******************************************************************************
 JXFloatTable.cpp

	Draws a table of numbers stored in a JFloatTableData object
	and buffered in a JFloatBufferTableData object.

	BASE CLASS = JXStyleTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFloatTable.h>
#include <JXFloatInput.h>
#include <JFloatTableData.h>
#include <JFloatBufferTableData.h>
#include <JPainter.h>
#include <JString.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFloatTable::JXFloatTable
	(
	JFloatTableData*	data,
	const int			precision,
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

	itsFloatData = data;

	itsFloatBufferData = new JFloatBufferTableData(data, precision);
	assert( itsFloatBufferData != NULL );
	SetTableData(itsFloatBufferData);

	itsFloatInputField = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFloatTable::~JXFloatTable()
{
	delete itsFloatBufferData;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXFloatTable::TableDrawCell
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

		const JString& str = itsFloatBufferData->GetString(cell);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, str, JPainter::kHAlignRight, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JXFloatTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsFloatInputField == NULL );

	itsFloatInputField =
		CreateFloatTableInput(cell, this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsFloatInputField != NULL );

	JSize fontSize;
	const JString& fontName = GetFont(&fontSize);

	itsFloatInputField->SetFont(fontName, fontSize, GetCellStyle(cell));
	itsFloatInputField->SetValue(itsFloatData->GetElement(cell));
	return itsFloatInputField;
}

/******************************************************************************
 CreateFloatTableInput (virtual protected)

	Derived class can override this to instantiate a derived class of
	JXFloatInput.

 ******************************************************************************/

JXFloatInput*
JXFloatTable::CreateFloatTableInput
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
	JXFloatInput* obj = new JXFloatInput(enclosure, hSizing, vSizing, x,y, w,h);
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
JXFloatTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsFloatInputField != NULL );

	if (itsFloatInputField->InputValid())
		{
		JFloat value;
		const JBoolean valid = itsFloatInputField->GetValue(&value);
		assert( valid );
		itsFloatData->SetElement(cell, value);
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
JXFloatTable::PrepareDeleteXInputField()
{
	itsFloatInputField = NULL;
}
