/******************************************************************************
 JFloatBufferTableData.cpp

	Class to buffer a table of numbers as JStrings so drawing the table
	will be faster.

	BASE CLASS = JBufferTableData

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JFloatBufferTableData.h"
#include "JFloatTableData.h"
#include "JString.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JFloatBufferTableData::JFloatBufferTableData
	(
	const JFloatTableData*	floatData,
	const int				precision
	)
	:
	JBufferTableData(floatData),
	itsFloatData(floatData),
	itsPrecision(precision)
{
	UpdateRect(JRect(1, 1, GetRowCount()+1, GetColCount()+1));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFloatBufferTableData::~JFloatBufferTableData()
{
}

/******************************************************************************
 UpdateCell (virtual protected)

 ******************************************************************************/

void
JFloatBufferTableData::UpdateCell
	(
	const JPoint& cell
	)
{
	SetItem(cell, JString(itsFloatData->GetItem(cell), itsPrecision));
}
