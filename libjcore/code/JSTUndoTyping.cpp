/******************************************************************************
 JSTUndoTyping.cpp

	Class to undo typing into a JStyledText object.

	BASE CLASS = JSTUndoTextBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JSTUndoTyping.h"
#include "jx-af/jcore/JStringIterator.h"
#include "jx-af/jcore/JStringMatch.h"
#include "jx-af/jcore/jAssert.h"

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

 ******************************************************************************/

void
JSTUndoTyping::HandleDelete
	(
	const JStringMatch&	match,
	const bool			fromAutoIndent
	)
{
	assert( IsActive() );

	if (fromAutoIndent)
	{
		assert( !match.IsEmpty() &&
				match.GetCharacterRange().last <= itsOrigStartIndex.charIndex + itsCount.charCount - 1 );
	}

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

		const JIndex charIndex = iter.GetNextCharacterIndex() + firstCharIndex - 1;

		if ((!fromAutoIndent && itsCount.charCount > 0) ||
			( fromAutoIndent && charIndex >= itsOrigStartIndex.charIndex))
		{
			itsCount.charCount--;
			itsCount.byteCount -= byteCount;
		}
		else
		{
			PrependToSave(c, charIndex);
			itsOrigStartIndex.charIndex--;
			itsOrigStartIndex.byteIndex -= byteCount;
		}
	}
}

/******************************************************************************
 HandleFwdDelete

	Call this -before- deleting the characters.

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
