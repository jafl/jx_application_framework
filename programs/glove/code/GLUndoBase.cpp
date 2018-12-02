/******************************************************************************
 GLUndoBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <GLUndoBase.h>
#include <GLRaggedFloatTable.h>
#include <GLRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoBase::GLUndoBase
	(
	GLRaggedFloatTable* 	table
	)
	:
	JUndo(),
	itsTable(table)
{
	itsData = table->itsFloatData;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLUndoBase::~GLUndoBase()
{
}

/******************************************************************************
 NewUndo (protected)

 ******************************************************************************/

void
GLUndoBase::NewUndo
	(
	JUndo* undo
	)
{
	itsTable->NewUndo(undo);
}
