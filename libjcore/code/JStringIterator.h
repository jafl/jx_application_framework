/******************************************************************************
 JStringIterator.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStringIterator
#define _H_JStringIterator

#include <JString.h>

class JStringMatch;
class JRegex;

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

	// move

	void	MoveTo(const JIteratorPosition newPosition, const JIndex characterIndex);
	void	SkipPrev(const JSize characterCount = 1);
	void	SkipNext(const JSize characterCount = 1);

	// move & retrieve

	JBoolean	Prev(JUtf8Character* c, const JBoolean move = kJTrue);
	JBoolean	Next(JUtf8Character* c, const JBoolean move = kJTrue);

	// search

	JBoolean	Prev(const JString& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JUtf8Byte* str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JUtf8Byte* str, const JSize byteCount, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JUtf8Character& c, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const std::string& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Prev(const JRegex& pattern) const;

	JBoolean	Next(const JString& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JUtf8Byte* str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JUtf8Byte* str, const JSize byteCount, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JUtf8Character& c, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const std::string& str, const JBoolean caseSensitive = kJTrue);
	JBoolean	Next(const JRegex& pattern) const;

	const JStringMatch&	GetLastMatch() const;	// asserts that match occurred

	// modify - only allowed if constructed from non-const JString*
	// (invalidates last match)

	JBoolean	SetPrev(const JUtf8Character& data, const JBoolean move = kJTrue);
	JBoolean	SetNext(const JUtf8Character& data, const JBoolean move = kJTrue);

	JBoolean	RemovePrev(const JSize characterCount = 1);
	JBoolean	RemoveNext(const JSize characterCount = 1);

	void	RemoveLastMatch();
	void	ReplaceLastMatch(const JString& str);
	void	ReplaceLastMatch(const JString& str, const JCharacterRange& range);
	void	ReplaceLastMatch(const JUtf8Byte* str);
	void	ReplaceLastMatch(const JUtf8Byte* str, const JSize byteCount);
	void	ReplaceLastMatch(const JUtf8Byte* str, const JUtf8ByteRange& range);
	void	ReplaceLastMatch(const std::string& str);
	void	ReplaceLastMatch(const std::string& str, const JUtf8ByteRange& range);

	// called by JString

	void	Invalidate();

private:

	const JString*	itsConstString;			// NULL if invalidated
	JString*		itsString;				// NULL if we were passed a const object
	JCursorPosition	itsCursorPosition;		// bytes!
	JStringMatch*	itsLastMatch;			// can be NULL

private:

	void	ClearLastMatch();

	// not allowed - cannot have 2 iterators for a string

	JStringIterator(const JStringIterator& source);
	const JStringIterator& operator=(const JStringIterator& source);
};


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

	Return kJTrue if iterator is positioned at the beginning of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

inline JBoolean
JStringIterator::AtBeginning()
	const
{
	return JI2B( itsCursorPosition == 0 );
}

/******************************************************************************
 AtEnd

	Return kJTrue if iterator is positioned at the end of the string
	or if the iterator has been invalidated.

 ******************************************************************************/

inline JBoolean
JStringIterator::AtEnd()
	const
{
	return JI2B( itsConstString == NULL ||
				 itsCursorPosition >= itsConstString->GetByteCount() );
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
	return Prev(str.GetBytes(), str.GetByteCount(), caseSensitive);
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
	return Next(str.GetBytes(), str.GetByteCount(), caseSensitive);
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

#endif
