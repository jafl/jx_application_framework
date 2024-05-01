/******************************************************************************
 JSTUndoTextBase.cpp

	Base class to support undoing any operation on a JStyledText object
	that starts by deleting some text.

	BASE CLASS = JSTUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JSTUndoTextBase.h"
#include "JSTUndoPaste.h"
#include "JUndoRedoChain.h"
#include "jAssert.h"

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
	const JStyledText::TextRange range	// copy, because we will be deleted half way through, and it probably comes from an instance variable
	)
{
	JStyledText* text = GetText();

	auto* newUndo = jnew JSTUndoPaste(text, range);

	const JStyledText::TextCount pasteCount = text->PrivatePaste(range, itsOrigText, itsOrigStyles);
	assert( pasteCount.charCount == itsOrigText.GetCharacterCount() );

	newUndo->SetCount(pasteCount);

	text->GetUndoRedoChain()->ReplaceUndo(this, newUndo);		// deletes us

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

	TODO: optimize callers to avoid random access into JST style run array.

 ******************************************************************************/

void
JSTUndoTextBase::PrependToSave
	(
	const JUtf8Character&	c,
	const JIndex			charIndex
	)
{
	itsOrigText.Prepend(c);
	itsOrigStyles->PrependItem(GetText()->GetFont(charIndex));
}

/******************************************************************************
 AppendToSave (protected)

	Append the character at index to the save text.

	TODO: optimize callers to avoid random access into JST style run array.

 ******************************************************************************/

void
JSTUndoTextBase::AppendToSave
	(
	const JUtf8Character&	c,
	const JIndex			charIndex
	)
{
	itsOrigText.Append(c);
	itsOrigStyles->AppendItem(GetText()->GetFont(charIndex));
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
