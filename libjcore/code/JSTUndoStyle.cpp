/******************************************************************************
 JSTUndoStyle.cpp

	Class to undo style change in a JStyledText object.

	BASE CLASS = JSTUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JSTUndoStyle.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JSTUndoStyle::JSTUndoStyle
	(
	JStyledText*					text,
	const JStyledText::TextRange&	range
	)
	:
	JSTUndoBase(text),
	itsRange(range)
{
	assert( !itsRange.IsEmpty() );

	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != nullptr );

	itsOrigStyles->AppendSlice(text->GetStyles(), itsRange.charRange);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTUndoStyle::~JSTUndoStyle()
{
	jdelete itsOrigStyles;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JSTUndoStyle::Undo()
{
	JStyledText* text = GetText();

	JSTUndoStyle* newUndo = jnew JSTUndoStyle(text, itsRange);
	assert( newUndo != nullptr );

	text->SetFont(itsRange, *itsOrigStyles);

	const JStyledText::TextRange r = itsRange;

	text->ReplaceUndo(this, newUndo);		// deletes us

	// NO instance variables past this point; we have been deleted

	text->BroadcastTextChanged(r, 0, 0, kJFalse);
	text->BroadcastUndoFinished(r);
}

/******************************************************************************
 SetFont (virtual)

	Called by JStyledText::SetAllFontNameAndSize().

 ******************************************************************************/

void
JSTUndoStyle::SetFont
	(
	const JString&	name,
	const JSize		size
	)
{
	JSTUndoBase::SetFont(itsOrigStyles, name, size);
}

/******************************************************************************
 SameRange

 ******************************************************************************/

JBoolean
JSTUndoStyle::SameRange
	(
	const JStyledText::TextRange& range
	)
	const
{
	return JI2B( range.charRange == itsRange.charRange &&
				 range.byteRange == itsRange.byteRange );
}
