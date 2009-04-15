/******************************************************************************
 JFloatTableData.cpp

	Class to store a matrix of JFloat.

	BASE CLASS = JValueTableData<JArray<JFloat>,JFloat>

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JFloatTableData.h>

// instantiate base class

#define JTemplateType JArray<JFloat>,JFloat
#include <JValueTableData.tmpls>
#undef JTemplateType

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
