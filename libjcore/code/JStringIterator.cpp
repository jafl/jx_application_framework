/******************************************************************************
 JStringIterator.cpp

						The JString Iterator Class

	UTF-8 combines the worst of arrays and linked lists.  Search and
	insertion are both O(N), and iteration is complicated by characters not
	being a fixed number of bytes.

				+-----+-----+-----+*****+*****+-----+-----+
		byte	|  1  |  2  |  3  |  4  |  5  |  6  |  7  |
				+-----+-----+-----+*****+*****+-----+-----+
		char	0     1     2     3     *     4     5     6

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

#include "JStringIterator.h"
#include "JStringMatch.h"
#include "JRegex.h"
#include "JMinMax.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JStringIterator::JStringIterator
	(
	const JString&	s,
	const Position	start,
	const JIndex	index
	)
	:
	itsConstString(&s),
	itsString(nullptr),
	itsByteOffset(0),
	itsCharacterOffset(0),
	itsLastMatch(nullptr)
{
	s.SetIterator(this);
	MoveTo(start, index);
}

JStringIterator::JStringIterator
	(
	JString*		s,
	const Position	start,
	const JIndex	index
	)
	:
	itsConstString(s),
	itsString(s),
	itsByteOffset(0),
	itsCharacterOffset(0),
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
 ClearLastMatch

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
	const Position	newPosition,
	const JIndex	index
	)
{
	if (itsConstString == nullptr)
	{
		return;
	}

	if (newPosition == kStartAtBeginning)
	{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
	}
	else if (newPosition == kStartAtEnd)
	{
		itsByteOffset      = itsConstString->GetByteCount();
		itsCharacterOffset = itsConstString->GetCharacterCount();
	}
	else if (newPosition == kStartBeforeByte)
	{
		assert( (itsConstString->IsEmpty() && index == 1) ||
				index == itsConstString->GetByteCount()+1 ||
				itsConstString->ByteIndexValid(index) );
		PrivateMoveToAfterByte(index-1);
	}
	else if (newPosition == kStartAfterByte)
	{
		assert( itsConstString->ByteIndexValid(index) );
		PrivateMoveToAfterByte(index);
	}
	else if (newPosition == kStartBeforeChar)
	{
		assert( (itsConstString->IsEmpty() && index == 1) ||
				index == itsConstString->GetCharacterCount()+1 ||
				itsConstString->CharacterIndexValid(index) );
		PrivateMoveToAfterCharacter(index-1);
	}
	else
	{
		assert( newPosition == kStartAfterChar );
		assert( itsConstString->CharacterIndexValid(index) );
		PrivateMoveToAfterCharacter(index);
	}
}

/******************************************************************************
 PrivateMoveToAfterCharacter (private)

 ******************************************************************************/

void
JStringIterator::PrivateMoveToAfterCharacter
	(
	const JIndex index
	)
{
	if (index > itsCharacterOffset)
	{
		SkipNext(index - itsCharacterOffset);
	}
	else if (index < itsCharacterOffset)
	{
		SkipPrev(itsCharacterOffset - index);
	}
}

/******************************************************************************
 PrivateMoveToAfterByte (private)

 ******************************************************************************/

void
JStringIterator::PrivateMoveToAfterByte
	(
	const JIndex index
	)
{
	const JUtf8Byte* str = itsConstString->GetRawBytes();
	JSize count;
	if (index < itsByteOffset)
	{
		while (index < itsByteOffset)
		{
			// accept invalid byte sequences as single characters
			JUtf8Character::GetPrevCharacterByteCount(str + itsByteOffset - 1, &count);
			if (itsByteOffset <= count)
			{
				itsCharacterOffset = itsByteOffset = 0;
				break;
			}
			itsByteOffset -= count;
			itsCharacterOffset--;
		}
	}
	else if (itsByteOffset < index)
	{
		while (itsByteOffset < index)
		{
			// accept invalid byte sequences as single characters
			JUtf8Character::GetCharacterByteCount(str + itsByteOffset, &count);
			itsByteOffset += count;
			itsCharacterOffset++;
		}
	}
}

/******************************************************************************
 UnsafeMoveTo

	Intended for highly optimized code working with huge strings.  Only
	accepts kJIteratorStartBefore/AfterChar, since others can be passed to
	MoveTo().

	*** Assumes, but does not verify, that character index and byte index
		point to the same location!

 ******************************************************************************/

