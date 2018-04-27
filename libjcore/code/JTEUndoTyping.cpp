/******************************************************************************
 JTEUndoTyping.cpp

	Class to undo typing into a JStyledTextBuffer object.

	BASE CLASS = JTEUndoTextBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTyping.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTyping::JTEUndoTyping
	(
	JStyledTextBuffer*					buffer,
	const JStyledTextBuffer::TextIndex&	start
	)
	:
	JTEUndoTextBase(buffer, JStyledTextBuffer::TextRange(start, JStyledTextBuffer::TextCount())),
	itsOrigStartIndex(start)
{
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
	UndoText(JStyledTextBuffer::TextRange(itsOrigStartIndex, itsCount));
}

/******************************************************************************
 HandleCharacters

 ******************************************************************************/

void
JTEUndoTyping::HandleCharacters
	(
	const JStyledTextBuffer::TextCount& count
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
	const JStringMatch& match
	)
{
	assert( IsActive() );

	const JString s(GetBuffer()->GetText().GetBytes(), match.GetUtf8ByteRange(), kJFalse);
	JStringIterator iter(s, kJIteratorStartAtEnd);

	const JIndex firstCharIndex = match.GetCharacterRange().first;

	JUtf8Character c;
	while (!iter.AtBeginning())
		{
		const JIndex startByte = iter.GetPrevByteIndex();

		const JBoolean ok = iter.Prev(&c);
		assert( ok );

		JIndex byteCount = startByte;
		if (!iter.AtBeginning())
			{
			byteCount -= iter.GetPrevByteIndex();
			}

		if (itsCount.charCount > 0)
			{
			itsCount.charCount--;
			itsCount.byteCount -= byteCount;
			}
		else
			{
			PrependToSave(c, iter.GetNextCharacterIndex() + firstCharIndex - 1);
			itsOrigStartIndex.charIndex--;
			itsOrigStartIndex.byteIndex -= byteCount;
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
	const JStringMatch& match
	)
{
	assert( IsActive() );

	const JString s(GetBuffer()->GetText().GetBytes(), match.GetUtf8ByteRange(), kJFalse);
	JStringIterator iter(s, kJIteratorStartAtBeginning);

	const JIndex firstCharIndex = match.GetCharacterRange().first;

	JUtf8Character c;
	while (!iter.AtEnd())
		{
		const JBoolean ok = iter.Next(&c);
		assert( ok );

		AppendToSave(c, iter.GetPrevCharacterIndex() + firstCharIndex - 1);
		}
}

/******************************************************************************
 SameStartIndex

 ******************************************************************************/

JBoolean
JTEUndoTyping::SameStartIndex
	(
	const JStyledTextBuffer::TextIndex& index
	)
	const
{
	return JI2B( index.charIndex == itsOrigStartIndex.charIndex &&
				 index.byteIndex == itsOrigStartIndex.byteIndex );
}
