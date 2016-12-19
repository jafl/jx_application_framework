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
		JUtf8ByteRange r  = itsConstString->CharacterToUtf8ByteRange(JCharacterRange(1, index-1));
		itsCursorPosition = r.last;
		}
	else
		{
		assert( newPosition == kJIteratorStartAfter );
		assert( itsConstString->CharacterIndexValid(index) );
		JUtf8ByteRange r  = itsConstString->CharacterToUtf8ByteRange(JCharacterRange(1, index));
		itsCursorPosition = r.last;
		}
}

/******************************************************************************
 SkipPrev

	If there are enough previous characters, skips them and returns kJTrue.
	Otherwise, moves iterator position to the beginning and returns
	kJFalse.

 ******************************************************************************/

JBoolean
JStringIterator::SkipPrev
	(
	const JSize characterCount
	)
{
	if (AtBeginning())
		{
		return kJFalse;
		}

	const JUtf8Byte* bytes = itsConstString->GetBytes() + itsCursorPosition - 1;
	for (JIndex i=1; i<=characterCount; i++)
		{
		JSize byteCount;
		JUtf8Character::GetPrevCharacterByteCount(bytes, &byteCount);	// accept invalid characters

		if (itsCursorPosition <= byteCount)
			{
			itsCursorPosition = 0;
			return kJFalse;
			}

		itsCursorPosition -= byteCount;
		bytes             -= byteCount;
		}

	return kJTrue;
}

/******************************************************************************
 SkipNext

	If there are enough following characters, skips them and returns kJTrue.
	Otherwise, moves iterator position to the end and returns kJFalse.

 ******************************************************************************/

JBoolean
JStringIterator::SkipNext
	(
	const JSize characterCount
	)
{
	if (AtEnd())
		{
		return kJFalse;
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
			return kJFalse;
			}

		bytes += byteCount;
		}

	return kJTrue;
}

/******************************************************************************
 Prev

	Returns kJTrue if there is a previous character, fetching the previous
	character in the string and (if requested) decrementing the iterator
	position.  Otherwise returns kJFalse without fetching or decrementing.

 ******************************************************************************/

JBoolean
JStringIterator::Prev
	(
	JUtf8Character*	c,
	const JBoolean	move
	)
{
	const JUtf8Byte* ptr = itsConstString->GetBytes() + itsCursorPosition - 1;

	JSize byteCount;
	if (!PreparePrev(move, &byteCount))
		{
		return kJFalse;
		}

	ptr -= byteCount-1;
	c->Set(ptr);
	return kJTrue;
}

/******************************************************************************
 Next

	Returns kJTrue if there is a next character, fetching the next
	character in the list and (if requested) incrementing the iterator
	position.  Otherwise returns kJFalse without fetching or incrementing.

 ******************************************************************************/