void
JStringIterator::UnsafeMoveTo
	(
	const Position	newPosition,
	const JIndex	characterIndex,
	const JIndex	byteIndex
	)
{
	if (itsConstString == nullptr)
	{
		return;
	}

	if (newPosition == kStartBeforeChar)
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

	If there are enough previous characters, skips them and returns true.
	Otherwise, moves iterator position to the beginning and returns
	false.

 ******************************************************************************/

bool
JStringIterator::SkipPrev
	(
	const JSize characterCount
	)
{
	if (AtBeginning())
	{
		return false;
	}

	JSize byteCount;
	const bool ok =
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

	If there are enough following characters, skips them and returns true.
	Otherwise, moves iterator position to the end and returns false.

 ******************************************************************************/

bool
JStringIterator::SkipNext
	(
	const JSize characterCount
	)
{
	if (AtEnd())
	{
		return false;
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
			return false;
		}

		bytes += byteCount;
	}

	return true;
}

/******************************************************************************
 Prev

	Returns true if there is a previous character, fetching the previous
	character in the string and (if requested) decrementing the iterator
	position.  Otherwise returns false without fetching or decrementing.

 ******************************************************************************/

bool
JStringIterator::Prev
	(
	JUtf8Character*	c,
	const Action	move
	)
{
	const JUtf8Byte* ptr = itsConstString->GetRawBytes() + itsByteOffset - 1;

	JSize byteCount;
	if (!PreparePrev(move, &byteCount))
	{
		return false;
	}

	ptr -= byteCount-1;
	c->Set(ptr);
	return true;
}

/******************************************************************************
 Next

	Returns true if there is a next character, fetching the next
	character in the list and (if requested) incrementing the iterator
	position.  Otherwise returns false without fetching or incrementing.

 ******************************************************************************/

bool
JStringIterator::Next
	(
	JUtf8Character*	c,
	const Action	move
	)
{
	const JUtf8Byte* ptr = itsConstString->GetRawBytes() + itsByteOffset;

	JSize byteCount;
	if (!PrepareNext(move, &byteCount))
	{
		return false;
	}

	c->Set(ptr);
	return true;
}

/******************************************************************************
 Prev

	Returns true if a match is found earlier in the string.  Match
	details can be retrieved from GetLastMatch().

	If a match is found, the cursor position is set to the start of the
	match.  Otherwise the cursor position is moved to the start of the
	string.

 ******************************************************************************/

bool
JStringIterator::Prev
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JString::Case	caseSensitive
	)
{
	ClearLastMatch();
	if (AtBeginning())
	{
		return false;
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
		itsLastMatch->SetFirstCharacterIndex(itsCharacterOffset + 1);

		return true;
	}
	else
	{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
		return false;
	}
}

/******************************************************************************
 Next

	Returns true if a match is found later in the string.  Match
	details can be retrieved from GetLastMatch().

	If a match is found, the cursor position is set beyond the end of the
	match.  Otherwise the cursor position is moved to the end of the
	string.

 ******************************************************************************/

bool
JStringIterator::Next
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JString::Case	caseSensitive
	)
{
	ClearLastMatch();
	if (AtEnd())
	{
		return false;
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
		itsLastMatch->SetLastCharacterIndex(itsCharacterOffset);

		return true;
	}
	else
	{
		itsByteOffset      = itsConstString->GetByteCount();
		itsCharacterOffset = itsConstString->GetCharacterCount();
		return false;
	}
}

/******************************************************************************
 Prev

	Returns true if a match is found earlier in the string.  Match
	details can be retrieved from GetLastMatch().

	If a match is found, the cursor position is set to the start of the
	match.  Otherwise the cursor position is moved to the start of the
	string.

 ******************************************************************************/

bool
JStringIterator::Prev
	(
	const JRegex& pattern
	)
{
	ClearLastMatch();
	if (AtBeginning())
	{
		return false;
	}

	const JStringMatch m = pattern.MatchBackward(*itsConstString, itsByteOffset);
	if (!m.IsEmpty())
	{
		itsCharacterOffset -=	// before updating itsByteOffset
			JString::CountCharacters(itsConstString->GetRawBytes(),
				JUtf8ByteRange(m.GetUtf8ByteRange().first, itsByteOffset));

		itsByteOffset = m.GetUtf8ByteRange().first - 1;

		itsLastMatch = jnew JStringMatch(m);

		return true;
	}
	else
	{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
		return false;
	}
}

/******************************************************************************
 Next

	Returns true if a match is found later in the string.  Match
	details can be retrieved from GetLastMatch().

	If a match is found, the cursor position is set beyond the end of the
	match.  Otherwise the cursor position is moved to the end of the
	string.

 ******************************************************************************/

bool
JStringIterator::Next
	(
	const JRegex& pattern
	)
{
	ClearLastMatch();
	if (AtEnd())
	{
		return false;
	}

	const JStringMatch& m = pattern.MatchForward(*itsConstString, itsByteOffset + 1);
	if (!m.IsEmpty())
	{
		itsCharacterOffset +=	// before updating itsByteOffset
			JString::CountCharacters(itsConstString->GetRawBytes(),
				JUtf8ByteRange(itsByteOffset + 1, m.GetUtf8ByteRange().last));

		itsByteOffset = m.GetUtf8ByteRange().last;

		itsLastMatch = jnew JStringMatch(m);
		itsLastMatch->SetLastCharacterIndex(itsCharacterOffset);

		return true;
	}
	else
	{
		itsByteOffset      = itsConstString->GetByteCount();
		itsCharacterOffset = itsConstString->GetCharacterCount();
		return false;
	}
}

