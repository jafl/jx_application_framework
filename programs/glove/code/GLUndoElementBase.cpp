/******************************************************************************
 GLUndoElementBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoElementBase.h>
#include <GLRaggedFloatTable.h>
#include <GLRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementBase::GLUndoElementBase
	(
	GLRaggedFloatTable* 	table,
	const JPoint&			cell
	)
	:
	GLUndoBase(table),
	itsCell(cell)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLUndoElementBase::~GLUndoElementBase()
{
}
