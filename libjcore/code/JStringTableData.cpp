/******************************************************************************
 JStringTableData.cpp

	Class to store a matrix of JStrings.

	BASE CLASS = JObjTableData<JString>

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include <JStringTableData.h>
#include <JString.h>

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
