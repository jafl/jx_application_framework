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
	const JTextEditor::TextCount&	pasteCount
	)
	:
	JTEUndoTextBase(te)
{
	itsOrigSelStart = te->GetInsertionIndex();
	itsCount        = pasteCount;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoPaste::~JTEUndoPaste()
{
}

/******************************************************************************
 SetPasteCount (virtual)

 ******************************************************************************/

void
JTEUndoPaste::SetPasteCount
	(
	const JTextEditor::TextCount& count
	)
{
	itsCount = count;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoPaste::Undo()
{
	PrepareForUndo(itsOrigSelStart, itsCount);
	JTEUndoTextBase::Undo();
}
