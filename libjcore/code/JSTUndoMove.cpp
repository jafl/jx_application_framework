/******************************************************************************
 JSTUndoMove.cpp

	Class to undo moving text from one place to another, typically via
	drag-and-drop.

	BASE CLASS = JSTUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JSTUndoMove.h"
#include "JStyledText.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JSTUndoMove::JSTUndoMove
	(
	JStyledText*					text,
	const JStyledText::TextIndex&	srcIndex,
	const JStyledText::TextIndex&	destIndex,
	const JStyledText::TextCount&	count
	)
	:
	JSTUndoBase(text),
	itsSrcIndex(srcIndex),
	itsDestIndex(destIndex),
	itsCount(count)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTUndoMove::~JSTUndoMove()
{
}

/******************************************************************************
 SetCount (virtual)

 ******************************************************************************/

void
JSTUndoMove::SetCount
	(
	const JStyledText::TextCount& count
	)
{
	itsCount = count;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JSTUndoMove::Undo()
{
	JStyledText::TextRange newRange;
	const JBoolean ok = GetText()->MoveText(		// deletes us
		JStyledText::TextRange(itsDestIndex, itsCount), itsSrcIndex,
		kJFalse, &newRange);
	assert( ok );
}
