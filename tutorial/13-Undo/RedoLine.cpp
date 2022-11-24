/******************************************************************************
 RedoLine.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "RedoLine.h"
#include "UndoWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

RedoLine::RedoLine
	(
	UndoWidget*		uw,
	const JPoint& 	start,
	const JPoint& 	end
	)
	:
	JUndo(),
	itsWidget(uw),
	itsStartPt(start),
	itsEndPt(end)
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
RedoLine::Undo()
{
	itsWidget->AddLine(itsStartPt, itsEndPt);
}
