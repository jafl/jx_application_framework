/******************************************************************************
 JStringIterator.h

	Copyright (C) 2016-17 by John Lindal.

 ******************************************************************************/

#ifndef _H_JStringIterator
#define _H_JStringIterator

#include <JString.h>

class JStringMatch;
class JRegex;
class JError;

class JStringIterator
{
public:

	JStringIterator(const JString& s,
					const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0);
	JStringIterator(JString* s,
					const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0);

	~JStringIterator();

	JBoolean	IsValid() const;
	JBoolean	AtBeginning() const;
	JBoolean	AtEnd() const;
	JIndex		GetPrevCharacterIndex() const;		// asserts
	JIndex		GetNextCharacterIndex() const;		// asserts
	JBoolean	GetPrevCharacterIndex(JIndex* i) const;
	JBoolean	GetNextCharacterIndex(JIndex* i) const;

	const JString&	GetString() const;

	// move

	void		MoveTo(const JIteratorPosition newPosition, const JIndex characterIndex);
	JBoolean	SkipPrev(const JSize characterCount = 1);
	JBoolean	SkipNext(const JSize characterCount = 1);

	void	UnsafeMoveTo(const JIteratorPosition newPosition,
						 const JIndex characterIndex, const JIndex byteIndex);

	// retrieve & move

	JBoolean	Prev(JUtf8Character* c, const JBoolean move = kJTrue);
	JBoolean	Next(JUtf8Character* c, const JBoolean move = kJTrue);

	// search & move

	JBoolean	Prev(const JString& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JUtf8Byte* str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JUtf8Byte* str, const JSize byteCount, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JUtf8Character& c, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const std::string& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JRegex& pattern);

	JBoolean	Next(const JString& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JUtf8Byte* str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JUtf8Byte* str, const JSize byteCount, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JUtf8Character& c, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const std::string& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JRegex& pattern);

	const JStringMatch&	GetLastMatch() const;	// asserts that match occurred

	// accumulate

	void				BeginMatch();
	const JStringMatch&	FinishMatch(const JBoolean ignoreLastMatch = kJTrue);

	// modify - only allowed if constructed from non-const JString*
	// (invalidates last match)

	JBoolean	SetPrev(const JUtf8Character& c, const JBoolean move = kJTrue);
	JBoolean	SetNext(const JUtf8Character& c, const JBoolean move = kJTrue);

	JBoolean	RemovePrev(const JSize characterCount = 1);
	JBoolean	RemoveNext(const JSize characterCount = 1);

	void	RemoveAllPrev();
	void	RemoveAllNext();

	void	RemoveLastMatch();
	void	ReplaceLastMatch(const JString& str, const JBoolean matchCase = kJFalse);
	void	ReplaceLastMatch(const JString& str, const JCharacterRange& range, const JBoolean matchCase = kJFalse);
	void	ReplaceLastMatch(const JUtf8Byte* str, const JBoolean matchCase = kJFalse);
	void	ReplaceLastMatch(const JUtf8Byte* str, const JSize byteCount, const JBoolean matchCase = kJFalse);
	void	ReplaceLastMatch(const JUtf8Byte* str, const JUtf8ByteRange& range, const JBoolean matchCase = kJFalse);
	void	ReplaceLastMatch(const JUtf8Character& c, const JBoolean matchCase = kJFalse);
	void	ReplaceLastMatch(const std::string& str, const JBoolean matchCase = kJFalse);
	void	ReplaceLastMatch(const std::string& str, const JUtf8ByteRange& range, const JBoolean matchCase = kJFalse);

	// after cursor position

	void	Insert(const JString& str);
	void	Insert(const JString& str, const JCharacterRange& range);
	void	Insert(const JUtf8Byte* str);
	void	Insert(const JUtf8Byte* str, const JSize byteCount);
	void	Insert(const JUtf8Byte* str, const JUtf8ByteRange& range);
	void	Insert(const JUtf8Character& c);
	void	Insert(const std::string& str);
	void	Insert(const std::string& str, const JUtf8ByteRange& range);

	// called by JString - public to avoid friendship

	void	Invalidate();

	// misc

	JIndex	GetPrevByteIndex() const;		// asserts
	JIndex	GetNextByteIndex() const;		// asserts

private:

	const JString*	itsConstString;			// NULL if invalidated
	JString*		itsString;				// NULL if we were passed a const object
	JCursorPosition	itsByteOffset;
	JCursorPosition	itsCharacterOffset;
	JStringMatch*	itsLastMatch;			// can be NULL
	JCursorPosition	itsMatchStartByte;		// set by BeginMatch()
	JCursorPosition	itsMatchStartChar;		// set by BeginMatch()

private:

	JBoolean	PreparePrev(const JBoolean move, JSize* byteCount);
	JBoolean	PrepareNext(const JBoolean move, JSize* byteCount);

	void	ClearLastMatch();

	// not allowed - cannot have 2 iterators for a string

	JStringIterator(const JStringIterator& source);
	const JStringIterator& operator=(const JStringIterator& source);
};


/******************************************************************************
 GetString

	*** Iterator must be valid.

 ******************************************************************************/

inline const JString&
JStringIterator::GetString()
	const
{
	return *itsConstString;
}

/******************************************************************************
 IsValid

	Return kJTrue if iterator has not been invalidated.

 ******************************************************************************/

inline JBoolean
JStringIterator::IsValid()
	const
{
	return JI2B( itsConstString != NULL );
}

