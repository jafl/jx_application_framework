/******************************************************************************
 JTEUndoTextBase.cpp

	Base class to support undoing any operation on a JStyledTextBuffer object
	that starts by deleting some text.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTextBase.h>
#include <JTEUndoPaste.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Saves a snapshot of the specified range, to allow undo.

 ******************************************************************************/

JTEUndoTextBase::JTEUndoTextBase
	(
	JStyledTextBuffer*					buffer,
	const JStyledTextBuffer::TextRange&	range
	)
	:
	JTEUndoBase(buffer)
{
	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != NULL );

	buffer->Copy(range, &itsOrigText, itsOrigStyles);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTextBase::~JTEUndoTextBase()
{
	jdelete itsOrigStyles;
}

/******************************************************************************
 UndoText (protected)

	Call this to put back the original text.

 ******************************************************************************/

void
JTEUndoTextBase::UndoText
	(
	const JStyledTextBuffer::TextRange& range
	)
{
	JStyledTextBuffer* buffer = GetBuffer();

	JTEUndoPaste* newUndo = jnew JTEUndoPaste(buffer, range);
	assert( newUndo != NULL );

	const JStyledTextBuffer::TextCount pasteCount = buffer->PrivatePaste(range, itsOrigText, itsOrigStyles);
	assert( pasteCount.charCount == itsOrigText.GetCharacterCount() );

	newUndo->SetCount(pasteCount);

	buffer->ReplaceUndo(this, newUndo);		// deletes us

	JCharacterRange charRange;
	charRange.SetFirstAndCount(range.charRange.first, pasteCount.charCount);

	JUtf8ByteRange byteRange;
	byteRange.SetFirstAndCount(range.byteRange.first, pasteCount.byteCount);

	buffer->BroadcastForUndo(JStyledTextBuffer::TextRange(charRange, byteRange));
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
	itsOrigText.Prepend(c);
	itsOrigStyles->PrependElement(GetBuffer()->GetFont(charIndex));
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
	itsOrigText.Append(c);
	itsOrigStyles->AppendElement(GetBuffer()->GetFont(charIndex));
}

/******************************************************************************
 SetFont (virtual)

	Called by JStyledTextBuffer::SetAllFontNameAndSize().

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
