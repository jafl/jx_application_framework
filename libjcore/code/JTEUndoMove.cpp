/******************************************************************************
 JTEUndoMove.cpp

	Class to undo moving text from one place to another, typically via
	drag-and-drop.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTEUndoMove.h>
#include <JStyledTextBuffer.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoMove::JTEUndoMove
	(
	JStyledTextBuffer*					buffer,
	const JStyledTextBuffer::TextIndex&	srcIndex,
	const JStyledTextBuffer::TextIndex&	destIndex,
	const JStyledTextBuffer::TextCount&	count
	)
	:
	JTEUndoBase(buffer),
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
	const JStyledTextBuffer::TextCount& count
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
	GetBuffer()->MoveText(		// deletes us
		JStyledTextBuffer::TextRange(itsDestIndex, itsCount), itsSrcIndex,
		kJFalse);
}
