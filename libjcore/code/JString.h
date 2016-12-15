/******************************************************************************
 JString.h

	Interface for the JString class

	Copyright (C) 1994-2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JString
#define _H_JString

#include <JUtf8Character.h>
#include <JUtf8ByteRange.h>
#include <JCharacterRange.h>
#include <string.h>
#include <unicode/ucasemap.h>

class JStringIterator;

class JString
{
	friend class JStringIterator;
	friend istream& operator>>(istream&, JString&);
	friend ostream& operator<<(ostream&, const JString&);

public:

	enum
	{
		kPrecisionAsNeeded = -1,
		kDefSigDigitCount  = 16
	};

	enum ExponentDisplay
	{
		kStandardExponent = -10000,
		kForceExponent    = -9999,
		kForceNoExponent  = 0,
		kUseGivenExponent = 1
	};

	enum Base	// sorry, have to use this to make ctors distinct
	{
		kBase10 = 10,
		kBase2  = 2,
		kBase8  = 8,
		kBase16 = 16
	};

public:

	JString();
	JString(const JString& str);
	JString(const JString& str, const JCharacterRange& range);
	JString(const JUtf8Byte* str);
	JString(const JUtf8Byte* str, const JSize byteCount);
	JString(const JUtf8Byte* str, const JUtf8ByteRange& range);
	JString(const std::string& str);
	JString(const std::string& str, const JUtf8ByteRange& range);
	JString(const JUtf8Character& c);

	JString(const JUInt64 number, const Base base, const JBoolean pad = kJFalse);
	JString(const JFloat number, const JInteger precision = kPrecisionAsNeeded,
			const ExponentDisplay expDisplay = kStandardExponent,
			const JInteger exponent = 0, const JInteger sigDigitCount = 0);

	~JString();

	const JString& operator=(const JString& str);
	const JString& operator=(const JUtf8Byte* str);
	const JString& operator=(const std::string& str);
	const JString& operator=(const JUtf8Character& c);

	JString& operator+=(const JString& str);
	JString& operator+=(const JUtf8Byte* str);
	JString& operator+=(const std::string& str);
	JString& operator+=(const JUtf8Character& c);

	void	Set(const JString& str);
	void	Set(const JString& str, const JCharacterRange& range);
	void	Set(const JUtf8Byte* str);
	void	Set(const JUtf8Byte* str, const JSize byteCount);
	void	Set(const JUtf8Byte* str, const JUtf8ByteRange& range);
	void	Set(const std::string& str);
	void	Set(const std::string& str, const JUtf8ByteRange& range);
	void	Set(const JUtf8Character& str);

	void	Clear();

	// no cast operator, to force everybody to remember it is UTF-8, not char*

	JBoolean	IsEmpty() const;
	JSize		GetCharacterCount() const;
	JSize		GetByteCount() const;
	JBoolean	CharacterIndexValid(const JIndex index) const;
	JBoolean	ByteIndexValid(const JIndex index) const;
	JBoolean	RangeValid(const JCharacterRange& range) const;
	JBoolean	RangeValid(const JUtf8ByteRange& range) const;

	JUtf8Character	GetFirstCharacter() const;
	JUtf8Character	GetLastCharacter() const;

	const JUtf8Byte*	GetBytes() const;
	JUtf8Byte*			AllocateBytes() const;	// client must call delete [] when finished with it

	JBoolean	BeginsWith(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JString& str, const JCharacterRange& range, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JUtf8Byte* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JUtf8Byte* str, const JSize byteCount, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JUtf8Byte* str, const JUtf8ByteRange& range, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JUtf8Character& c, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const std::string& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const std::string& str, const JUtf8ByteRange& range, const JBoolean caseSensitive = kJTrue) const;

	JBoolean	Contains(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JString& str, const JCharacterRange& range, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JUtf8Byte* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JUtf8Byte* str, const JSize byteCount, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JUtf8Byte* str, const JUtf8ByteRange& range, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JUtf8Character& c, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const std::string& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const std::string& str, const JUtf8ByteRange& range, const JBoolean caseSensitive = kJTrue) const;

	JBoolean	EndsWith(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JString& str, const JCharacterRange& range, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JUtf8Byte* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JUtf8Byte* str, const JSize byteCount, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JUtf8Byte* str, const JUtf8ByteRange& range, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JUtf8Character& c, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const std::string& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const std::string& str, const JUtf8ByteRange& range, const JBoolean caseSensitive = kJTrue) const;

	JString		EncodeBase64() const;
	JBoolean	DecodeBase64(JString* str) const;

	void		Read(istream& input, const JSize count);
	void		ReadDelimited(istream& input);
	void		Print(ostream& output) const;
	void		PrintHex(ostream& output) const;

	JSize		GetBlockSize() const;
	void		SetBlockSize(const JSize blockSize);

	// modify - do not require an iterator

	void	Prepend(const JString& str);
	void	Prepend(const JUtf8Byte* str);
	void	Prepend(const JUtf8Byte* str, const JSize byteCount);
	void	Prepend(const std::string& str);
	void	Prepend(const JUtf8Character& c);

	void	Append(const JString& str);
	void	Append(const JUtf8Byte* str);
	void	Append(const JUtf8Byte* str, const JSize byteCount);
	void	Append(const std::string& str);
	void	Append(const JUtf8Character& c);

	void	TrimWhitespace();
	void	ToLower();
	void	ToUpper();

	JBoolean	MatchCase(const JString& source, const JCharacterRange& sourceRange);
	JBoolean	MatchCase(const JUtf8Byte* source, const JUtf8ByteRange& sourceRange);
	JBoolean	MatchCase(const std::string& source, const JUtf8ByteRange& sourceRange);
	JBoolean	MatchCase(const JString& source, const JCharacterRange& sourceRange,
						  const JCharacterRange& destRange);
	JBoolean	MatchCase(const JUtf8Byte* source, const JUtf8ByteRange& sourceRange,
						  const JCharacterRange& destRange);
	JBoolean	MatchCase(const std::string& source, const JUtf8ByteRange& sourceRange,
						  const JCharacterRange& destRange);

	// number <-> string

	JBoolean	IsFloat() const;
	JBoolean	ConvertToFloat(JFloat* value) const;

	JBoolean	IsInteger(const JSize base = 10) const;
	JBoolean	ConvertToInteger(JInteger* value, const JSize base = 10) const;

	JBoolean	IsUInt(const JSize base = 10) const;
	JBoolean	IsHexValue() const;
	JBoolean	ConvertToUInt(JUInt* value, const JSize base = 10) const;

	static JBoolean	IsFloat(const JUtf8Byte* str);
	static JBoolean	IsFloat(const JUtf8Byte* str, const JSize byteCount);
	static JBoolean	ConvertToFloat(const JUtf8Byte* str, JFloat* value);
	static JBoolean	ConvertToFloat(const JUtf8Byte* str, const JSize byteCount,
								   JFloat* value);

	static JBoolean	IsInteger(const JUtf8Byte* str, const JSize base = 10);
	static JBoolean	IsInteger(const JUtf8Byte* str, const JSize byteCount, const JSize base);
	static JBoolean	ConvertToInteger(const JUtf8Byte* str,
									 JInteger* value, const JSize base = 10);
	static JBoolean	ConvertToInteger(const JUtf8Byte* str, const JSize byteCount,
									 JInteger* value, const JSize base = 10);

	static JBoolean	IsUInt(const JUtf8Byte* str, const JSize base = 10);
	static JBoolean	IsUInt(const JUtf8Byte* str, const JSize byteCount, const JSize base);
	static JBoolean	IsHexValue(const JUtf8Byte* str);
	static JBoolean	IsHexValue(const JUtf8Byte* str, const JSize byteCount);
	static JBoolean	ConvertToUInt(const JUtf8Byte* str,
								  JUInt* value, const JSize base = 10);
	static JBoolean	ConvertToUInt(const JUtf8Byte* str, const JSize byteCount,
								  JUInt* value, const JSize base = 10);

	// utility functions

	static JBoolean	IsEmpty(const JUtf8Byte* s);
	static JBoolean	IsEmpty(const JString* s);

	static JBoolean			IsValid(const JUtf8Byte* s);
	static JBoolean			IsValid(const JUtf8Byte* s, const JUtf8ByteRange& range);
	static JSize			CountCharacters(const JUtf8Byte* str);
	static JSize			CountCharacters(const JUtf8Byte* str, const JSize byteCount);
	static JSize			CountCharacters(const JUtf8Byte* str, const JUtf8ByteRange& range);
	static JUtf8ByteRange	CharacterToUtf8ByteRange(const JUtf8Byte* str, const JCharacterRange& range);

	static int Compare(const JString& s1, const JString& s2, const JBoolean caseSensitive = kJTrue);
	static int Compare(const JString& s1, const JUtf8Byte* s2, const JBoolean caseSensitive = kJTrue);
	static int Compare(const JUtf8Byte* s1, const JString& s2, const JBoolean caseSensitive = kJTrue);
	static int Compare(const JUtf8Byte* s1, const JUtf8Byte* s2, const JBoolean caseSensitive = kJTrue);
	static int Compare(const JString& s1, const std::string& s2, const JBoolean caseSensitive = kJTrue);
	static int Compare(const std::string& s1, const JString& s2, const JBoolean caseSensitive = kJTrue);
	static int Compare(const JUtf8Byte* s1, const JSize byteCount1,
					   const JUtf8Byte* s2, const JSize byteCount2,
					   const JBoolean caseSensitive = kJTrue);

	static int	CompareMaxNBytes(const JUtf8Byte* s1, const JUtf8Byte* s2, const JSize N,
								 const JBoolean caseSensitive = kJTrue);

	static JSize	CalcCharacterMatchLength(const JString& s1, const JString& s2,
											 const JBoolean caseSensitive = kJTrue);

	static JSize	Normalize(const JUtf8Byte* source, const JSize byteCount,
							  JUtf8Byte** destination);
	static JSize	CopyNormalizedBytes(const JUtf8Byte* source, const JSize maxBytes,
										JUtf8Byte* destination, const JSize capacity);

	static JSize	GetDefaultBlockSize();
	static void		SetDefaultBlockSize(const JSize blockSize);

protected:

	void	SetIterator(JStringIterator* iter) const;	// conceptually const

	JUtf8ByteRange	CharacterToUtf8ByteRange(const JCharacterRange& range) const;

	JBoolean	SearchForward(const JUtf8Byte* str, const JSize byteCount,
							  const JBoolean caseSensitive, JIndex* startIndex) const;
	JBoolean	SearchBackward(const JUtf8Byte* str, const JSize byteCount,
							   const JBoolean caseSensitive, JIndex* startIndex) const;

	void	ReplaceBytes(const JUtf8ByteRange& replaceRange,
						 const JUtf8Byte* stringToInsert, const JSize insertByteCount);

	JBoolean	MatchCase(const JUtf8Byte* source, const JUtf8ByteRange& sourceRange,
						  const JUtf8ByteRange& destRange);

private:

	JUtf8Byte*	itsBytes;			// characters
	JSize		itsByteCount;		// number of bytes used
	JSize		itsCharacterCount;	// number of characters
	JSize		itsAllocCount;		// number of bytes we have space for
	JSize		itsBlockSize;		// size by which to shrink and grow allocation

	UCaseMap*			itsUCaseMap;
	JStringIterator*	itsIterator;	// only one iterator allowed at a time

	static JSize theDefaultBlockSize;

private:

	void	CopyToPrivateString(const JUtf8Byte* str, const JSize byteCount);
	void	FoldCase(const JBoolean upper);

	static JBoolean	CompleteConversion(const JUtf8Byte* startPtr, const JSize byteCount,
									   const JUtf8Byte* convEndPtr);
};


/******************************************************************************
 GetBytes

 ******************************************************************************/

