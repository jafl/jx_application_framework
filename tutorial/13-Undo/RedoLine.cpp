/******************************************************************************
 RedoLine.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <JXStdInc.h>
#include <RedoLine.h>
#include <UndoWidget.h>
#include <jAssert.h>

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
 Destructor

 ******************************************************************************/

RedoLine::~RedoLine()
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
