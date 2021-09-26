/******************************************************************************
 JStringTableData.cpp

	Class to store a matrix of JStrings.

	BASE CLASS = JObjTableData<JString>

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JStringTableData.h"
#include "jx-af/jcore/JString.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JStringTableData::JStringTableData()
	:
	JObjTableData<JString>()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JStringTableData::JStringTableData
	(
	const JStringTableData& source
	)
	:
	JObjTableData<JString>(source)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStringTableData::~JStringTableData()
{
}
