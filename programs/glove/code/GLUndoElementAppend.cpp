/******************************************************************************
 GLUndoElementAppend.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = GLUndoElementBase

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <glStdInc.h>
#include <GLUndoElementAppend.h>
#include <GLUndoElementCut.h>
#include <GXRaggedFloatTable.h>
#include <GRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementAppend::GLUndoElementAppend
	(
	GXRaggedFloatTable* 	table,
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
		new GLUndoElementCut(GetTable(), GetCell(), value);
	assert(undo != NULL);
	NewUndo(undo);
}
