/******************************************************************************
 JSTUndoTyping.cpp

	Class to undo typing into a JStyledText object.

	BASE CLASS = JSTUndoTextBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JSTUndoTyping.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JSTUndoTyping::JSTUndoTyping
	(
	JStyledText*					text,
	const JStyledText::TextIndex&	start
	)
	:
	JSTUndoTextBase(text, JStyledText::TextRange(start, JStyledText::TextCount())),
	itsOrigStartIndex(start)
{
}

JSTUndoTyping::JSTUndoTyping
	(
	JStyledText*					text,
	const JStyledText::TextRange&	replaceRange
	)
	:
	JSTUndoTextBase(text, replaceRange),
	itsOrigStartIndex(replaceRange.charRange.first, replaceRange.byteRange.first)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTUndoTyping::~JSTUndoTyping()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JSTUndoTyping::Undo()
{
	UndoText(JStyledText::TextRange(itsOrigStartIndex, itsCount));
}

/******************************************************************************
 HandleCharacters

 ******************************************************************************/

void
JSTUndoTyping::HandleCharacters
	(
	const JStyledText::TextCount& count
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
JSTUndoTyping::HandleDelete
	(
	const JStringMatch& match
	)
{
	assert( IsActive() );

	const JString s(GetText()->GetText().GetBytes(), match.GetUtf8ByteRange(), JString::kNoCopy);
	JStringIterator iter(s, kJIteratorStartAtEnd);

	const JIndex firstCharIndex = match.GetCharacterRange().first;

	JUtf8Character c;
	while (!iter.AtBeginning())
	{
		const JIndex startByte = iter.GetPrevByteIndex();

		const bool ok = iter.Prev(&c);
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
JSTUndoTyping::HandleFwdDelete
	(
	const JStringMatch& match
	)
{
	assert( IsActive() );

	const JString s(GetText()->GetText().GetBytes(), match.GetUtf8ByteRange(), JString::kNoCopy);
	JStringIterator iter(s, kJIteratorStartAtBeginning);

	const JIndex firstCharIndex = match.GetCharacterRange().first;

	JUtf8Character c;
	while (!iter.AtEnd())
	{
		const bool ok = iter.Next(&c);
		assert( ok );

		AppendToSave(c, iter.GetPrevCharacterIndex() + firstCharIndex - 1);
	}
}

/******************************************************************************
 MatchesCurrentIndex

 ******************************************************************************/

bool
JSTUndoTyping::MatchesCurrentIndex
	(
	const JStyledText::TextIndex& index
	)
	const
{
	const JStyledText::TextIndex i = itsOrigStartIndex + itsCount;

	return index.charIndex == i.charIndex &&
				 index.byteIndex == i.byteIndex;
}
