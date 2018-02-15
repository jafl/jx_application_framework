/******************************************************************************
 JTEUndoPaste.cpp

	Class to undo paste into a JStyledTextBuffer object.

	BASE CLASS = JTEUndoTextBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoPaste.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoPaste::JTEUndoPaste
	(
	JStyledTextBuffer*					te,
	const JStyledTextBuffer::TextIndex&	start,
	const JStyledTextBuffer::TextCount&	pasteCount
	)
	:
	JTEUndoTextBase(te),
	itsStart(start),
	itsCount(pasteCount)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoPaste::~JTEUndoPaste()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoPaste::Undo()
{
	PrepareForUndo(itsStart, itsCount);
	JTEUndoTextBase::Undo();
}
