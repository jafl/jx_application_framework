/******************************************************************************
 JStringTableData.cpp

	Class to store a matrix of JStrings.

	BASE CLASS = JObjTableData<JString>

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JStringTableData.h>

// instantiate base class

#include <JString.h>
#define JTemplateType JString
#include <JObjTableData.tmpls>

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

/******************************************************************************
 SetString

 ******************************************************************************/

void
JStringTableData::SetString
	(
	const JIndex		row,
	const JIndex		col,
	const JCharacter*	str
	)
{
	JString s = str;
	SetElement(row,col, s);
}

void
JStringTableData::SetString
	(
	const JPoint&		cell,
	const JCharacter*	str
	)
{
	JString s = str;
	SetElement(cell, s);
}
