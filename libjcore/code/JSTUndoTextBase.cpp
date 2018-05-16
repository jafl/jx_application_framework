/******************************************************************************
 JSTUndoTextBase.cpp

	Base class to support undoing any operation on a JStyledText object
	that starts by deleting some text.

	BASE CLASS = JSTUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JSTUndoTextBase.h>
#include <JSTUndoPaste.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Saves a snapshot of the specified range, to allow undo.

 ******************************************************************************/

JSTUndoTextBase::JSTUndoTextBase
	(
	JStyledText*					text,
	const JStyledText::TextRange&	range
	)
	:
	JSTUndoBase(text)
{
	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != NULL );

	text->Copy(range, &itsOrigText, itsOrigStyles);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTUndoTextBase::~JSTUndoTextBase()
{
	jdelete itsOrigStyles;
}

/******************************************************************************
 UndoText (protected)

	Call this to put back the original text.

 ******************************************************************************/

void
JSTUndoTextBase::UndoText
	(
	const JStyledText::TextRange& range
	)
{
	JStyledText* text = GetText();

	JSTUndoPaste* newUndo = jnew JSTUndoPaste(text, range);
	assert( newUndo != NULL );

	const JStyledText::TextCount pasteCount = text->PrivatePaste(range, itsOrigText, itsOrigStyles);
	assert( pasteCount.charCount == itsOrigText.GetCharacterCount() );

	newUndo->SetCount(pasteCount);

	text->ReplaceUndo(this, newUndo);		// deletes us

	JCharacterRange charRange;
	charRange.SetFirstAndCount(range.charRange.first, pasteCount.charCount);

	JUtf8ByteRange byteRange;
	byteRange.SetFirstAndCount(range.byteRange.first, pasteCount.byteCount);

	text->BroadcastTextChanged(
		JStyledText::TextRange(charRange, byteRange), !range.IsEmpty());
}

/******************************************************************************
 PrependToSave (protected)

	Prepend the character to the save text.

 ******************************************************************************/

void
JSTUndoTextBase::PrependToSave
	(
	const JUtf8Character&	c,
	const JIndex			charIndex
	)
{
	itsOrigText.Prepend(c);
	itsOrigStyles->PrependElement(GetText()->GetFont(charIndex));
}

/******************************************************************************
 AppendToSave (protected)

	Append the character at index to the save text.

 ******************************************************************************/

void
JSTUndoTextBase::AppendToSave
	(
	const JUtf8Character&	c,
	const JIndex			charIndex
	)
{
	itsOrigText.Append(c);
	itsOrigStyles->AppendElement(GetText()->GetFont(charIndex));
}

/******************************************************************************
 SetFont (virtual)

	Called by JStyledText::SetAllFontNameAndSize().

 ******************************************************************************/

void
JSTUndoTextBase::SetFont
	(
	const JString&	name,
	const JSize		size
	)
{
	JSTUndoBase::SetFont(itsOrigStyles, name, size);
}
