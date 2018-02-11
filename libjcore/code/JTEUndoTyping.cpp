/******************************************************************************
 JTEUndoTyping.cpp

	Class to undo typing into a JTextEditor object.

	BASE CLASS = JTEUndoTextBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTyping.h>
#include <JStringIterator.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTyping::JTEUndoTyping
	(
	JTextEditor* te
	)
	:
	JTEUndoTextBase(te)
{
	itsOrigStartIndex = te->GetInsertionIndex();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTyping::~JTEUndoTyping()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoTyping::Undo()
{
	PrepareForUndo(itsOrigStartIndex, itsCount);
	JTEUndoTextBase::Undo();
}

/******************************************************************************
 HandleCharacters

 ******************************************************************************/

void
JTEUndoTyping::HandleCharacters
	(
	const JTextEditor::TextCount& count
	)
{
	assert( IsActive() );

	itsCount += count;
}

/******************************************************************************
 HandleDelete

	Call this -before- deleting the characters.

	Iterator is modified.

 ******************************************************************************/

void
JTEUndoTyping::HandleDelete
	(
	JStringIterator*	iter,
	const JSize			charCount
	)
{
	assert( IsActive() );

	JUtf8Character c;
	for (JIndex i=1; i<=charCount; i++)
		{
		const JIndex startByte = iter->GetPrevByteIndex();

		const JBoolean ok = iter->Prev(&c);
		assert( ok );

		JIndex endByte = 0;
		if (!iter->AtBeginning())
			{
			endByte = iter->GetPrevByteIndex();
			}

		if (itsCount.charCount > 0)
			{
			itsCount.charCount--;
			itsCount.byteCount -= startByte - endByte;
			}
		else
			{
			PrependToSave(c, iter->GetNextCharacterIndex());
			itsOrigStartIndex.charIndex--;
			itsOrigStartIndex.byteIndex -= startByte - endByte;
			}
		}
}

/******************************************************************************
 HandleFwdDelete

	Call this -before- deleting the characters.

	Iterator is modified.

 ******************************************************************************/

void
JTEUndoTyping::HandleFwdDelete
	(
	JStringIterator*	iter,
	const JSize			charCount
	)
{
	assert( IsActive() );

	JUtf8Character c;
	for (JIndex i=1; i<=charCount; i++)
		{
		const JBoolean ok = iter->Next(&c);
		assert( ok );

		AppendToSave(c, iter->GetPrevCharacterIndex());
		}
}