inline const JUtf8Byte*
JString::GetBytes()
	const
{
	return itsBytes;
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

inline JBoolean
JString::IsEmpty()
	const
{
	return JI2B( itsByteCount == 0 );
}

/******************************************************************************
 JString::IsEmpty

 ******************************************************************************/

inline JBoolean
JString::IsEmpty
	(
	const JUtf8Byte* s
	)
{
	return JI2B( s == NULL || s[0] == '\0' );
}

inline JBoolean
JString::IsEmpty
	(
	const JString* s
	)
{
	return JI2B( s == NULL || s->IsEmpty() );
}

/******************************************************************************
 GetCharacterCount

 ******************************************************************************/

inline JSize
JString::GetCharacterCount()
	const
{
	return itsCharacterCount;
}

/******************************************************************************
 GetByteCount

 ******************************************************************************/

inline JSize
JString::GetByteCount()
	const
{
	return itsByteCount;
}

/******************************************************************************
 CharacterIndexValid

 ******************************************************************************/

inline JBoolean
JString::CharacterIndexValid
	(
	const JIndex index
	)
	const
{
	return JI2B( 1 <= index && index <= itsCharacterCount );
}

/******************************************************************************
 ByteIndexValid

 ******************************************************************************/

inline JBoolean
JString::ByteIndexValid
	(
	const JIndex index
	)
	const
{
	return JI2B( 1 <= index && index <= itsByteCount );
}

/******************************************************************************
 RangeValid

 ******************************************************************************/

inline JBoolean
JString::RangeValid
	(
	const JCharacterRange& range
	)
	const
{
	return JI2B(CharacterIndexValid(range.first) &&
				(range.IsEmpty() || CharacterIndexValid(range.last)));
}

inline JBoolean
JString::RangeValid
	(
	const JUtf8ByteRange& range
	)
	const
{
	return JI2B(ByteIndexValid(range.first) &&
				(range.IsEmpty() || ByteIndexValid(range.last)));
}

/******************************************************************************
 CharacterToUtf8ByteRange (protected)

 ******************************************************************************/

inline JUtf8ByteRange
JString::CharacterToUtf8ByteRange
	(
	const JCharacterRange& range
	)
	const
{
	return CharacterToUtf8ByteRange(itsBytes, range);
}

/******************************************************************************
 IsValid

 ******************************************************************************/

inline JBoolean
JString::IsValid
	(
	const JUtf8Byte* str
	)
{
	return IsValid(str, JUtf8ByteRange(1, strlen(str)));
}

/******************************************************************************
 CountCharacters (static)

 ******************************************************************************/

inline JSize
JString::CountCharacters
	(
	const JUtf8Byte* str
	)
{
	return CountCharacters(str, JUtf8ByteRange(1, strlen(str)));
}

inline JSize
JString::CountCharacters
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	return CountCharacters(str, JUtf8ByteRange(1, byteCount));
}

