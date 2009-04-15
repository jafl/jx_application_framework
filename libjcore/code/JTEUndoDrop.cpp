/******************************************************************************
 JTEUndoDrop.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoDrop.h>
#include <JTextEditor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoDrop::JTEUndoDrop
	(
	JTextEditor*	te,
	const JIndex	origIndex,
	const JIndex	newIndex,
	const JSize		length
	)
	:
	JTEUndoBase(te)
{
	itsOrigCaretLoc = origIndex;
	itsNewSelStart  = newIndex;
	itsNewSelEnd    = itsNewSelStart + length - 1;
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
	const JSize length
	)
{
	itsNewSelEnd = itsNewSelStart + length - 1;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoDrop::Undo()
{
	JTextEditor* te = GetTE();
	te->SetSelection(itsNewSelStart, itsNewSelEnd);
	te->DropSelection(itsOrigCaretLoc, kJFalse);			// deletes us
}
