/******************************************************************************
 UndoLine.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright � 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <UndoLine.h>
#include <UndoWidget.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoLine::UndoLine
	(
	UndoWidget*		uw
	)
	:
	JUndo(),
	itsWidget(uw)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoLine::~UndoLine()
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
