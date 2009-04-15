/******************************************************************************
 JTEUndoTextBase.cpp

	Base class to support undoing any operation on a JTextEditor object
	that starts by deleting some text.

	BASE CLASS = JTEUndoBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoTextBase.h>
#include <JTEUndoPaste.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTextBase::JTEUndoTextBase
	(
	JTextEditor* te
	)
	:
	JTEUndoBase(te)
{
	itsOrigBuffer = new JString;
	assert( itsOrigBuffer != NULL );

	itsOrigStyles = new JRunArray<JTextEditor::Font>;
	assert( itsOrigStyles != NULL );

	te->GetSelection(itsOrigBuffer, itsOrigStyles);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTextBase::~JTEUndoTextBase()
{
	delete itsOrigBuffer;
	delete itsOrigStyles;
}

/******************************************************************************
 PrepareForUndo (protected)

	Call this to select the text to remove before calling Undo().

 ******************************************************************************/

void
JTEUndoTextBase::PrepareForUndo
	(
	const JIndex	start,
	const JSize		length
	)
{
	JTextEditor* te = GetTE();
	if (length == 0)
		{
		te->SetCaretLocation(start);
		}
	else
		{
		te->SetSelection(start, start + length - 1);
		}
}

/******************************************************************************
 Undo (virtual)

	Call this to put back the original text after selecting the new text.

 ******************************************************************************/

void
JTEUndoTextBase::Undo()
{
	JTextEditor* te = GetTE();

	JTEUndoPaste* newUndo = new JTEUndoPaste(te, itsOrigBuffer->GetLength());
	assert( newUndo != NULL );

	const JIndex selStart   = te->GetInsertionIndex();
	const JSize pasteLength = te->PrivatePaste(*itsOrigBuffer, itsOrigStyles);
	assert( pasteLength == itsOrigBuffer->GetLength() );

	if (!itsOrigBuffer->IsEmpty())
		{
		te->SetSelection(selStart, selStart + itsOrigBuffer->GetLength() - 1);
		}
	else
		{
		te->SetCaretLocation(selStart);
		}

	te->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 PrependToSave (protected)

	Prepend the character in front of index to the save buffer.

 ******************************************************************************/

void
JTEUndoTextBase::PrependToSave
	(
	const JIndex index
	)
{
	assert( index > 1 );

	JTextEditor* te = GetTE();

	const JString& text = te->GetText();
	const JCharacter c  = text.GetCharacter(index-1);

	const JTextEditor::Font f = te->GetFont(index-1);

	itsOrigBuffer->PrependCharacter(c);
	itsOrigStyles->PrependElement(f);
}

/******************************************************************************
 AppendToSave (protected)

	Append the character at index to the save buffer.

 ******************************************************************************/

void
JTEUndoTextBase::AppendToSave
	(
	const JIndex index
	)
{
	JTextEditor* te = GetTE();

	const JString& text = te->GetText();
	const JCharacter c  = text.GetCharacter(index);

	const JTextEditor::Font f = te->GetFont(index);

	itsOrigBuffer->AppendCharacter(c);
	itsOrigStyles->AppendElement(f);
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoTextBase::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	JTEUndoBase::SetFont(itsOrigStyles, name, size);
}
