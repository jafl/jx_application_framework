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

	Refer to the docs for JListIterator for more details.

	Copyright (C) 2016-17 by John Lindal.

 ******************************************************************************/

#include <JStringIterator.h>
#include <JStringMatch.h>
#include <JRegex.h>
#include <JMinMax.h>
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
	itsString(nullptr),
	itsLastMatch(nullptr)
{
	s.SetIterator(this);
	MoveTo(start, index);
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
	itsLastMatch(nullptr)
{
	s->SetIterator(this);
	MoveTo(start, index);
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
	if (itsConstString != nullptr)
		{
		itsConstString->SetIterator(nullptr);
		}

	ClearLastMatch();
	itsConstString     = nullptr;
	itsString          = nullptr;
	itsByteOffset      = 0;
	itsCharacterOffset = 0;
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
	assert( itsLastMatch != nullptr );
	return *itsLastMatch;
}

/******************************************************************************
 ClearLastMatch (private)

	Return kJTrue if iterator is positioned at the end of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

void
JStringIterator::ClearLastMatch()
{
	jdelete itsLastMatch;
	itsLastMatch = nullptr;
}

/******************************************************************************
 GetPrevCharacterIndex

	asserts that there is a previous character

 ******************************************************************************/

JIndex
JStringIterator::GetPrevCharacterIndex()
	const
{
	assert( !AtBeginning() );
	return itsCharacterOffset;
}

/******************************************************************************
 GetNextCharacterIndex

	asserts that there is a next character

 ******************************************************************************/

JIndex
JStringIterator::GetNextCharacterIndex()
	const
{
	assert( !AtEnd() );
	return itsCharacterOffset + 1;
}

/******************************************************************************
 GetPrevByteIndex

	asserts that there is a previous byte

 ******************************************************************************/

JIndex
JStringIterator::GetPrevByteIndex()
	const
{
	assert( !AtBeginning() );
	return itsByteOffset;
}

/******************************************************************************
 GetNextByteIndex

	asserts that there is a next byte

 ******************************************************************************/

JIndex
JStringIterator::GetNextByteIndex()
	const
{
	assert( !AtEnd() );
	return itsByteOffset + 1;
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
	if (itsConstString == nullptr)
		{
		return;
		}

	if (newPosition == kJIteratorStartAtBeginning)
		{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
		}
	else if (newPosition == kJIteratorStartAtEnd)
		{
		itsByteOffset      = itsConstString->GetByteCount();
		itsCharacterOffset = itsConstString->GetCharacterCount();
		}
	else if (newPosition == kJIteratorStartBeforeByte)
		{
		assert( (itsConstString->IsEmpty() && index == 1) ||
				itsConstString->ByteIndexValid(index) );

		itsByteOffset      = index-1;
		itsCharacterOffset = JString::CountCharacters(itsConstString->GetRawBytes(), itsByteOffset);
		}
	else if (newPosition == kJIteratorStartAfterByte)
		{
		assert( itsConstString->ByteIndexValid(index) );
		itsByteOffset      = index;
		itsCharacterOffset = JString::CountCharacters(itsConstString->GetRawBytes(), itsByteOffset);
		}
	else if (newPosition == kJIteratorStartBefore)
		{
		assert( (itsConstString->IsEmpty() && index == 1) ||
				itsConstString->CharacterIndexValid(index) );

		JUtf8ByteRange r   = itsConstString->CharacterToUtf8ByteRange(JCharacterRange(1, index-1));
		itsByteOffset      = r.last;
		itsCharacterOffset = index-1;
		}
	else
		{
		assert( newPosition == kJIteratorStartAfter );
		assert( itsConstString->CharacterIndexValid(index) );
		JUtf8ByteRange r   = itsConstString->CharacterToUtf8ByteRange(JCharacterRange(1, index));
		itsByteOffset      = r.last;
		itsCharacterOffset = index;
		}
}

/******************************************************************************
 UnsafeMoveTo

	Intended for highly optimized code working with huge strings.  Only
	accepts kJIteratorStartBefore/After, since others can be passed to
	MoveTo().

	*** Assumes, but does not verify, that character index and byte index
		point to the same location!

 ******************************************************************************/

void
JStringIterator::UnsafeMoveTo
	(
	const JIteratorPosition	newPosition,
	const JIndex			characterIndex,
	const JIndex			byteIndex
	)
{
	if (itsConstString == nullptr)
		{
		return;
		}

	if (newPosition == kJIteratorStartBefore)
		{
		assert( (itsConstString->IsEmpty() && characterIndex == 1) ||
				characterIndex == itsConstString->GetCharacterCount()+1 ||
				itsConstString->CharacterIndexValid(characterIndex) );

		assert( (itsConstString->IsEmpty() && byteIndex == 1) ||
				byteIndex == itsConstString->GetByteCount()+1 ||
				itsConstString->ByteIndexValid(byteIndex) );

		itsByteOffset      = byteIndex-1;
		itsCharacterOffset = characterIndex-1;
		}
	else
		{
		assert( itsConstString->CharacterIndexValid(characterIndex) );
		assert( itsConstString->ByteIndexValid(byteIndex) );

		JSize byteCount;
		JUtf8Character::GetCharacterByteCount(itsConstString->GetRawBytes() + byteIndex-1, &byteCount);

		itsByteOffset      = byteIndex + byteCount-1;
		itsCharacterOffset = characterIndex;
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

	JSize byteCount;
	const JBoolean ok =
		JString::CountBytesBackward(itsConstString->GetRawBytes(), itsByteOffset,
									characterCount, &byteCount);
	if (ok)
		{
		itsByteOffset      -= byteCount;
		itsCharacterOffset -= characterCount;
		}
	else
		{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
		}

	return ok;
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

	const JUtf8Byte* bytes = itsConstString->GetRawBytes() + itsByteOffset;
	for (JIndex i=1; i<=characterCount; i++)
		{
		JSize byteCount;
		JUtf8Character::GetCharacterByteCount(bytes, &byteCount);	// accept invalid characters
		itsByteOffset += byteCount;
		itsCharacterOffset++;

		if (itsByteOffset >= maxPosition)
			{
			itsByteOffset      = maxPosition;
			itsCharacterOffset = itsConstString->GetCharacterCount();
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
	const JUtf8Byte* ptr = itsConstString->GetRawBytes() + itsByteOffset - 1;

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
	const JUtf8Byte* ptr = itsConstString->GetRawBytes() + itsByteOffset;

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

	JIndex i = itsByteOffset;

	JSize count;
	JUtf8Character::GetPrevCharacterByteCount(itsConstString->GetRawBytes() + i-1, &count);

	i -= count-1;

	if (itsConstString->SearchBackward(str, byteCount, caseSensitive, &i))
		{
		itsCharacterOffset -=	// before updating itsByteOffset
			JString::CountCharacters(itsConstString->GetRawBytes(),
				JUtf8ByteRange(i, itsByteOffset));

		itsByteOffset = i-1;

		JUtf8ByteRange r;
		r.SetFirstAndCount(i, byteCount);

		itsLastMatch = jnew JStringMatch(*itsConstString, r);
		assert( itsLastMatch != nullptr );
		itsLastMatch->SetFirstCharacterIndex(itsCharacterOffset + 1);

		return kJTrue;
		}
	else
		{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
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

	JIndex i = itsByteOffset + 1;
	if (itsConstString->SearchForward(str, byteCount, caseSensitive, &i))
		{
		itsCharacterOffset +=	// before updating itsByteOffset
			JString::CountCharacters(itsConstString->GetRawBytes(),
				JUtf8ByteRange(itsByteOffset + 1, i + byteCount - 1));

		itsByteOffset = i + byteCount - 1;

		JUtf8ByteRange r;
		r.SetFirstAndCount(i, byteCount);

		itsLastMatch = jnew JStringMatch(*itsConstString, r);
		assert( itsLastMatch != nullptr );
		itsLastMatch->SetLastCharacterIndex(itsCharacterOffset);

		return kJTrue;
		}
	else
		{
		itsByteOffset      = itsConstString->GetByteCount();
		itsCharacterOffset = itsConstString->GetCharacterCount();
		return kJFalse;
		}
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
	const JRegex& pattern
	)
{
	ClearLastMatch();
	if (AtBeginning())
		{
		return kJFalse;
		}

	const JStringMatch m = pattern.MatchBackward(*itsConstString, itsByteOffset);
	if (!m.IsEmpty())
		{
		itsCharacterOffset -=	// before updating itsByteOffset
			JString::CountCharacters(itsConstString->GetRawBytes(),
				JUtf8ByteRange(m.GetUtf8ByteRange().first, itsByteOffset));

		itsByteOffset = m.GetUtf8ByteRange().first - 1;

		itsLastMatch = jnew JStringMatch(m);
		assert( itsLastMatch != nullptr );
		itsLastMatch->SetFirstCharacterIndex(itsCharacterOffset + 1);

		return kJTrue;
		}
	else
		{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
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
	const JRegex& pattern
	)
{
	ClearLastMatch();
	if (AtEnd())
		{
		return kJFalse;
		}

	const JStringMatch& m = pattern.MatchForward(*itsConstString, itsByteOffset + 1);
	if (!m.IsEmpty())
		{
		itsCharacterOffset +=	// before updating itsByteOffset
			JString::CountCharacters(itsConstString->GetRawBytes(),
				JUtf8ByteRange(itsByteOffset + 1, m.GetUtf8ByteRange().last));

		itsByteOffset = m.GetUtf8ByteRange().last;

		itsLastMatch = jnew JStringMatch(m);
		assert( itsLastMatch != nullptr );
		itsLastMatch->SetLastCharacterIndex(itsCharacterOffset);

		return kJTrue;
		}
	else
		{
		itsByteOffset      = itsConstString->GetByteCount();
		itsCharacterOffset = itsConstString->GetCharacterCount();
		return kJFalse;
		}
}

/******************************************************************************
 FinishMatch

	Finishes matching and sets itsLastMatch.

	If ignoreLastMatch, ignores the range currently stored in itsLastMatch.
	This is useful for extracting everything except end-of-token.

 ******************************************************************************/

const JStringMatch&
JStringIterator::FinishMatch
	(
	const JBoolean ignoreLastMatch
	)
{
	JCursorPosition pos = itsByteOffset;
	if (ignoreLastMatch && itsLastMatch != nullptr)
		{
		const JSize ignoreCount = itsLastMatch->GetByteCount();
		if (pos < itsMatchStartByte)
			{
			pos += ignoreCount;
			pos  = JMin(pos, itsMatchStartByte);
			}
		else if (itsMatchStartByte < pos && ignoreCount <= pos)
			{
			pos -= ignoreCount;
			pos  = JMax(pos, itsMatchStartByte);
			}
		else if (itsMatchStartByte < pos)
			{
			pos = itsMatchStartByte;
			}
		}

	ClearLastMatch();

	JUtf8ByteRange r;
	r.first = JMin(itsMatchStartByte, pos) + 1;
	r.last  = JMax(itsMatchStartByte, pos);

	itsLastMatch = jnew JStringMatch(*itsConstString, r);
	assert( itsLastMatch != nullptr );

	if (itsMatchStartByte <= pos)
		{
		itsLastMatch->SetFirstCharacterIndex(itsMatchStartChar + 1);
		}
	else	// pos < itsMatchStartByte
		{
		itsLastMatch->SetLastCharacterIndex(itsMatchStartChar);
		}

	return *itsLastMatch;
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
	assert( itsString != nullptr );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.last = itsByteOffset;

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
	assert( itsString != nullptr );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.first = itsByteOffset + 1;

	JSize byteCount;
	if (!PrepareNext(move, &byteCount))
		{
		return kJFalse;
		}

	r.last = r.first + byteCount - 1;
	itsString->ReplaceBytes(r, c.GetBytes(), c.GetByteCount());

	if (move)
		{
		itsByteOffset += c.GetByteCount();
		itsByteOffset -= r.GetCount();
		// itsCharacterOffset does not change
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
	assert( itsString != nullptr );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.last = itsByteOffset;

	const JBoolean result = SkipPrev(characterCount);

	r.first = itsByteOffset + 1;
	itsString->ReplaceBytes(r, nullptr, 0);
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
	assert( itsString != nullptr );
	ClearLastMatch();

	const JCursorPosition savedByteOffset      = itsByteOffset;
	const JCursorPosition savedCharacterOffset = itsCharacterOffset;

	JUtf8ByteRange r;
	r.first = itsByteOffset + 1;

	const JBoolean result = SkipNext(characterCount);

	r.last = itsByteOffset;
	itsString->ReplaceBytes(r, nullptr, 0);

	itsByteOffset      = savedByteOffset;
	itsCharacterOffset = savedCharacterOffset;
	return result;
}

/******************************************************************************
 RemoveAllPrev

	Removes all preceding characters.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

void
JStringIterator::RemoveAllPrev()
{
	assert( itsString != nullptr );
	ClearLastMatch();

	const JUtf8ByteRange r(1, itsByteOffset);
	itsString->ReplaceBytes(r, nullptr, 0);

	itsByteOffset      = 0;
	itsCharacterOffset = 0;
}

/******************************************************************************
 RemoveAllNext

	Removes all following characters.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

void
JStringIterator::RemoveAllNext()
{
	assert( itsString != nullptr );
	ClearLastMatch();

	const JUtf8ByteRange r(itsByteOffset + 1, itsString->GetByteCount());
	itsString->ReplaceBytes(r, nullptr, 0);
}

/******************************************************************************
 ReplaceLastMatch

	Replaces the characters from the last match, optionally matching case.

	*** Match must exist.
	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range,
	const JBoolean			matchCase
	)
{
	assert( itsLastMatch != nullptr );
	assert( itsString != nullptr );

	JString s(str, range, kJFalse);

	if (matchCase)
		{
		s.MatchCase(itsString->GetRawBytes(), itsLastMatch->GetUtf8ByteRange());
		}

	const JUtf8ByteRange r = itsLastMatch->GetUtf8ByteRange();
	if (itsByteOffset >= r.last)
		{
		itsByteOffset      -= r.GetCount();
		itsCharacterOffset -= itsLastMatch->GetCharacterCount();

		itsByteOffset      += s.GetByteCount();
		itsCharacterOffset += s.GetCharacterCount();
		}
	else if (itsByteOffset > r.first)
		{
		itsCharacterOffset -=	// before updating itsByteOffset
			JString::CountCharacters(itsConstString->GetRawBytes(),
				JUtf8ByteRange(r.first, itsByteOffset));

		itsByteOffset = r.first - 1;
		}

	itsString->ReplaceBytes(r, s.GetUnterminatedBytes(), s.GetByteCount());

	ClearLastMatch();
}

/******************************************************************************
 Insert

	Insert the given characters after the current position.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

void
JStringIterator::Insert
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range
	)
{
	assert( itsString != nullptr );

	JUtf8ByteRange r;
	r.SetFirstAndCount(itsByteOffset + 1, 0);
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

	const JUtf8Byte* ptr = itsConstString->GetRawBytes() + itsByteOffset - 1;

	JUtf8Character::GetPrevCharacterByteCount(ptr, byteCount);	// accept invalid characters

	if (itsByteOffset < *byteCount)	// went backwards beyond start of string!
		{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
		return kJFalse;
		}

	if (move)
		{
		itsByteOffset -= *byteCount;
		itsCharacterOffset--;
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

	const JUtf8Byte* ptr = itsConstString->GetRawBytes() + itsByteOffset;

	JUtf8Character::GetCharacterByteCount(ptr, byteCount);	// accept invalid characters

	if (move)
		{
		itsByteOffset += *byteCount;
		itsCharacterOffset++;
		}

	const JSize maxPosition = itsConstString->GetByteCount();
	if (itsByteOffset > maxPosition)	// went forwards beyond end of string!
		{
		itsByteOffset      = maxPosition;
		itsCharacterOffset = itsConstString->GetCharacterCount();
		return kJFalse;
		}

	return kJTrue;
}
