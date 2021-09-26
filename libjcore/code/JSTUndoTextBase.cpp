/******************************************************************************
 JSTUndoTextBase.cpp

	Base class to support undoing any operation on a JStyledText object
	that starts by deleting some text.

	BASE CLASS = JSTUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JSTUndoTextBase.h"
#include "jx-af/jcore/JSTUndoPaste.h"
#include "jx-af/jcore/jAssert.h"

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
	assert( itsOrigStyles != nullptr );

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

	Does NOT take a reference because that might be an instance variable,
	and we are deleted part way through.

 ******************************************************************************/

void
JSTUndoTextBase::UndoText
	(
	const JStyledText::TextRange range
	)
{
	JStyledText* text = GetText();

	auto* newUndo = jnew JSTUndoPaste(text, range);
	assert( newUndo != nullptr );

	const JStyledText::TextCount pasteCount = text->PrivatePaste(range, itsOrigText, itsOrigStyles);
	assert( pasteCount.charCount == itsOrigText.GetCharacterCount() );

	newUndo->SetCount(pasteCount);

	text->ReplaceUndo(this, newUndo);		// deletes us

	// NO instance variables past this point; we have been deleted

	JCharacterRange charRange;
	charRange.SetFirstAndCount(range.charRange.first, pasteCount.charCount);

	JUtf8ByteRange byteRange;
	byteRange.SetFirstAndCount(range.byteRange.first, pasteCount.byteCount);

	const JInteger charDelta = pasteCount.charCount - range.charRange.GetCount(),
				   byteDelta = pasteCount.byteCount - range.byteRange.GetCount();

	const JStyledText::TextRange r(charRange, byteRange);

	text->BroadcastTextChanged(r, charDelta, byteDelta, !range.IsEmpty());
	text->BroadcastUndoFinished(r);
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
