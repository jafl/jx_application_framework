/******************************************************************************
 JTEUndoStyle.cpp

	Class to undo style change in a JTextEditor object.

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
	JTextEditor* te
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
	JTextEditor* te = GetTE();
	te->SetSelection(itsCharRange, itsByteRange);

	JTEUndoStyle* newUndo = jnew JTEUndoStyle(te);
	assert( newUndo != NULL );

	te->SetFont(itsCharRange.first, *itsOrigStyles, kJFalse);

	te->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor::SetAllFontNameAndSize().

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
