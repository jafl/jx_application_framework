/******************************************************************************
 GLUndoElementCut.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = GLUndoElementBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoElementCut.h>
#include <GLUndoElementAppend.h>
#include <GLRaggedFloatTable.h>
#include <GLRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementCut::GLUndoElementCut
	(
	GLRaggedFloatTable* 	table,
	const JPoint& 			cell,
	const JFloat			value
	)
	:
	GLUndoElementBase(table, cell),
	itsValue(value)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLUndoElementCut::~GLUndoElementCut()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
GLUndoElementCut::Undo()
{
	// change value to old value
	GetData()->InsertElement(GetCell(), itsValue);

	// create undo object to change it back
	GLUndoElementAppend* undo = 
		jnew GLUndoElementAppend(GetTable(), GetCell());
	assert(undo != nullptr);
	NewUndo(undo);
}
