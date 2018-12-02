/******************************************************************************
 GLUndoElementAppend.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = GLUndoElementBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoElementAppend.h>
#include <GLUndoElementCut.h>
#include <GLRaggedFloatTable.h>
#include <GLRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementAppend::GLUndoElementAppend
	(
	GLRaggedFloatTable* 	table,
	const JPoint& 			cell
	)
	:
	GLUndoElementBase(table, cell)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLUndoElementAppend::~GLUndoElementAppend()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
GLUndoElementAppend::Undo()
{
	// get current value
	JFloat value = 0;
	GetData()->GetElement(GetCell(), &value);

	// change value to old value
	GetData()->RemoveElement(GetCell());

	// create undo object to change it back
	GLUndoElementCut* undo = 
		jnew GLUndoElementCut(GetTable(), GetCell(), value);
	assert(undo != nullptr);
	NewUndo(undo);
}
