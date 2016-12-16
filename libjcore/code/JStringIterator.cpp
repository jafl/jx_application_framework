/******************************************************************************
 JStringIterator.cpp

						The JString Iterator Class

	UTF-8 combines the worst of arrays and linked lists.  Search and
	insertion are both O(N), and iteration is complicated by characters not
	being a fixed number of bytes.

	This iterator provides the most efficient, error-resistant way to loop
	through the UTF-8 characters in a string:

		JString s;
		...
		JStringIterator iterator(s);
		JUtf8Character  char;

		while (iterator.Next(&char))
			{
			<do something with the character>
			}

	or

		while (iterator.Next("..."))
			{
			<do something with JStringMatch returned by GetLastMatch()>
			}

	An iterator is robust in the face of insertions and deletions to
	the JString it is working on.

	*** For simplicity, each JString can have only one iterator at a time,
		and any modification not done via the iterator will invalidate the
		iterator.

	Refer to the docs for JOrderedSetIterator for more details.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JStringIterator::JStringIterator
	(
	const JString&			s,
	const JIteratorPosition	start,
	const JIndex			index
	)
	:
	itsConstString(&s),
	itsString(NULL),
	itsLastMatch(NULL)
{
	MoveTo(start, index);
	s.SetIterator(this);
}

JStringIterator::JStringIterator
	(
	JString*				s,
	const JIteratorPosition	start,
	const JIndex			index
	)
	:
	itsConstString(s),
	itsString(s),
	itsLastMatch(NULL)
{
	MoveTo(start, index);
	s->SetIterator(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStringIterator::~JStringIterator()
{
	Invalidate();
}

/******************************************************************************
 Invalidate

 ******************************************************************************/

void
JStringIterator::Invalidate()
{
	if (itsConstString != NULL)
		{
		itsConstString->SetIterator(NULL);
		}

	ClearLastMatch();
	itsConstString    = NULL;
	itsString         = NULL;
	itsCursorPosition = 0;
}

/******************************************************************************
 GetLastMatch

	Explodes if there was no last match, because this makes the API simpler
	and it's really easy to know if you just got a match.

 ******************************************************************************/

const JStringMatch&
JStringIterator::GetLastMatch()
	const
{
	assert( itsLastMatch != NULL );
	return *itsLastMatch;
}

/******************************************************************************
 ClearLastMatch (private)

	Return kJTrue if iterator is positioned at the end of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

inline void
JStringIterator::ClearLastMatch()
{
	jdelete itsLastMatch;
	itsLastMatch = NULL;
}

/******************************************************************************
 MoveTo

 ******************************************************************************/

void
JStringIterator::MoveTo
	(
	const JIteratorPosition	newPosition,
	const JIndex			index
	)
{
	if (itsConstString == NULL)
		{
		return;
		}

	if (newPosition == kJIteratorStartAtBeginning)
		{
		itsCursorPosition = 0;
		}
	else if (newPosition == kJIteratorStartAtEnd)
		{
		itsCursorPosition = itsConstString->GetByteCount();
		}
	else if (newPosition == kJIteratorStartBeforeByte)
		{
		assert( itsConstString->ByteIndexValid(index) );
		itsCursorPosition = index-1;
		}
	else if (newPosition == kJIteratorStartAfterByte)
		{
		assert( itsConstString->ByteIndexValid(index) );
		itsCursorPosition = index;
		}
	else if (newPosition == kJIteratorStartBefore)
		{
		assert( itsConstString->CharacterIndexValid(index) );
		JUtf8ByteRange r =
			JString::CharacterToUtf8ByteRange(itsConstString->GetBytes(),
											  JCharacterRange(1, index-1));
		itsCursorPosition = r.last;
		}
	else
		{
		assert( newPosition == kJIteratorStartAfter );
		assert( itsConstString->CharacterIndexValid(index) );
		JUtf8ByteRange r =
			JString::CharacterToUtf8ByteRange(itsConstString->GetBytes(),
											  JCharacterRange(1, index));
		itsCursorPosition = r.last;
		}
}

/******************************************************************************
 SkipPrev

 ******************************************************************************/

void
JStringIterator::SkipPrev
	(
	const JSize characterCount
	)
{
	if (AtBeginning())
		{
		return;
		}

	const JUtf8Byte* bytes = itsConstString->GetBytes() + itsCursorPosition - 1;
	for (JIndex i=1; i<=characterCount; i++)
		{
		JSize byteCount;
		JUtf8Character::GetPrevCharacterByteCount(bytes, &byteCount);	// accept invalid characters

		if (itsCursorPosition <= byteCount)
			{
			itsCursorPosition = 0;
			break;
			}

		itsCursorPosition -= byteCount;
		bytes             -= byteCount;
		}
}