/******************************************************************************
 BeginsWith

 ******************************************************************************/

inline JBoolean
JString::BeginsWith
	(
	const JString&	str,
	const JBoolean	caseSensitive
	)
	const
{
	return BeginsWith(str.itsBytes, JUtf8ByteRange(1, str.itsByteCount), caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const JString&			str,
	const JCharacterRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	return BeginsWith(str.itsBytes + range.first-1, str.CharacterToUtf8ByteRange(range), caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const JUtf8Byte*	str,
	const JBoolean		caseSensitive
	)
	const
{
	return BeginsWith(str, JUtf8ByteRange(1, strlen(str)), caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		caseSensitive
	)
	const
{
	return BeginsWith(str, JUtf8ByteRange(1, byteCount), caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const JUtf8Character&	c,
	const JBoolean			caseSensitive
	)
	const
{
	return BeginsWith(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
	const
{
	return BeginsWith(str.data(), JUtf8ByteRange(1, str.length()), caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	return BeginsWith(str.data(), range, caseSensitive);
}

/******************************************************************************
 Contains

 ******************************************************************************/

inline JBoolean
JString::Contains
	(
	const JString&	str,
	const JBoolean	caseSensitive
	)
	const
{
	return Contains(str.itsBytes, JUtf8ByteRange(1, str.itsByteCount), caseSensitive);
}

inline JBoolean
JString::Contains
	(
	const JString&			str,
	const JCharacterRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	return Contains(str.itsBytes + range.first-1, str.CharacterToUtf8ByteRange(range), caseSensitive);
}

inline JBoolean
JString::Contains
	(
	const JUtf8Byte*	str,
	const JBoolean		caseSensitive
	)
	const
{
	return Contains(str, JUtf8ByteRange(1, strlen(str)), caseSensitive);
}

inline JBoolean
JString::Contains
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		caseSensitive
	)
	const
{
	return Contains(str, JUtf8ByteRange(1, byteCount), caseSensitive);
}

inline JBoolean
JString::Contains
	(
	const JUtf8Character&	c,
	const JBoolean			caseSensitive
	)
	const
{
	return Contains(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), caseSensitive);
}

inline JBoolean
JString::Contains
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
	const
{
	return Contains(str.data(), JUtf8ByteRange(1, str.length()), caseSensitive);
}

inline JBoolean
JString::Contains
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	return Contains(str.data(), range, caseSensitive);
}

/******************************************************************************
 EndsWith

 ******************************************************************************/

inline JBoolean
JString::EndsWith
	(
	const JString&	str,
	const JBoolean	caseSensitive
	)
	const
{
	return EndsWith(str.itsBytes, JUtf8ByteRange(1, str.itsByteCount), caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const JString&			str,
	const JCharacterRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	return EndsWith(str.itsBytes + range.first-1, str.CharacterToUtf8ByteRange(range), caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const JUtf8Byte*	str,
	const JBoolean		caseSensitive
	)
	const
{
	return EndsWith(str, JUtf8ByteRange(1, strlen(str)), caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		caseSensitive
	)
	const
{
	return EndsWith(str, JUtf8ByteRange(1, byteCount), caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const JUtf8Character&	c,
	const JBoolean			caseSensitive
	)
	const
{
	return EndsWith(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
	const
{
	return EndsWith(str.data(), JUtf8ByteRange(1, str.length()), caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	return EndsWith(str.data(), range, caseSensitive);
}

/******************************************************************************
 MatchCase

 ******************************************************************************/

inline JBoolean
JString::MatchCase
	(
	const JString&			source,
	const JCharacterRange&	sourceRange
	)
{
	return MatchCase(source.itsBytes, source.CharacterToUtf8ByteRange(sourceRange),
					 JUtf8ByteRange(1, itsByteCount));
}

inline JBoolean
JString::MatchCase
	(
	const JString&			source,
	const JCharacterRange&	sourceRange,
	const JCharacterRange&	destRange
	)
{
	return MatchCase(source.itsBytes, source.CharacterToUtf8ByteRange(sourceRange),
					 CharacterToUtf8ByteRange(destRange));
}

inline JBoolean
JString::MatchCase
	(
	const JUtf8Byte*		source,
	const JUtf8ByteRange&	sourceRange
	)
{
	return MatchCase(source, sourceRange,
					 JUtf8ByteRange(1, itsByteCount));
}

inline JBoolean
JString::MatchCase
	(
	const JUtf8Byte*		source,
	const JUtf8ByteRange&	sourceRange,
	const JCharacterRange&	destRange
	)
{
	return MatchCase(source, sourceRange,
					 CharacterToUtf8ByteRange(destRange));
}

inline JBoolean
JString::MatchCase
	(
	const std::string&		source,
	const JUtf8ByteRange&	sourceRange
	)
{
	return MatchCase(source.data(), sourceRange,
					 JUtf8ByteRange(1, itsByteCount));
}

inline JBoolean
JString::MatchCase
	(
	const std::string&		source,
	const JUtf8ByteRange&	sourceRange,
	const JCharacterRange&	destRange
	)
{
	return MatchCase(source.data(), sourceRange,
					 CharacterToUtf8ByteRange(destRange));
}

/******************************************************************************
 Prepend

 ******************************************************************************/

inline void
JString::Prepend
	(
	const JString& str
	)
{
	Prepend(str.itsBytes, str.itsByteCount);
}

inline void
JString::Prepend
	(
	const JUtf8Byte* str
	)
{
	Prepend(str, strlen(str));
}

inline void
JString::Prepend
	(
	const std::string& str
	)
{
	Prepend(str.data(), str.length());
}

inline void
JString::Prepend
	(
	const JUtf8Character& c
	)
{
	Prepend(c.GetBytes(), c.GetByteCount());
}

/******************************************************************************
 Append

 ******************************************************************************/

inline void
JString::Append
	(
	const JString& str
	)
{
	Append(str.itsBytes, str.itsByteCount);
}

inline void
JString::Append
	(
	const JUtf8Byte* str
	)
{
	Append(str, strlen(str));
}

inline void
JString::Append
	(
	const std::string& str
	)
{
	Append(str.data(), str.length());
}

inline void
JString::Append
	(
	const JUtf8Character& c
	)
{
	Append(c.GetBytes(), c.GetByteCount());
}

/******************************************************************************
 Concatenation

 ******************************************************************************/

inline JString&
JString::operator+=
	(
	const JString& str
	)
{
	Append(str.itsBytes, str.itsByteCount);
	return *this;
}

inline JString&
JString::operator+=
	(
	const JUtf8Byte* str
	)
{
	Append(str, strlen(str));
	return *this;
}

inline JString&
JString::operator+=
	(
	const std::string& str
	)
{
	Append(str.data(), str.length());
	return *this;
}

inline JString&
JString::operator+=
	(
	const JUtf8Character& c
	)
{
	Append(c.GetBytes(), c.GetByteCount());
	return *this;
}

/******************************************************************************
 Addition

 ******************************************************************************/

inline JString
operator+
	(
	const JString& s1,
	const JString& s2
	)
{
	JString sum = s1;
	sum += s2;
	return sum;
}

inline JString
operator+
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	JString sum = s;
	sum += str;
	return sum;
}

inline JString
operator+
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	JString sum(str, strlen(str));
	sum += s;
	return sum;
}

inline JString
operator+
	(
	const JString&		s,
	const std::string&	str
	)
{
	JString sum = s;
	sum += str;
	return sum;
}

inline JString
operator+
	(
	const std::string&	str,
	const JString&		s
	)
{
	JString sum;
	sum.Set(str);
	sum += s;
	return sum;
}

/******************************************************************************
 Equality (case-sensitive)

	*** We have to use the version of JString::Compare() that takes byte counts
		because JStrings can contain NULLs.

 ******************************************************************************/

// operator==

inline int
operator==
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.GetBytes(), s2.GetByteCount(), kJTrue) == 0);
}

inline int
operator==
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJTrue) == 0);
}

inline int
operator==
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJTrue) == 0);
}

