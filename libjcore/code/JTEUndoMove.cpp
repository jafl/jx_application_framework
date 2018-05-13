/******************************************************************************
 JTEUndoMove.cpp

	Class to undo moving text from one place to another, typically via
	drag-and-drop.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTEUndoMove.h>
#include <JStyledText.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoMove::JTEUndoMove
	(
	JStyledText*					text,
	const JStyledText::TextIndex&	srcIndex,
	const JStyledText::TextIndex&	destIndex,
	const JStyledText::TextCount&	count
	)
	:
	JTEUndoBase(text),
	itsSrcIndex(srcIndex),
	itsDestIndex(destIndex),
	itsCount(count)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoMove::~JTEUndoMove()
{
}

/******************************************************************************
 SetCount (virtual)

 ******************************************************************************/

void
JTEUndoMove::SetCount
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
JTEUndoMove::Undo()
{
	JStyledText::TextRange newRange;
	const JBoolean ok = GetText()->MoveText(		// deletes us
		JStyledText::TextRange(itsDestIndex, itsCount), itsSrcIndex,
		kJFalse, &newRange);
	assert( ok );
}