/******************************************************************************
 FinishMatch

	Finishes matching and sets itsLastMatch.

	If includeLastMatch, includes the range currently stored in itsLastMatch.
	This is off by default to simplify extracting everything except end-of-token.

 ******************************************************************************/

const JStringMatch&
JStringIterator::FinishMatch
	(
	const bool includeLastMatch
	)
{
	JCursorPosition pos = itsByteOffset;
	if (!includeLastMatch && itsLastMatch != nullptr)
	{
		const JSize ignoreCount = itsLastMatch->GetByteCount();
		if (pos < itsMatchStartByte && itsLastMatch->GetUtf8ByteRange().first == pos+1)
		{
			pos += ignoreCount;
			pos  = JMin(pos, itsMatchStartByte);
		}
		else if (itsMatchStartByte < pos && itsLastMatch->GetUtf8ByteRange().last == pos)
		{
			assert( ignoreCount <= pos );
			pos -= ignoreCount;
			pos  = JMax(pos, itsMatchStartByte);
		}
	}
	else if (includeLastMatch && itsLastMatch != nullptr)
	{
		const JSize includeCount = itsLastMatch->GetByteCount();
		if (pos < itsMatchStartByte && itsLastMatch->GetUtf8ByteRange().last == pos)
		{
			pos -= includeCount;
		}
		else if (itsMatchStartByte < pos && itsLastMatch->GetUtf8ByteRange().first == pos+1)
		{
			assert( pos + includeCount <= itsConstString->GetByteCount() );
			pos += includeCount;
		}
	}

	ClearLastMatch();

	JUtf8ByteRange r;
	r.first = JMin(itsMatchStartByte, pos) + 1;
	r.last  = JMax(itsMatchStartByte, pos);

	itsLastMatch = jnew JStringMatch(*itsConstString, r);

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
	the iterator position, and returns true.  Otherwise, returns false.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

bool
JStringIterator::SetPrev
	(
	const JUtf8Character&	c,
	const Action			move
	)
{
	assert( itsString != nullptr );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.last = itsByteOffset;

	JSize byteCount;
	if (!PreparePrev(move, &byteCount))
	{
		return false;
	}

	r.first = r.last - byteCount + 1;
	itsString->ReplaceBytes(r, c.GetBytes(), c.GetByteCount());
	return true;
}

/******************************************************************************
 SetNext

	If there is a next character, sets it, (if requested) increments the
	iterator position, and returns true.  Otherwise, returns false.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

bool
JStringIterator::SetNext
	(
	const JUtf8Character&	c,
	const Action			move
	)
{
	assert( itsString != nullptr );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.first = itsByteOffset + 1;

	JSize byteCount;
	if (!PrepareNext(move, &byteCount))
	{
		return false;
	}

	r.last = r.first + byteCount - 1;
	itsString->ReplaceBytes(r, c.GetBytes(), c.GetByteCount());

	if (move)
	{
		itsByteOffset += c.GetByteCount();
		itsByteOffset -= r.GetCount();
		// itsCharacterOffset does not change
	}

	return true;
}

/******************************************************************************
 RemovePrev

	If there are enough previous characters, removes them and returns
	true.  Otherwise, removes all preceding characters and returns false.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

bool
JStringIterator::RemovePrev
	(
	const JSize characterCount
	)
{
	assert( itsString != nullptr );
	ClearLastMatch();

	JUtf8ByteRange r;
	r.last = itsByteOffset;

	const bool result = SkipPrev(characterCount);

	r.first = itsByteOffset + 1;
	itsString->ReplaceBytes(r, nullptr, 0);
	return result;
}

/******************************************************************************
 RemoveNext

	If there are enough following characters, removes them and returns
	true.  Otherwise, removes all following characters and returns false.

	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

bool
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

	const bool result = SkipNext(characterCount);

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
	const bool			matchCase
	)
{
	assert( itsLastMatch != nullptr );
	assert( itsString != nullptr );

	JString s(str, range, JString::kNoCopy);

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

bool
JStringIterator::PreparePrev
	(
	const bool	move,
	JSize*		byteCount
	)
{
	if (AtBeginning())
	{
		return false;
	}

	const JUtf8Byte* ptr = itsConstString->GetRawBytes() + itsByteOffset - 1;

	JUtf8Character::GetPrevCharacterByteCount(ptr, byteCount);	// accept invalid characters

	if (itsByteOffset < *byteCount)	// went backwards beyond start of string!
	{
		itsByteOffset      = 0;
		itsCharacterOffset = 0;
		return false;
	}

	if (move)
	{
		itsByteOffset -= *byteCount;
		itsCharacterOffset--;
	}

	return true;
}

/******************************************************************************
 PrepareNext (private)

 ******************************************************************************/

bool
JStringIterator::PrepareNext
	(
	const bool	move,
	JSize*		byteCount
	)
{
	if (AtEnd())
	{
		return false;
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
		return false;
	}

	return true;
}