/******************************************************************************
 SkipNext

 ******************************************************************************/

void
JStringIterator::SkipNext
	(
	const JSize characterCount
	)
{
	if (AtEnd())
		{
		return;
		}

	const JSize maxPosition = itsConstString->GetByteCount();

	const JUtf8Byte* bytes = itsConstString->GetBytes() + itsCursorPosition;
	for (JIndex i=1; i<=characterCount; i++)
		{
		JSize byteCount;
		JUtf8Character::GetCharacterByteCount(bytes, &byteCount);	// accept invalid characters
		itsCursorPosition += byteCount;

		if (itsCursorPosition >= maxPosition)
			{
			itsCursorPosition = maxPosition;
			break;
			}

		bytes += byteCount;
		}
}

/******************************************************************************
 Prev

	Returns kJTrue if there is a previous character, fetching the previous
	character in the string and decrementing the iterator position.
	Otherwise returns kJFalse without fetching or decrementing.

 ******************************************************************************/

JBoolean
JStringIterator::Prev
	(
	JUtf8Character*	data,
	const JBoolean	move
	)
{
	if (AtBeginning())
		{
		return kJFalse;
		}

	const JUtf8Byte* ptr = itsConstString->GetBytes() + itsCursorPosition - 1;

	JSize byteCount;
	JUtf8Character::GetPrevCharacterByteCount(ptr, &byteCount);	// accept invalid characters

	if (itsCursorPosition < byteCount)	// went backwards beyond start of string!
		{
		itsCursorPosition = 0;
		return kJFalse;
		}

	if (move)
		{
		itsCursorPosition -= byteCount;
		}

	ptr -= byteCount-1;
	data->Set(ptr);
	return kJTrue;
}

/******************************************************************************
 Next

	Returns kJTrue if there is a next character, fetching the next
	character in the list and incrementing the iterator position.
	Otherwise returns kJFalse without fetching or incrementing.

 ******************************************************************************/

JBoolean
JStringIterator::Next
	(
	JUtf8Character*	data,
	const JBoolean	move
	)
{
	if (AtEnd())
		{
		return kJFalse;
		}

	const JUtf8Byte* ptr = itsConstString->GetBytes() + itsCursorPosition;

	JSize byteCount;
	JUtf8Character::GetCharacterByteCount(ptr, &byteCount);	// accept invalid characters

	if (move)
		{
		itsCursorPosition += byteCount;
		}

	const JSize maxPosition = itsConstString->GetByteCount();
	if (itsCursorPosition > maxPosition)	// went backwards beyond end of string!
		{
		itsCursorPosition = maxPosition;
		return kJFalse;
		}

	data->Set(ptr);
	return kJTrue;
}

/******************************************************************************
 Prev

	Returns kJTrue if a match is found earlier in the string.  Match
	details can be retrieved from GetLastMatch().

	If a match is found, the cursor position is set to the start of the
	match.  Otherwise the cursor position is moved to the start of the
	string.

 ******************************************************************************/

JBoolean
JStringIterator::Prev
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		caseSensitive
	)
{
	ClearLastMatch();
	if (AtBeginning())
		{
		return kJFalse;
		}

	JIndex i = itsCursorPosition;
	if (itsConstString->SearchBackward(str, byteCount, caseSensitive, &i))
		{
		itsCursorPosition = i-1;

		JUtf8ByteRange r;
		r.SetFirstAndCount(i, byteCount);

		itsLastMatch = jnew JStringMatch(*itsConstString, r);
		assert( itsLastMatch != NULL );

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Next

	Returns kJTrue if a match is found later in the string.  Match
	details can be retrieved from GetLastMatch().

	If a match is found, the cursor position is set beyond the end of the
	match.  Otherwise the cursor position is moved to the end of the
	string.

 ******************************************************************************/

JBoolean
JStringIterator::Next
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		caseSensitive
	)
{
	ClearLastMatch();
	if (AtEnd())
		{
		return kJFalse;
		}

	JIndex i = itsCursorPosition + 1;
	if (itsConstString->SearchForward(str, byteCount, caseSensitive, &i))
		{
		itsCursorPosition = i + byteCount - 1;

		JUtf8ByteRange r;
		r.SetFirstAndCount(i, byteCount);

		itsLastMatch = jnew JStringMatch(*itsConstString, r);
		assert( itsLastMatch != NULL );

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
