/******************************************************************************
 GLUndoElementBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoElementBase.h>
#include <GXRaggedFloatTable.h>
#include <GRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementBase::GLUndoElementBase
	(
	GXRaggedFloatTable* 	table,
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