inline int
operator==
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJTrue) == 0);
}

inline int
operator==
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJTrue) == 0);
}

// operator!=

inline int
operator!=
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.GetBytes(), s2.GetByteCount(), kJTrue) != 0);
}

inline int
operator!=
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJTrue) != 0);
}

inline int
operator!=
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJTrue) != 0);
}

inline int
operator!=
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJTrue) != 0);
}

inline int
operator!=
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJTrue) != 0);
}

/******************************************************************************
 Comparison (case-insensitive)

	*** We have to use the version of JString::Compare() that takes byte counts
		because JStrings can contain NULLs.

 ******************************************************************************/

// operator<

inline int
operator<
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.GetBytes(), s2.GetByteCount(), kJFalse) < 0);
}

inline int
operator<
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) < 0);
}

inline int
operator<
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) < 0);
}

inline int
operator<
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) < 0);
}

inline int
operator<
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) < 0);
}

// operator<=

inline int
operator<=
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.GetBytes(), s2.GetByteCount(), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) <= 0);
}

inline int
operator<=
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) <= 0);
}

// operator>


inline int
operator>
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.GetBytes(), s2.GetByteCount(), kJFalse) > 0);
}

inline int
operator>
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) > 0);
}

inline int
operator>
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) > 0);
}

inline int
operator>
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) > 0);
}

