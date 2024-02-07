/******************************************************************************
 JIntegerTableData.cpp

	Class to store a matrix of JInteger.

	BASE CLASS = JValueTableData<JArray<JInteger>,JInteger>

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JIntegerTableData.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JIntegerTableData::JIntegerTableData
	(
	const JInteger defValue
	)
	:
	JValueTableData<JArray<JInteger>,JInteger>(defValue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JIntegerTableData::~JIntegerTableData()
{
}
