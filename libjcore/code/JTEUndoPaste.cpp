/******************************************************************************
 JTEUndoPaste.cpp

	Class to undo paste into a JTextEditor object.

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
	JTextEditor*					te,
	const JTextEditor::TextCount&	pasteLength
	)
	:
	JTEUndoTextBase(te)
{
	itsOrigSelStart = te->GetInsertionIndex();
	itsLength       = pasteLength;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoPaste::~JTEUndoPaste()
{
}

/******************************************************************************
 SetPasteLength (virtual)

 ******************************************************************************/

void
JTEUndoPaste::SetPasteLength
	(
	const JTextEditor::TextCount& length
	)
{
	itsLength = length;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoPaste::Undo()
{
	PrepareForUndo(itsOrigSelStart, itsLength);
	JTEUndoTextBase::Undo();
}
