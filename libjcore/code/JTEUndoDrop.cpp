/******************************************************************************
 JTEUndoDrop.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoDrop.h>
#include <JTextEditor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoDrop::JTEUndoDrop
	(
	JTextEditor*					te,
	const JTextEditor::TextIndex&	origIndex,
	const JTextEditor::TextIndex&	newIndex,
	const JTextEditor::TextCount&	length
	)
	:
	JTEUndoBase(te)
{
	itsOrigCaretLoc = origIndex;
	itsNewSelStart  = newIndex;
	SetPasteLength(length);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoDrop::~JTEUndoDrop()
{
}

/******************************************************************************
 SetPasteLength (virtual)

 ******************************************************************************/

void
JTEUndoDrop::SetPasteLength
	(
	const JTextEditor::TextCount& length
	)
{
	itsNewSelEnd.charIndex = itsNewSelStart.charIndex + length.charCount - 1;
	itsNewSelEnd.byteIndex = itsNewSelStart.byteIndex + length.byteCount - 1;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoDrop::Undo()
{
	JTextEditor* te = GetTE();
	te->SetSelection(JCharacterRange(itsNewSelStart.charIndex, itsNewSelEnd.charIndex),
					 JUtf8ByteRange(itsNewSelStart.byteIndex,  itsNewSelEnd.byteIndex));
	te->DropSelection(itsOrigCaretLoc, kJFalse);			// deletes us
}
