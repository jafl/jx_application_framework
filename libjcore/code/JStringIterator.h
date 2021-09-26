/******************************************************************************
 JStringIterator.h

	Copyright (C) 2016-17 by John Lindal.

 ******************************************************************************/

#ifndef _H_JStringIterator
#define _H_JStringIterator

#include "jx-af/jcore/JString.h"

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

	bool	IsValid() const;
	bool	AtBeginning() const;
	bool	AtEnd() const;
	JIndex	GetPrevCharacterIndex() const;		// asserts
	JIndex	GetNextCharacterIndex() const;		// asserts
	bool	GetPrevCharacterIndex(JIndex* i) const;
	bool	GetNextCharacterIndex(JIndex* i) const;

	const JString&	GetString() const;

	// move

	void	MoveTo(const JIteratorPosition newPosition, const JIndex characterIndex);
	bool	SkipPrev(const JSize characterCount = 1);
	bool	SkipNext(const JSize characterCount = 1);

	void	UnsafeMoveTo(const JIteratorPosition newPosition,
						 const JIndex characterIndex, const JIndex byteIndex);

	// retrieve & move

	bool	Prev(JUtf8Character* c, const JIteratorAction move = kJIteratorMove);
	bool	Next(JUtf8Character* c, const JIteratorAction move = kJIteratorMove);

	// search & move

	bool	Prev(const JString& str, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Prev(const JUtf8Byte* str, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Prev(const JUtf8Byte* str, const JSize byteCount, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Prev(const JUtf8Character& c, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Prev(const std::string& str, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Prev(const JRegex& pattern);

	bool	Next(const JString& str, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Next(const JUtf8Byte* str, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Next(const JUtf8Byte* str, const JSize byteCount, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Next(const JUtf8Character& c, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Next(const std::string& str, const JString::Case caseSensitive = JString::kCompareCase);
	bool	Next(const JRegex& pattern);

	const JStringMatch&	GetLastMatch() const;	// asserts that match occurred

	// accumulate

	void				BeginMatch();
	const JStringMatch&	FinishMatch(const bool includeLastMatch = false);

	// modify - only allowed if constructed from non-const JString*
	// (invalidates last match)

	bool	SetPrev(const JUtf8Character& c, const JIteratorAction move = kJIteratorMove);
	bool	SetNext(const JUtf8Character& c, const JIteratorAction move = kJIteratorMove);

	bool	RemovePrev(const JSize characterCount = 1);
	bool	RemoveNext(const JSize characterCount = 1);

	void	RemoveAllPrev();
	void	RemoveAllNext();

	void	RemoveLastMatch();
	void	ReplaceLastMatch(const JString& str, const bool matchCase = false);
	void	ReplaceLastMatch(const JString& str, const JCharacterRange& range, const bool matchCase = false);
	void	ReplaceLastMatch(const JUtf8Byte* str, const bool matchCase = false);
	void	ReplaceLastMatch(const JUtf8Byte* str, const JSize byteCount, const bool matchCase = false);
	void	ReplaceLastMatch(const JUtf8Byte* str, const JUtf8ByteRange& range, const bool matchCase = false);
	void	ReplaceLastMatch(const JUtf8Character& c, const bool matchCase = false);
	void	ReplaceLastMatch(const std::string& str, const bool matchCase = false);
	void	ReplaceLastMatch(const std::string& str, const JUtf8ByteRange& range, const bool matchCase = false);

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
	bool	GetPrevByteIndex(JIndex* i) const;
	bool	GetNextByteIndex(JIndex* i) const;

private:

	const JString*	itsConstString;			// nullptr if invalidated
	JString*		itsString;				// nullptr if we were passed a const object
	JCursorPosition	itsByteOffset;
	JCursorPosition	itsCharacterOffset;
	JStringMatch*	itsLastMatch;			// can be nullptr
	JCursorPosition	itsMatchStartByte;		// set by BeginMatch()
	JCursorPosition	itsMatchStartChar;		// set by BeginMatch()

private:

	bool	PreparePrev(const bool move, JSize* byteCount);
	bool	PrepareNext(const bool move, JSize* byteCount);

	void	ClearLastMatch();

	// not allowed - cannot have 2 iterators for a string

	JStringIterator(const JStringIterator&) = delete;
	JStringIterator& operator=(const JStringIterator&) = delete;
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

	Return true if iterator has not been invalidated.

 ******************************************************************************/

inline bool
JStringIterator::IsValid()
	const
{
	return itsConstString != nullptr;
}

/******************************************************************************
 AtBeginning

	Returns true if iterator is positioned at the beginning of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

inline bool
JStringIterator::AtBeginning()
	const
{
	return itsByteOffset == 0;
}

/******************************************************************************
 AtEnd

	Returns true if iterator is positioned at the end of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

inline bool
JStringIterator::AtEnd()
	const
{
	return itsConstString == nullptr ||
				 itsByteOffset >= itsConstString->GetByteCount();
}

/******************************************************************************
 GetPrevCharacterIndex

	Returns true if there is a previous character.

 ******************************************************************************/

inline bool
JStringIterator::GetPrevCharacterIndex
	(
	JIndex* i
	)
	const
{
	if (AtBeginning())
		{
		*i = 0;
		return false;
		}

	*i = itsCharacterOffset;
	return true;
}

/******************************************************************************
 GetNextCharacterIndex

	Returns true if there is a next character.

 ******************************************************************************/

inline bool
JStringIterator::GetNextCharacterIndex
	(
	JIndex* i
	)
	const
{
	if (AtEnd())
		{
		*i = 0;
		return false;
		}

	*i = itsCharacterOffset + 1;
	return true;
}

/******************************************************************************
 GetPrevByteIndex

	Returns true if there is a previous character.

 ******************************************************************************/

inline bool
JStringIterator::GetPrevByteIndex
	(
	JIndex* i
	)
	const
{
	if (AtBeginning())
		{
		*i = 0;
		return false;
		}

	*i = itsByteOffset;
	return true;
}

/******************************************************************************
 GetNextByteIndex

	Returns true if there is a next character.

 ******************************************************************************/

inline bool
JStringIterator::GetNextByteIndex
	(
	JIndex* i
	)
	const
{
	if (AtEnd())
		{
		*i = 0;
		return false;
		}

	*i = itsByteOffset + 1;
	return true;
}

/******************************************************************************
 Prev

 ******************************************************************************/

inline bool
JStringIterator::Prev
	(
	const JString&		str,
	const JString::Case	caseSensitive
	)
{
	return Prev(str.GetRawBytes(), str.GetByteCount(), caseSensitive);
}

inline bool
JStringIterator::Prev
	(
	const JUtf8Byte*	str,
	const JString::Case	caseSensitive
	)
{
	return Prev(str, strlen(str), caseSensitive);
}

inline bool
JStringIterator::Prev
	(
	const JUtf8Character&	c,
	const JString::Case		caseSensitive
	)
{
	return Prev(c.GetBytes(), c.GetByteCount(), caseSensitive);
}

inline bool
JStringIterator::Prev
	(
	const std::string&	str,
	const JString::Case	caseSensitive
	)
{
	return Prev(str.data(), str.length(), caseSensitive);
}

/******************************************************************************
 Next

 ******************************************************************************/

inline bool
JStringIterator::Next
	(
	const JString&		str,
	const JString::Case	caseSensitive
	)
{
	return Next(str.GetRawBytes(), str.GetByteCount(), caseSensitive);
}

inline bool
JStringIterator::Next
	(
	const JUtf8Byte*	str,
	const JString::Case	caseSensitive
	)
{
	return Next(str, strlen(str), caseSensitive);
}

inline bool
JStringIterator::Next
	(
	const JUtf8Character&	c,
	const JString::Case		caseSensitive
	)
{
	return Next(c.GetBytes(), c.GetByteCount(), caseSensitive);
}

inline bool
JStringIterator::Next
	(
	const std::string&	str,
	const JString::Case	caseSensitive
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
	ReplaceLastMatch(nullptr, JUtf8ByteRange());
}

/******************************************************************************
 ReplaceLastMatch

 ******************************************************************************/

inline void
JStringIterator::ReplaceLastMatch
	(
	const JString& str,
	const bool matchCase
	)
{
	ReplaceLastMatch(str.GetRawBytes(), JUtf8ByteRange(1, str.GetByteCount()), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JString&			str,
	const JCharacterRange&	range,
	const bool			matchCase
	)
{
	ReplaceLastMatch(str.GetRawBytes(), str.CharacterToUtf8ByteRange(range), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Byte*	str,
	const bool		matchCase
	)
{
	ReplaceLastMatch(str, JUtf8ByteRange(1, strlen(str)), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const bool		matchCase
	)
{
	ReplaceLastMatch(str, JUtf8ByteRange(1, byteCount), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const JUtf8Character&	c,
	const bool			matchCase
	)
{
	ReplaceLastMatch(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const std::string&	str,
	const bool		matchCase
	)
{
	ReplaceLastMatch(str.data(), JUtf8ByteRange(1, str.length()), matchCase);
}

inline void
JStringIterator::ReplaceLastMatch
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const bool			matchCase
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
