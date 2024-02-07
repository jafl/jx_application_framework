/******************************************************************************
 JXIntegerTable.cpp

	Draws a table of numbers stored in a JIntegerTableData object
	and buffered in a JIntegerBufferTableData object.

	BASE CLASS = JXStyleTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXIntegerTable.h"
#include "JXIntegerInput.h"
#include <jx-af/jcore/JIntegerTableData.h>
#include <jx-af/jcore/JIntegerBufferTableData.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXIntegerTable::JXIntegerTable
	(
	JIntegerTableData*	data,
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
	assert( data != nullptr );

	itsIntegerData = data;

	itsIntegerBufferData = jnew JIntegerBufferTableData(data);
	SetTableData(itsIntegerBufferData);

	itsIntegerInputField = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXIntegerTable::~JXIntegerTable()
{
	jdelete itsIntegerBufferData;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXIntegerTable::TableDrawCell
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

		JFont font = GetFont();
		font.SetStyle(GetCellStyle(cell));
		p.SetFont(font);

		const JString& str = itsIntegerBufferData->GetString(cell);

		JRect r  = rect;
		r.left  += kHMarginWidth;
		r.right -= kHMarginWidth;
		p.String(r, str, JPainter::HAlign::kRight, JPainter::VAlign::kCenter);
	}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JXIntegerTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsIntegerInputField == nullptr );

	itsIntegerInputField =
		CreateIntegerTableInput(cell, this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsIntegerInputField != nullptr );

	JFont font = GetFont();
	font.SetStyle(GetCellStyle(cell));
	itsIntegerInputField->SetFont(font);

	itsIntegerInputField->SetValue(itsIntegerData->GetItem(cell));
	return itsIntegerInputField;
}

/******************************************************************************
 CreateIntegerTableInput (virtual protected)

	Derived class can override this to instantiate a derived class of
	JXIntegerInput.

 ******************************************************************************/

JXIntegerInput*
JXIntegerTable::CreateIntegerTableInput
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
	auto* obj = jnew JXIntegerInput(enclosure, hSizing, vSizing, x,y, w,h);
	return obj;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it,
	and delete the input field if successful.

	Returns true if the data is valid and the process succeeded.

 ******************************************************************************/

bool
JXIntegerTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsIntegerInputField != nullptr );

	if (itsIntegerInputField->InputValid())
	{
		JInteger value;
		const bool valid = itsIntegerInputField->GetValue(&value);
		assert( valid );
		itsIntegerData->SetItem(cell, value);
		return true;
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
JXIntegerTable::PrepareDeleteXInputField()
{
	itsIntegerInputField = nullptr;
}