/******************************************************************************
 AtBeginning

	Returns kJTrue if iterator is positioned at the beginning of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

inline JBoolean
JStringIterator::AtBeginning()
	const
{
	return JI2B( itsByteOffset == 0 );
}

/******************************************************************************
 AtEnd

	Returns kJTrue if iterator is positioned at the end of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

inline JBoolean
JStringIterator::AtEnd()
	const
{
	return JI2B( itsConstString == NULL ||
				 itsByteOffset >= itsConstString->GetByteCount() );
}

/******************************************************************************
 GetPrevCharacterIndex

	Returns kJTrue if there is a previous character.

 ******************************************************************************/

inline JBoolean
JStringIterator::GetPrevCharacterIndex
	(
	JIndex* i
	)
	const
{
	if (AtBeginning())
		{
		*i = 0;
		return kJFalse;
		}

	*i = itsCharacterOffset;
	return kJTrue;
}

/******************************************************************************
 GetNextCharacterIndex

	Returns kJTrue if there is a next character.

 ******************************************************************************/

inline JBoolean
JStringIterator::GetNextCharacterIndex
	(
	JIndex* i
	)
	const
{
	if (AtEnd())
		{
		*i = 0;
		return kJFalse;
		}

	*i = itsCharacterOffset + 1;
	return kJTrue;
}

/******************************************************************************
 Prev

 ******************************************************************************/

inline JBoolean
JStringIterator::Prev
	(
	const JString&	str,
	const JBoolean	caseSensitive
	)
{
	return Prev(str.GetRawBytes(), str.GetByteCount(), caseSensitive);
}

inline JBoolean
JStringIterator::Prev
	(
	const JUtf8Byte*	str,
	const JBoolean		caseSensitive
	)
{
	return Prev(str, strlen(str), caseSensitive);
}

inline JBoolean
JStringIterator::Prev
	(
	const JUtf8Character&	c,
	const JBoolean			caseSensitive
	)
{
	return Prev(c.GetBytes(), c.GetByteCount(), caseSensitive);
}

inline JBoolean
JStringIterator::Prev
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
{
	return Prev(str.data(), str.length(), caseSensitive);
}

/******************************************************************************
 Next

 ******************************************************************************/

inline JBoolean
JStringIterator::Next
	(
	const JString&	str,
	const JBoolean	caseSensitive
	)
{
	return Next(str.GetRawBytes(), str.GetByteCount(), caseSensitive);
}

inline JBoolean
JStringIterator::Next
	(
	const JUtf8Byte*	str,
	const JBoolean		caseSensitive
	)
{
	return Next(str, strlen(str), caseSensitive);
}

inline JBoolean
JStringIterator::Next
	(
	const JUtf8Character&	c,
	const JBoolean			caseSensitive
	)
{
	return Next(c.GetBytes(), c.GetByteCount(), caseSensitive);
}

inline JBoolean
JStringIterator::Next
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
{
	return Next(str.data(), str.length(), caseSensitive);
}

/******************************************************************************
 BeginMatch

	Marks the current position as the start of a match.

 ******************************************************************************/

inline void
JStringIterator::BeginMatch()
{
	ClearLastMatch();
	itsMatchStartByte = itsByteOffset;
	itsMatchStartChar = itsCharacterOffset;
}

/******************************************************************************
 RemoveLastMatch

	Removes the characters from the last match.

	*** Match must exist.
	*** Only allowed if iterator was constructed with non-const JString.

 ******************************************************************************/

inline void
JStringIterator::RemoveLastMatch()
{
	ReplaceLastMatch(NULL, JUtf8ByteRange());
}

/******************************************************************************
 ReplaceLastMatch

 ******************************************************************************/

inline void
JStringIterator::ReplaceLastMatch
	(
	const JString& str,
	const JBoolean matchCase
	)
{
	ReplaceLastMatch(str.GetRawBytes(), JUtf8ByteRange(1, str.GetByteCount()), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JString&			str,
	const JCharacterRange&	range,
	const JBoolean			matchCase
	)
{
	ReplaceLastMatch(str.GetRawBytes(), str.CharacterToUtf8ByteRange(range), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Byte*	str,
	const JBoolean		matchCase
	)
{
	ReplaceLastMatch(str, JUtf8ByteRange(1, strlen(str)), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		matchCase
	)
{
	ReplaceLastMatch(str, JUtf8ByteRange(1, byteCount), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Character&	c,
	const JBoolean			matchCase
	)
{
	ReplaceLastMatch(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const std::string&	str,
	const JBoolean		matchCase
	)
{
	ReplaceLastMatch(str.data(), JUtf8ByteRange(1, str.length()), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const JBoolean			matchCase
	)
{
	ReplaceLastMatch(str.data(), range, matchCase);
}

/******************************************************************************
 Insert

 ******************************************************************************/

inline void
JStringIterator::Insert
	(
	const JString& str
	)
{
	Insert(str.GetRawBytes(), JUtf8ByteRange(1, str.GetByteCount()));
}

inline void
JStringIterator::Insert
	(
	const JString&			str,
	const JCharacterRange&	range
	)
{
	Insert(str.GetRawBytes(), str.CharacterToUtf8ByteRange(range));
}

inline void
JStringIterator::Insert
	(
	const JUtf8Byte* str
	)
{
	Insert(str, JUtf8ByteRange(1, strlen(str)));
}

inline void
JStringIterator::Insert
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	Insert(str, JUtf8ByteRange(1, byteCount));
}

inline void
JStringIterator::Insert
	(
	const JUtf8Character& c
	)
{
	Insert(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()));
}

inline void
JStringIterator::Insert
	(
	const std::string& str
	)
{
	Insert(str.data(), JUtf8ByteRange(1, str.length()));
}

inline void
JStringIterator::Insert
	(
	const std::string&		str,
	const JUtf8ByteRange&	range
	)
{
	Insert(str.data(), range);
}

#endif
