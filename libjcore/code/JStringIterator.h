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
	JBoolean	RemoveLastMatch();

	void	ReplaceLastMatchPrev(const JString& str, const JBoolean move = kJTrue);
	void	ReplaceLastMatchPrev(const JString& str, const JCharacterRange& range, const JBoolean move = kJTrue);
	void	ReplaceLastMatchPrev(const JUtf8Byte* str, const JBoolean move = kJTrue);
	void	ReplaceLastMatchPrev(const JUtf8Byte* str, const JSize byteCount, const JBoolean move = kJTrue);
	void	ReplaceLastMatchPrev(const JUtf8Byte* str, const JUtf8ByteRange& range, const JBoolean move = kJTrue);
	void	ReplaceLastMatchPrev(const std::string& str, const JBoolean move = kJTrue);
	void	ReplaceLastMatchPrev(const std::string& str, const JUtf8ByteRange& range, const JBoolean move = kJTrue);

	void	ReplaceLastMatchNext(const JString& str, const JBoolean move = kJTrue);
	void	ReplaceLastMatchNext(const JString& str, const JCharacterRange& range, const JBoolean move = kJTrue);
	void	ReplaceLastMatchNext(const JUtf8Byte* str, const JBoolean move = kJTrue);
	void	ReplaceLastMatchNext(const JUtf8Byte* str, const JSize byteCount, const JBoolean move = kJTrue);
	void	ReplaceLastMatchNext(const JUtf8Byte* str, const JUtf8ByteRange& range, const JBoolean move = kJTrue);
	void	ReplaceLastMatchNext(const std::string& str, const JBoolean move = kJTrue);
	void	ReplaceLastMatchNext(const std::string& str, const JUtf8ByteRange& range, const JBoolean move = kJTrue);

	// called by JString

	void	Invalidate();

private:

	const JString*	itsConstString;			// NULL if invalidated
	JString*		itsString;				// NULL if we were passed a const object
	JCursorPosition	itsCursorPosition;		// bytes!

private:

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
 Invalidate

 ******************************************************************************/

inline void
JStringIterator::Invalidate()
{
	itsConstString    = NULL;
	itsString         = NULL;
	itsCursorPosition = 0;
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

#endif
