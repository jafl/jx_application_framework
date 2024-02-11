/******************************************************************************
 JIntegerBufferTableData.cpp

	Class to buffer a table of numbers as JStrings so drawing the table
	will be faster.

	BASE CLASS = JBufferTableData

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JIntegerBufferTableData.h"
#include "JIntegerTableData.h"
#include "JString.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JIntegerBufferTableData::JIntegerBufferTableData
	(
	const JIntegerTableData* intData
	)
	:
	JBufferTableData(intData),
	itsIntegerData(intData)
{
	UpdateRect(JRect(1, 1, GetRowCount()+1, GetColCount()+1));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JIntegerBufferTableData::~JIntegerBufferTableData()
{
}

/******************************************************************************
 UpdateCell (virtual protected)

 ******************************************************************************/

void
JIntegerBufferTableData::UpdateCell
	(
	const JPoint& cell
	)
{
	SetItem(cell, JString(itsIntegerData->GetItem(cell)));
}
