/******************************************************************************
 GLUndoElementsBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoElementsBase.h>
#include <GXRaggedFloatTable.h>
#include <GRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementsBase::GLUndoElementsBase
	(
	GXRaggedFloatTable* 	table,
	const JPoint&			start,
	const JPoint&			end,
	const UndoType			type
	)
	:
	GLUndoBase(table),
	itsStartCell(start),
	itsEndCell(end),
	itsType(type)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLUndoElementsBase::~GLUndoElementsBase()
{
}
