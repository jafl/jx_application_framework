/******************************************************************************
 JTEUndoStyle.cpp

	Class to undo style change in a JStyledText object.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTEUndoStyle.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoStyle::JTEUndoStyle
	(
	JStyledText*					text,
	const JStyledText::TextRange&	range
	)
	:
	JTEUndoBase(text),
	itsRange(range)
{
	assert( !itsRange.IsEmpty() );

	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != NULL );

	itsOrigStyles->AppendSlice(text->GetStyles(), itsRange.charRange);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoStyle::~JTEUndoStyle()
{
	jdelete itsOrigStyles;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoStyle::Undo()
{
	JStyledText* text = GetText();

	JTEUndoStyle* newUndo = jnew JTEUndoStyle(text, itsRange);
	assert( newUndo != NULL );

	text->SetFont(itsRange, *itsOrigStyles);

	text->ReplaceUndo(this, newUndo);		// deletes us

	text->BroadcastTextChanged(itsRange, kJFalse);
}

/******************************************************************************
 SetFont (virtual)

	Called by JStyledText::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoStyle::SetFont
	(
	const JString&	name,
	const JSize		size
	)
{
	JTEUndoBase::SetFont(itsOrigStyles, name, size);
}

/******************************************************************************
 SameRange

 ******************************************************************************/

JBoolean
JTEUndoStyle::SameRange
	(
	const JStyledText::TextRange& range
	)
	const
{
	return JI2B( range.charRange == itsRange.charRange &&
				 range.byteRange == itsRange.byteRange );
}
