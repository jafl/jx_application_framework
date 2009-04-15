/******************************************************************************
 JTEUndoPaste.cpp

	Class to undo paste into a JTextEditor object.

	BASE CLASS = JTEUndoTextBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoPaste.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoPaste::JTEUndoPaste
	(
	JTextEditor*	te,
	const JSize		pasteLength
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
	const JSize length
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