inline int
operator>
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) > 0);
}

// operator>=

inline int
operator>=
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.GetBytes(), s2.GetByteCount(), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str, strlen(str), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) >= 0);
}

inline int
operator>=
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetBytes(), s.GetByteCount(), str.data(), str.length(), kJFalse) >= 0);
}

/******************************************************************************
 Compare (static)

	Replaces strcmp(): + if s1>s2, 0 if s1==s2, - if s1<s2

	*** We have to use the version of Compare() that takes byte counts
		because JStrings can contain NULLs.

 ******************************************************************************/

inline int
JString::Compare
	(
	const JString&	s1,
	const JString&	s2,
	const JBoolean	caseSensitive
	)
{
	return JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.GetBytes(), s2.GetByteCount(), caseSensitive);
}

inline int
JString::Compare
	(
	const JString&		s1,
	const JUtf8Byte*	s2,
	const JBoolean		caseSensitive
	)
{
	return JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2, strlen(s2), caseSensitive);
}

inline int
JString::Compare
	(
	const JUtf8Byte*	s1,
	const JString&		s2,
	const JBoolean		caseSensitive
	)
{
	return JString::Compare(s1, strlen(s1), s2.GetBytes(), s2.GetByteCount(), caseSensitive);
}

inline int
JString::Compare
	(
	const JUtf8Byte*	s1,
	const JUtf8Byte*	s2,
	const JBoolean		caseSensitive
	)
{
	return Compare(s1, strlen(s1), s2, strlen(s2), caseSensitive);
}

