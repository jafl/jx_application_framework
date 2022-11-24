/******************************************************************************
 UndoLine.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "UndoLine.h"
#include "UndoWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoLine::UndoLine
	(
	UndoWidget* uw
	)
	:
	JUndo(),
	itsWidget(uw)
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
UndoLine::Undo()
{
	itsWidget->RemoveLastLine();
}
