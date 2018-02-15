/******************************************************************************
 JTEUndoStyle.cpp

	Class to undo style change in a JStyledTextBuffer object.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoStyle.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoStyle::JTEUndoStyle
	(
	JStyledTextBuffer* te
	)
	:
	JTEUndoBase(te)
{
	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != NULL );

	JBoolean hasSelection = te->GetSelection(&itsCharRange, &itsByteRange);
	assert( hasSelection );

	JString selText;
	hasSelection = te->GetSelection(&selText, itsOrigStyles);
	assert( hasSelection );
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
	JStyledTextBuffer* te = GetSTB();
	te->SetSelection(itsCharRange, itsByteRange);

	JTEUndoStyle* newUndo = jnew JTEUndoStyle(te);
	assert( newUndo != NULL );

	te->SetFont(JStyledTextBuffer::TextIndex(itsCharRange.first, itsByteRange.first),
				*itsOrigStyles, kJFalse);

	te->ReplaceUndo(this, newUndo);		// deletes us
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
