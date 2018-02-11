/******************************************************************************
 JTEUndoTextBase.cpp

	Base class to support undoing any operation on a JTextEditor object
	that starts by deleting some text.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTextBase.h>
#include <JTEUndoPaste.h>
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
	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != NULL );

	te->GetSelection(&itsOrigBuffer, itsOrigStyles);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTextBase::~JTEUndoTextBase()
{
	jdelete itsOrigStyles;
}

/******************************************************************************
 PrepareForUndo (protected)

	Call this to select the text to remove before calling Undo().

 ******************************************************************************/

void
JTEUndoTextBase::PrepareForUndo
	(
	const JTextEditor::TextIndex& start,
	const JTextEditor::TextCount& count
	)
{
	JTextEditor* te = GetTE();
	if (count.charCount == 0)
		{
		te->SetCaretLocation(start);
		}
	else
		{
		te->SetSelection(
			JCharacterRange(start.charIndex, start.charIndex + count.charCount - 1),
			JUtf8ByteRange (start.byteIndex, start.byteIndex + count.byteCount - 1));
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

	JTEUndoPaste* newUndo = jnew JTEUndoPaste(te,
		JTextEditor::TextCount(itsOrigBuffer.GetCharacterCount(), itsOrigBuffer.GetByteCount()));
	assert( newUndo != NULL );

	const JTextEditor::TextIndex selStart   = te->GetInsertionIndex();
	const JTextEditor::TextCount pasteCount = te->PrivatePaste(itsOrigBuffer, itsOrigStyles);
	assert( pasteCount.charCount == itsOrigBuffer.GetCharacterCount() );

	if (!itsOrigBuffer.IsEmpty())
		{
		te->SetSelection(
			JCharacterRange(selStart.charIndex, selStart.charIndex + itsOrigBuffer.GetCharacterCount() - 1),
			JUtf8ByteRange (selStart.byteIndex, selStart.byteIndex + itsOrigBuffer.GetByteCount() - 1));
		}
	else
		{
		te->SetCaretLocation(selStart);
		}

	te->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 PrependToSave (protected)

	Prepend the character to the save buffer.

 ******************************************************************************/

void
JTEUndoTextBase::PrependToSave
	(
	const JUtf8Character&	c,
	const JIndex			charIndex
	)
{
	itsOrigBuffer.Prepend(c);
	itsOrigStyles->PrependElement(GetTE()->GetFont(charIndex));
}

/******************************************************************************
 AppendToSave (protected)

	Append the character at index to the save buffer.

 ******************************************************************************/

void
JTEUndoTextBase::AppendToSave
	(
	const JUtf8Character&	c,
	const JIndex			charIndex
	)
{
	itsOrigBuffer.Append(c);
	itsOrigStyles->AppendElement(GetTE()->GetFont(charIndex));
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoTextBase::SetFont
	(
	const JString&	name,
	const JSize		size
	)
{
	JTEUndoBase::SetFont(itsOrigStyles, name, size);
}
