/******************************************************************************
 JTEUndoStyle.cpp

	Class to undo style change in a JTextEditor object.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1996 by John Lindal. All rights reserved.

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
	itsStartIndex = te->GetInsertionIndex();

	itsOrigStyles = jnew JRunArray<JFont>;
	assert( itsOrigStyles != NULL );

	JString selText;
	const JBoolean hasSelection = te->GetSelection(&selText, itsOrigStyles);
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
	te->SetSelection(itsStartIndex, itsStartIndex + itsOrigStyles->GetElementCount() - 1);

	JTEUndoStyle* newUndo = jnew JTEUndoStyle(te);
	assert( newUndo != NULL );

	te->SetFont(itsStartIndex, *itsOrigStyles, kJFalse);

	te->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoStyle::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	JTEUndoBase::SetFont(itsOrigStyles, name, size);
}
