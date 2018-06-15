/******************************************************************************
 JFloatTableData.cpp

	Class to store a matrix of JFloat.

	BASE CLASS = JValueTableData<JArray<JFloat>,JFloat>

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JFloatTableData.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JFloatTableData::JFloatTableData
	(
	const JFloat defValue
	)
	:
	JValueTableData<JArray<JFloat>,JFloat>(defValue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFloatTableData::~JFloatTableData()
{
}