inline int
JString::Compare
	(
	const JString&		s1,
	const std::string&	s2,
	const JBoolean		caseSensitive
	)
{
	return JString::Compare(s1.GetBytes(), s1.GetByteCount(), s2.data(), s2.length(), caseSensitive);
}

inline int
JString::Compare
	(
	const std::string&	s1,
	const JString&		s2,
	const JBoolean		caseSensitive
	)
{
	return JString::Compare(s1.data(), s1.length(), s2.GetBytes(), s2.GetByteCount(), caseSensitive);
}

/******************************************************************************
 Set

 ******************************************************************************/

inline void
JString::Set
	(
	const JString& str
	)
{
	if (this->itsBytes != str.itsBytes)
		{
		CopyToPrivateString(str.itsBytes, str.itsByteCount);
		}
}

inline void
JString::Set
	(
	const JString&			str,
	const JCharacterRange&	charRange
	)
{
	if (this->itsBytes != str.itsBytes)
		{
		const JUtf8ByteRange byteRange = CharacterToUtf8ByteRange(charRange);
		CopyToPrivateString(str.itsBytes + byteRange.first-1, byteRange.GetCount());
		}
}

inline void
JString::Set
	(
	const JUtf8Byte* str
	)
{
	CopyToPrivateString(str, strlen(str));
}