JBoolean
JStringIterator::Next
	(
	JUtf8Character*	c,
	const JBoolean	move
	)
{
	const JUtf8Byte* ptr = itsConstString->GetBytes() + itsCursorPosition;

	JSize byteCount;
	if (!PrepareNext(move, &byteCount))
		{
		return kJFalse;
		}

	c->Set(ptr);
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

/******************************************************************************
 SetPrev

	If there is a previous character, sets it, (if requested) decrements
	the iterator position, and returns kJTrue.  Otherwise, returns kJFalse.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

JBoolean
JStringIterator::SetPrev
	(
	const JUtf8Character&	c,
	const JBoolean			move
	)
{
	assert( itsString != NULL );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.last = itsCursorPosition;

	JSize byteCount;
	if (!PreparePrev(move, &byteCount))
		{
		return kJFalse;
		}

	r.first = r.last - byteCount + 1;
	itsString->ReplaceBytes(r, c.GetBytes(), c.GetByteCount());
	return kJTrue;
}

/******************************************************************************
 SetNext

	If there is a next character, sets it, (if requested) increments the
	iterator position, and returns kJTrue.  Otherwise, returns kJFalse.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

JBoolean
JStringIterator::SetNext
	(
	const JUtf8Character&	c,
	const JBoolean			move
	)
{
	assert( itsString != NULL );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.first = itsCursorPosition + 1;

	JSize byteCount;
	if (!PrepareNext(move, &byteCount))
		{
		return kJFalse;
		}

	r.last = r.first + byteCount - 1;
	itsString->ReplaceBytes(r, c.GetBytes(), c.GetByteCount());

	if (move)
		{
		itsCursorPosition += c.GetByteCount();
		itsCursorPosition -= r.GetCount();
		}

	return kJTrue;
}

/******************************************************************************
 RemovePrev

	If there are enough previous characters, removes them and returns
	kJTrue.  Otherwise, removes all preceding characters and returns kJFalse.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

JBoolean
JStringIterator::RemovePrev
	(
	const JSize characterCount
	)
{
	assert( itsString != NULL );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.last = itsCursorPosition;

	const JBoolean result = SkipPrev(characterCount);

	r.first = itsCursorPosition + 1;
	itsString->ReplaceBytes(r, NULL, 0);
	return result;
}

/******************************************************************************
 RemoveNext

	If there are enough following characters, removes them and returns
	kJTrue.  Otherwise, removes all following characters and returns kJFalse.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

JBoolean
JStringIterator::RemoveNext
	(
	const JSize characterCount
	)
{
	assert( itsString != NULL );
	ClearLastMatch();

	const JCursorPosition saved = itsCursorPosition;

	JUtf8ByteRange r;
	r.first = itsCursorPosition + 1;

	const JBoolean result = SkipNext(characterCount);

	r.last = itsCursorPosition;
	itsString->ReplaceBytes(r, NULL, 0);

	itsCursorPosition = saved;
	return result;
}

/******************************************************************************
 ReplaceLastMatch

	Replaces the characters from the last match.

	*** Match must exist.
	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range
	)
{
	assert( itsLastMatch != NULL );
	assert( itsString != NULL );

	JUtf8ByteRange r = itsLastMatch->GetUtf8ByteRange();
	if (itsCursorPosition > r.last)
		{
		itsCursorPosition -= r.GetCount();
		}
	else if (itsCursorPosition >= r.first)
		{
		itsCursorPosition = r.first - 1;
		}

	itsString->ReplaceBytes(r, str + range.first - 1, range.GetCount());
	ClearLastMatch();
}

/******************************************************************************
 PreparePrev (private)

 ******************************************************************************/

JBoolean
JStringIterator::PreparePrev
	(
	const JBoolean	move,
	JSize*			byteCount
	)
{
	if (AtBeginning())
		{
		return kJFalse;
		}

	const JUtf8Byte* ptr = itsConstString->GetBytes() + itsCursorPosition - 1;

	JUtf8Character::GetPrevCharacterByteCount(ptr, byteCount);	// accept invalid characters

	if (itsCursorPosition < *byteCount)	// went backwards beyond start of string!
		{
		itsCursorPosition = 0;
		return kJFalse;
		}

	if (move)
		{
		itsCursorPosition -= *byteCount;
		}

	return kJTrue;
}

/******************************************************************************
 PrepareNext (private)

 ******************************************************************************/

JBoolean
JStringIterator::PrepareNext
	(
	const JBoolean	move,
	JSize*			byteCount
	)
{
	if (AtEnd())
		{
		return kJFalse;
		}

	const JUtf8Byte* ptr = itsConstString->GetBytes() + itsCursorPosition;

	JUtf8Character::GetCharacterByteCount(ptr, byteCount);	// accept invalid characters

	if (move)
		{
		itsCursorPosition += *byteCount;
		}

	const JSize maxPosition = itsConstString->GetByteCount();
	if (itsCursorPosition > maxPosition)	// went forwards beyond end of string!
		{
		itsCursorPosition = maxPosition;
		return kJFalse;
		}

	return kJTrue;
}
