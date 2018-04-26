/******************************************************************************
 JTEUndoStyle.cpp

	Class to undo style change in a JStyledTextBuffer object.

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
	JStyledTextBuffer*		buffer,
	const JCharacterRange&	range
	)
	:
	JTEUndoBase(buffer),
	itsRange(range)
{
	assert( !itsRange.IsEmpty() );

	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != NULL );

	itsOrigStyles->AppendSlice(buffer->GetStyles(), itsRange);
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
	JStyledTextBuffer* buffer = GetBuffer();

	JTEUndoStyle* newUndo = jnew JTEUndoStyle(buffer, itsRange);
	assert( newUndo != NULL );

	buffer->SetFont(itsRange.first, *itsOrigStyles, kJFalse);

	buffer->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 SetFont (virtual)

	Called by JStyledTextBuffer::SetAllFontNameAndSize().

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
	const JCharacterRange& range
	)
	const
{
	return JI2B( range == itsRange );
}
