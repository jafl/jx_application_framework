/******************************************************************************
 JXFloatTable.cpp

	Draws a table of numbers stored in a JFloatTableData object
	and buffered in a JFloatBufferTableData object.

	BASE CLASS = JXStyleTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXFloatTable.h"
#include "JXFloatInput.h"
#include <jx-af/jcore/JFloatTableData.h>
#include <jx-af/jcore/JFloatBufferTableData.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/jAssert.h>

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
	assert( data != nullptr );

	itsFloatData = data;

	itsFloatBufferData = jnew JFloatBufferTableData(data, precision);
	SetTableData(itsFloatBufferData);

	itsFloatInputField = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFloatTable::~JXFloatTable()
{
	jdelete itsFloatBufferData;
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

		JFont font = GetFont();
		font.SetStyle(GetCellStyle(cell));
		p.SetFont(font);

		const JString& str = itsFloatBufferData->GetString(cell);

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
JXFloatTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsFloatInputField == nullptr );

	itsFloatInputField =
		CreateFloatTableInput(cell, this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsFloatInputField != nullptr );

	JFont font = GetFont();
	font.SetStyle(GetCellStyle(cell));
	itsFloatInputField->SetFont(font);

	itsFloatInputField->SetValue(itsFloatData->GetItem(cell));
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
	auto* obj = jnew JXFloatInput(enclosure, hSizing, vSizing, x,y, w,h);
	return obj;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it,
	and delete the input field if successful.

	Returns true if the data is valid and the process succeeded.

 ******************************************************************************/

bool
JXFloatTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsFloatInputField != nullptr );

	if (itsFloatInputField->InputValid())
	{
		JFloat value;
		const bool valid = itsFloatInputField->GetValue(&value);
		assert( valid );
		itsFloatData->SetItem(cell, value);
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
JXFloatTable::PrepareDeleteXInputField()
{
	itsFloatInputField = nullptr;
}
