/******************************************************************************
 JTEUndoDrop.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoDrop.h>
#include <JStyledTextBuffer.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoDrop::JTEUndoDrop
	(
	JStyledTextBuffer*					te,
	const JStyledTextBuffer::TextIndex&	origIndex,
	const JStyledTextBuffer::TextIndex&	newIndex,
	const JStyledTextBuffer::TextCount&	count
	)
	:
	JTEUndoBase(te)
{
	itsOrigCaretLoc = origIndex;
	itsNewSelStart  = newIndex;
	SetCount(count);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoDrop::~JTEUndoDrop()
{
}

/******************************************************************************
 SetCount

 ******************************************************************************/

void
JTEUndoDrop::SetCount
	(
	const JStyledTextBuffer::TextCount& count
	)
{
	itsNewSelEnd.charIndex = itsNewSelStart.charIndex + count.charCount - 1;
	itsNewSelEnd.byteIndex = itsNewSelStart.byteIndex + count.byteCount - 1;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoDrop::Undo()
{
	JStyledTextBuffer* te = GetSTB();
	te->SetSelection(JCharacterRange(itsNewSelStart.charIndex, itsNewSelEnd.charIndex),
					 JUtf8ByteRange(itsNewSelStart.byteIndex,  itsNewSelEnd.byteIndex));
	te->DropSelection(itsOrigCaretLoc, kJFalse);			// deletes us
}
