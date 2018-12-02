/******************************************************************************
 GLUndoElementChange.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoElementChange.h>
#include <GLRaggedFloatTable.h>
#include <GLRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoElementChange::GLUndoElementChange
	(
	GLRaggedFloatTable* 	table,
	const JPoint& 			cell,
	const JFloat 			value
	)
	:
	GLUndoElementBase(table, cell),
	itsValue(value)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLUndoElementChange::~GLUndoElementChange()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
GLUndoElementChange::Undo()
{
	// get current value
	JFloat value = 0;
	GetData()->GetElement(GetCell(), &value);

	// change value to old value
	GetData()->SetElement(GetCell(), itsValue);

	// create undo object to change it back
	GLUndoElementChange* undo = 
		jnew GLUndoElementChange(GetTable(), GetCell(), value);
	assert(undo != nullptr);
	NewUndo(undo);
}