inline void
JString::Set
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	CopyToPrivateString(str, byteCount);
}

inline void
JString::Set
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range
	)
{
	if (range.IsNothing())
		{
		Clear();
		}
	else
		{
		CopyToPrivateString(str + range.first-1, range.GetCount());
		}
}

inline void
JString::Set
	(
	const std::string& str
	)
{
	CopyToPrivateString(str.data(), str.length());
}

inline void
JString::Set
	(
	const std::string&		str,
	const JUtf8ByteRange&	range
	)
{
	if (range.IsNothing())
		{
		Clear();
		}
	else
		{
		CopyToPrivateString(str.data() + range.first-1, range.GetCount());
		}
}

inline void
JString::Set
	(
	const JUtf8Character& c
	)
{
	CopyToPrivateString(c.GetBytes(), c.GetByteCount());
}

/******************************************************************************
 Assignment operator

	We do not copy itsBlockSize because we assume the client has set it
	appropriately.

 ******************************************************************************/

inline const JString&
JString::operator=
	(
	const JString& source
	)
{
	Set(source);
	return *this;
}

inline const JString&
JString::operator=
	(
	const JUtf8Byte* str
	)
{
	Set(str);
	return *this;
}

inline const JString&
JString::operator=
	(
	const std::string& str
	)
{
	Set(str);
	return *this;
}

