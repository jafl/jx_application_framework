/******************************************************************************
 JTEUndoTextBase.cpp

	Base class to support undoing any operation on a JTextEditor object
	that starts by deleting some text.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTextBase.h>
#include <JTEUndoPaste.h>
#include <JStringIterator.h>
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

	JTEUndoPaste* newUndo = jnew JTEUndoPaste(te, itsOrigBuffer.GetCharacterCount());
	assert( newUndo != NULL );

	const JIndex selStart   = te->GetInsertionIndex();
	const JSize pasteLength = te->PrivatePaste(itsOrigBuffer, itsOrigStyles);
	assert( pasteLength == itsOrigBuffer.GetCharacterCount() );

	if (!itsOrigBuffer.IsEmpty())
		{
		te->SetSelection(selStart, selStart + itsOrigBuffer.GetCharacterCount() - 1);
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

	JStringIterator iter(te->GetText(), kJIteratorStartBefore, index);
	JUtf8Character c;
	const JBoolean ok = iter.Prev(&c);
	assert( ok );

	const JFont f = te->GetFont(index-1);

	itsOrigBuffer.Prepend(c);
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

	JStringIterator iter(te->GetText(), kJIteratorStartBefore, index);
	JUtf8Character c;
	const JBoolean ok = iter.Next(&c);
	assert( ok );

	const JFont f = te->GetFont(index);

	itsOrigBuffer.Append(c);
	itsOrigStyles->AppendElement(f);
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
