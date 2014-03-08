/******************************************************************************
 GLUndoBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <glStdInc.h>
#include <GLUndoBase.h>
#include <GXRaggedFloatTable.h>
#include <GRaggedFloatTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLUndoBase::GLUndoBase
	(
	GXRaggedFloatTable* 	table
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