inline const JString&
JString::operator=
	(
	const JUtf8Character& c
	)
{
	Set(c);
	return *this;
}

/******************************************************************************
 Is number

	Returns kJTrue if we can convert ourselves to a number.

 ******************************************************************************/

inline JBoolean
JString::IsFloat()
	const
{
	JFloat value;
	return ConvertToFloat(itsBytes, itsByteCount, &value);
}

inline JBoolean
JString::IsInteger
	(
	const JSize base
	)
	const
{
	JInteger value;
	return ConvertToInteger(itsBytes, itsByteCount, &value, base);
}

inline JBoolean
JString::IsUInt
	(
	const JSize base
	)
	const
{
	JUInt value;
	return ConvertToUInt(itsBytes, itsByteCount, &value, base);
}

inline JBoolean
JString::IsHexValue()
	const
{
	return IsHexValue(itsBytes, itsByteCount);
}

// static

inline JBoolean
JString::IsFloat
	(
	const JUtf8Byte* str
	)
{
	JFloat value;
	return ConvertToFloat(str, strlen(str), &value);
}

inline JBoolean
JString::IsFloat
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	JFloat value;
	return ConvertToFloat(str, byteCount, &value);
}

inline JBoolean
JString::IsInteger
	(
	const JUtf8Byte*	str,
	const JSize			base
	)
{
	JInteger value;
	return ConvertToInteger(str, strlen(str), &value, base);
}

inline JBoolean
JString::IsInteger
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JSize			base
	)
{
	JInteger value;
	return ConvertToInteger(str, byteCount, &value, base);
}

inline JBoolean
JString::IsUInt
	(
	const JUtf8Byte*	str,
	const JSize			base
	)
{
	JUInt value;
	return ConvertToUInt(str, strlen(str), &value, base);
}

inline JBoolean
JString::IsUInt
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JSize			base
	)
{
	JUInt value;
	return ConvertToUInt(str, byteCount, &value, base);
}

inline JBoolean
JString::IsHexValue
	(
	const JUtf8Byte* str
	)
{
	return IsHexValue(str, strlen(str));
}

/******************************************************************************
 Convert to number

 ******************************************************************************/

inline JBoolean
JString::ConvertToFloat
	(
	JFloat* value
	)
	const
{
	return ConvertToFloat(itsBytes, itsByteCount, value);
}

inline JBoolean
JString::ConvertToInteger
	(
	JInteger*	value,
	const JSize	base
	)
	const
{
	return ConvertToInteger(itsBytes, itsByteCount, value, base);
}

inline JBoolean
JString::ConvertToUInt
	(
	JUInt*		value,
	const JSize	base
	)
	const
{
	return ConvertToUInt(itsBytes, itsByteCount, value, base);
}

// static

inline JBoolean
JString::ConvertToFloat
	(
	const JUtf8Byte*	str,
	JFloat*				value
	)
{
	return ConvertToFloat(str, strlen(str), value);
}

inline JBoolean
JString::ConvertToInteger
	(
	const JUtf8Byte*	str,
	JInteger*			value,
	const JSize			base
	)
{
	return ConvertToInteger(str, strlen(str), value, base);
}

inline JBoolean
JString::ConvertToUInt
	(
	const JUtf8Byte*	str,
	JUInt*				value,
	const JSize			base
	)
{
	return ConvertToUInt(str, strlen(str), value, base);
}

/******************************************************************************
 Block size

 ******************************************************************************/

inline JSize
JString::GetBlockSize()
	const
{
	return itsBlockSize;
}

inline void
JString::SetBlockSize
	(
	const JSize blockSize
	)
{
	itsBlockSize = blockSize;
}

/******************************************************************************
 Default block size (static)

 ******************************************************************************/

inline JSize
JString::GetDefaultBlockSize()
{
	return theDefaultBlockSize;
}

inline void
JString::SetDefaultBlockSize
	(
	const JSize blockSize
	)
{
	theDefaultBlockSize = blockSize;
}

#endif
