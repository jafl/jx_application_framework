/******************************************************************************
 JString.h

	Interface for the JString class

	Copyright (C) 1994-2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_JString
#define _H_JString

#include "JUtf8Character.h"
#include "JUtf8ByteRange.h"
#include "JCharacterRange.h"
#include "JPtrArray.h"	// for Split()
#include <string.h>
#include <unicode/ucasemap.h>
#include <unicode/ucol.h>
#include <compare>

class JStringIterator;
class JRegex;

class JString
{
	friend class JStringIterator;
	friend std::istream& operator>>(std::istream&, JString&);
	friend std::ostream& operator<<(std::ostream&, const JString&);

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

	enum Case
	{
		kIgnoreCase  = 0,
		kCompareCase = 1
	};

	enum Copy
	{
		kNoCopy = 0,
		kCopy   = 1
	};

	static const JString empty;
	static const JString newline;

public:

	JString() : JString(true) {};
	JString(const JString& str, const Copy copy = kCopy);
	JString(const JString& str, const JCharacterRange& range, const Copy copy = kCopy);
	JString(const JUtf8Byte* str, const JSize byteCount, const Copy copy = kCopy);
	JString(const JUtf8Byte* str, const JUtf8ByteRange& range, const Copy copy = kCopy);
	JString(const JUtf8Character& c) : JString(c.GetBytes(), 0) {};
	JString(const std::string& str, const JUtf8ByteRange& range);

	explicit JString(const bool normalize);		// prevent sneaky, incorrect, automatic construction from char*
	explicit JString(const JUtf8Byte* str, const Copy copy = kCopy) : JString(str, 0, copy) {};
	explicit JString(const std::string& str) : JString(str, JUtf8ByteRange()) {};	// prevent sneaky automatic construction from char*

	explicit JString(const JUInt64 number, const Base base = kBase10, const bool pad = false);
	explicit JString(const JFloat number, const JInteger precision = kPrecisionAsNeeded,
					 const ExponentDisplay expDisplay = kStandardExponent,
					 const JInteger exponent = 0, const JInteger sigDigitCount = 0);

	~JString();

	void* operator new(size_t sz) noexcept;
	void* operator new(size_t size, const JUtf8Byte* file, const JUInt32 line) noexcept;
	void* operator new(size_t sz, const bool forceShallow) noexcept;
	void  operator delete(void* memory) noexcept;

	JString& operator=(const JString& str);
	JString& operator=(const JUtf8Byte* str);
	JString& operator=(const std::string& str);
	JString& operator=(const JUtf8Character& c);

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
	void	Set(const JUtf8Character& c);

	void	Clear();

	// no cast operator, to force everybody to remember it is UTF-8, not char*

	bool	IsEmpty() const;
	JSize	GetCharacterCount() const;
	JSize	GetByteCount() const;
	bool	CharacterIndexValid(const JIndex index) const;
	bool	ByteIndexValid(const JIndex index) const;
	bool	RangeValid(const JCharacterRange& range) const;
	bool	RangeValid(const JUtf8ByteRange& range) const;
	bool	IsAscii() const;

	JUtf8Character	GetFirstCharacter() const;
	JUtf8Character	GetLastCharacter() const;

	const JUtf8Byte*	GetBytes() const;
	JUtf8Byte*			AllocateBytes() const;	// client must call delete [] when finished with it
	bool				IsOwner() const;		// primarily for debugging
	const JUtf8Byte*	GetRawBytes() const;	// NOT guaranteed to be nullptr terminated

	bool	StartsWith(const JString& str, const Case caseSensitive = kCompareCase) const;
	bool	StartsWith(const JString& str, const JCharacterRange& range, const Case caseSensitive = kCompareCase) const;
	bool	StartsWith(const JUtf8Byte* str, const Case caseSensitive = kCompareCase) const;
	bool	StartsWith(const JUtf8Byte* str, const JSize byteCount, const Case caseSensitive = kCompareCase) const;
	bool	StartsWith(const JUtf8Byte* str, const JUtf8ByteRange& range, const Case caseSensitive = kCompareCase) const;
	bool	StartsWith(const JUtf8Character& c, const Case caseSensitive = kCompareCase) const;
	bool	StartsWith(const std::string& str, const Case caseSensitive = kCompareCase) const;
	bool	StartsWith(const std::string& str, const JUtf8ByteRange& range, const Case caseSensitive = kCompareCase) const;

	bool	Contains(const JString& str, const Case caseSensitive = kCompareCase) const;
	bool	Contains(const JString& str, const JCharacterRange& range, const Case caseSensitive = kCompareCase) const;
	bool	Contains(const JUtf8Byte* str, const Case caseSensitive = kCompareCase) const;
	bool	Contains(const JUtf8Byte* str, const JSize byteCount, const Case caseSensitive = kCompareCase) const;
	bool	Contains(const JUtf8Byte* str, const JUtf8ByteRange& range, const Case caseSensitive = kCompareCase) const;
	bool	Contains(const JUtf8Character& c, const Case caseSensitive = kCompareCase) const;
	bool	Contains(const std::string& str, const Case caseSensitive = kCompareCase) const;
	bool	Contains(const std::string& str, const JUtf8ByteRange& range, const Case caseSensitive = kCompareCase) const;

	bool	EndsWith(const JString& str, const Case caseSensitive = kCompareCase) const;
	bool	EndsWith(const JString& str, const JCharacterRange& range, const Case caseSensitive = kCompareCase) const;
	bool	EndsWith(const JUtf8Byte* str, const Case caseSensitive = kCompareCase) const;
	bool	EndsWith(const JUtf8Byte* str, const JSize byteCount, const Case caseSensitive = kCompareCase) const;
	bool	EndsWith(const JUtf8Byte* str, const JUtf8ByteRange& range, const Case caseSensitive = kCompareCase) const;
	bool	EndsWith(const JUtf8Character& c, const Case caseSensitive = kCompareCase) const;
	bool	EndsWith(const std::string& str, const Case caseSensitive = kCompareCase) const;
	bool	EndsWith(const std::string& str, const JUtf8ByteRange& range, const Case caseSensitive = kCompareCase) const;

	JString	EncodeBase64() const;
	bool	DecodeBase64(JString* str) const;

	void	Read(std::istream& input, const JSize byteCount);
	void	ReadDelimited(std::istream& input);
	void	Print(std::ostream& output) const;
	void	PrintHex(std::ostream& output) const;

	JSize	GetBlockSize() const;
	void	SetBlockSize(const JSize blockSize);

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

	bool	MatchCase(const JString& source, const JCharacterRange& sourceRange);
	bool	MatchCase(const JUtf8Byte* source, const JUtf8ByteRange& sourceRange);
	bool	MatchCase(const std::string& source, const JUtf8ByteRange& sourceRange);
	bool	MatchCase(const JString& source, const JCharacterRange& sourceRange,
					  const JCharacterRange& destRange);
	bool	MatchCase(const JUtf8Byte* source, const JUtf8ByteRange& sourceRange,
					  const JCharacterRange& destRange);
	bool	MatchCase(const std::string& source, const JUtf8ByteRange& sourceRange,
					  const JCharacterRange& destRange);

	void	Split(const JUtf8Byte* separator, JPtrArray<JString>* partList,
				  const JSize limit = 0,
				  const bool includeSeparators = false) const;
	void	Split(const JString& separator, JPtrArray<JString>* partList,
				  const JSize limit = 0,
				  const bool includeSeparators = false) const;
	void	Split(const JRegex& separator, JPtrArray<JString>* partList,
				  const JSize limit = 0,
				  const bool includeSeparators = false) const;

	// number <-> string

	bool	IsFloat() const;
	bool	ConvertToFloat(JFloat* value) const;

	bool	IsInteger(const JSize base = 10) const;
	bool	ConvertToInteger(JInteger* value, const JSize base = 10) const;

	bool	IsUInt(const JSize base = 10) const;
	bool	IsHexValue() const;
	bool	ConvertToUInt(JUInt* value, const JSize base = 10) const;

	static bool	IsFloat(const JUtf8Byte* str);
	static bool	IsFloat(const JUtf8Byte* str, const JSize byteCount);
	static bool	ConvertToFloat(const JUtf8Byte* str, JFloat* value);
	static bool	ConvertToFloat(const JUtf8Byte* str, const JSize byteCount,
							   JFloat* value);

	static bool	IsInteger(const JUtf8Byte* str, const JSize base = 10);
	static bool	IsInteger(const JUtf8Byte* str, const JSize byteCount, const JSize base);
	static bool	ConvertToInteger(const JUtf8Byte* str,
								 JInteger* value, const JSize base = 10);
	static bool	ConvertToInteger(const JUtf8Byte* str, const JSize byteCount,
								 JInteger* value, const JSize base = 10);

	static bool	IsUInt(const JUtf8Byte* str, const JSize base = 10);
	static bool	IsUInt(const JUtf8Byte* str, const JSize byteCount, const JSize base);
	static bool	IsHexValue(const JUtf8Byte* str);
	static bool	IsHexValue(const JUtf8Byte* str, const JSize byteCount);
	static bool	ConvertToUInt(const JUtf8Byte* str,
							  JUInt* value, const JSize base = 10);
	static bool	ConvertToUInt(const JUtf8Byte* str, const JSize byteCount,
							  JUInt* value, const JSize base = 10);

	// utility functions

	static bool	IsEmpty(const JUtf8Byte* s);
	static bool	IsEmpty(const JString* s);

	static bool				IsValid(const JUtf8Byte* s);
	static bool				IsValid(const JUtf8Byte* s, const JUtf8ByteRange& range);
	static JSize			CountCharacters(const JUtf8Byte* str);
	static JSize			CountCharacters(const JUtf8Byte* str, const JSize byteCount);
	static JSize			CountCharacters(const JUtf8Byte* str, const JUtf8ByteRange& range);
	static JSize			CountBytes(const JUtf8Byte* str, const JSize characterCount);
	static bool				CountBytesBackward(const JUtf8Byte* str, const JSize byteOffset, const JSize characterCount, JSize* byteCount);
	static JUtf8ByteRange	CharacterToUtf8ByteRange(const JUtf8Byte* str, const JCharacterRange& range);

	static int Compare(const JString& s1, const JString& s2, const Case caseSensitive = kCompareCase);
	static int Compare(const JString& s1, const JUtf8Byte* s2, const Case caseSensitive = kCompareCase);
	static int Compare(const JUtf8Byte* s1, const JString& s2, const Case caseSensitive = kCompareCase);
	static int Compare(const JUtf8Byte* s1, const JUtf8Byte* s2, const Case caseSensitive = kCompareCase);
	static int Compare(const JString& s1, const std::string& s2, const Case caseSensitive = kCompareCase);
	static int Compare(const std::string& s1, const JString& s2, const Case caseSensitive = kCompareCase);
	static int Compare(const JUtf8Byte* s1, const JSize byteCount1,
					   const JUtf8Byte* s2, const JSize byteCount2,
					   const Case caseSensitive = kCompareCase);

	static int	CompareMaxNBytes(const JUtf8Byte* s1, const JUtf8Byte* s2, const JSize N,
								 const Case caseSensitive = kCompareCase);

	static JSize	CalcCharacterMatchLength(const JString& s1, const JString& s2,
											 const Case caseSensitive = kCompareCase);

	static JSize	Normalize(const JUtf8Byte* source, const JSize byteCount,
							  JUtf8Byte** destination);
	static JSize	CopyNormalizedBytes(const JUtf8Byte* source, const JSize maxBytes,
										JUtf8Byte* destination, const JSize capacity);

	static JSize	GetDefaultBlockSize();
	static void		SetDefaultBlockSize(const JSize blockSize);

protected:

	const JUtf8Byte*	GetUnterminatedBytes() const;
	void				SetIterator(JStringIterator* iter) const;	// conceptually const

	JUtf8ByteRange	CharacterToUtf8ByteRange(const JCharacterRange& range) const;

	bool	SearchForward(const JUtf8Byte* str, const JSize byteCount,
						  const Case caseSensitive, JIndex* byteIndex) const;
	bool	SearchBackward(const JUtf8Byte* str, const JSize byteCount,
						   const Case caseSensitive, JIndex* byteIndex) const;

	void	ReplaceBytes(const JUtf8ByteRange& replaceRange,
						 const JUtf8Byte* stringToInsert, const JSize insertByteCount);

	bool	MatchCase(const JUtf8Byte* source, const JUtf8ByteRange& sourceRange,
					  const JUtf8ByteRange& destRange);

private:

	bool		itsOwnerFlag;		// false => somebody else owns the byte allocation
	const bool	itsNormalizeFlag;	// false => store raw bytes, including null
	JUtf8Byte*	itsBytes;			// characters
	JSize		itsByteCount;		// number of bytes used
	JSize		itsCharacterCount;	// number of characters
	JSize		itsAllocCount;		// number of bytes we have space for
	JSize		itsBlockSize;		// size by which to shrink and grow allocation

	UCaseMap*			itsUCaseMap;
	JStringIterator*	itsIterator;	// only one iterator allowed at a time

	static JSize theDefaultBlockSize;

private:

	void	CopyToPrivateBuffer(const JUtf8Byte* str, const JSize byteCount,
								const bool invalidateIterator = true);
	void	FoldCase(const bool upper);

	static bool	CompleteConversion(const JUtf8Byte* startPtr, const JSize byteCount,
									   const JUtf8Byte* convEndPtr);

	static bool				GetCollator(UCollator** coll, const Case caseSensitive);
	static UCollationResult	Compare(UCollator* coll,
									const JUtf8Byte* s1, const JSize length1,
									const JUtf8Byte* s2, const JSize length2,
									const Case caseSensitive);
};


/******************************************************************************
 IsOwner

 ******************************************************************************/

inline bool
JString::IsOwner()
	const
{
	return itsOwnerFlag;
}

/******************************************************************************
 IsAscii

 ******************************************************************************/

inline bool
JString::IsAscii()
	const
{
	return itsByteCount == itsCharacterCount;
}

/******************************************************************************
 GetRawBytes

	*** NOT guaranteed to be nullptr terminated

 ******************************************************************************/

inline const JUtf8Byte*
JString::GetRawBytes()
	const
{
	return itsBytes;
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

inline bool
JString::IsEmpty()
	const
{
	return itsByteCount == 0;
}

/******************************************************************************
 JString::IsEmpty

 ******************************************************************************/

inline bool
JString::IsEmpty
	(
	const JUtf8Byte* s
	)
{
	return s == nullptr || s[0] == '\0';
}

inline bool
JString::IsEmpty
	(
	const JString* s
	)
{
	return s == nullptr || s->IsEmpty();
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

inline bool
JString::CharacterIndexValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= itsCharacterCount;
}

/******************************************************************************
 ByteIndexValid

 ******************************************************************************/

inline bool
JString::ByteIndexValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= itsByteCount;
}

/******************************************************************************
 RangeValid

 ******************************************************************************/

inline bool
JString::RangeValid
	(
	const JCharacterRange& range
	)
	const
{
	return CharacterIndexValid(range.first) &&
				(range.IsEmpty() || CharacterIndexValid(range.last));
}

inline bool
JString::RangeValid
	(
	const JUtf8ByteRange& range
	)
	const
{
	return ByteIndexValid(range.first) &&
				(range.IsEmpty() || ByteIndexValid(range.last));
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

inline bool
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
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	return CountCharacters(str, JUtf8ByteRange(1, byteCount));
}

/******************************************************************************
 StartsWith

 ******************************************************************************/

inline bool
JString::StartsWith
	(
	const JString&	str,
	const Case		caseSensitive
	)
	const
{
	return StartsWith(str.itsBytes, JUtf8ByteRange(1, str.itsByteCount), caseSensitive);
}

inline bool
JString::StartsWith
	(
	const JString&			str,
	const JCharacterRange&	range,
	const Case				caseSensitive
	)
	const
{
	return StartsWith(str.itsBytes + range.first-1, str.CharacterToUtf8ByteRange(range), caseSensitive);
}

inline bool
JString::StartsWith
	(
	const JUtf8Byte*	str,
	const Case			caseSensitive
	)
	const
{
	return StartsWith(str, JUtf8ByteRange(1, strlen(str)), caseSensitive);
}

inline bool
JString::StartsWith
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const Case			caseSensitive
	)
	const
{
	return StartsWith(str, JUtf8ByteRange(1, byteCount), caseSensitive);
}

inline bool
JString::StartsWith
	(
	const JUtf8Character&	c,
	const Case				caseSensitive
	)
	const
{
	return StartsWith(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), caseSensitive);
}

inline bool
JString::StartsWith
	(
	const std::string&	str,
	const Case			caseSensitive
	)
	const
{
	return StartsWith(str.data(), JUtf8ByteRange(1, str.length()), caseSensitive);
}

inline bool
JString::StartsWith
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const Case				caseSensitive
	)
	const
{
	return StartsWith(str.data(), range, caseSensitive);
}

/******************************************************************************
 Contains

 ******************************************************************************/

inline bool
JString::Contains
	(
	const JString&	str,
	const Case		caseSensitive
	)
	const
{
	return Contains(str.itsBytes, JUtf8ByteRange(1, str.itsByteCount), caseSensitive);
}

inline bool
JString::Contains
	(
	const JString&			str,
	const JCharacterRange&	range,
	const Case				caseSensitive
	)
	const
{
	return Contains(str.itsBytes + range.first-1, str.CharacterToUtf8ByteRange(range), caseSensitive);
}

inline bool
JString::Contains
	(
	const JUtf8Byte*	str,
	const Case			caseSensitive
	)
	const
{
	return Contains(str, JUtf8ByteRange(1, strlen(str)), caseSensitive);
}

inline bool
JString::Contains
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const Case			caseSensitive
	)
	const
{
	return Contains(str, JUtf8ByteRange(1, byteCount), caseSensitive);
}

inline bool
JString::Contains
	(
	const JUtf8Character&	c,
	const Case				caseSensitive
	)
	const
{
	return Contains(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), caseSensitive);
}

inline bool
JString::Contains
	(
	const std::string&	str,
	const Case			caseSensitive
	)
	const
{
	return Contains(str.data(), JUtf8ByteRange(1, str.length()), caseSensitive);
}

inline bool
JString::Contains
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const Case				caseSensitive
	)
	const
{
	return Contains(str.data(), range, caseSensitive);
}

/******************************************************************************
 EndsWith

 ******************************************************************************/

inline bool
JString::EndsWith
	(
	const JString&	str,
	const Case		caseSensitive
	)
	const
{
	return EndsWith(str.itsBytes, JUtf8ByteRange(1, str.itsByteCount), caseSensitive);
}

inline bool
JString::EndsWith
	(
	const JString&			str,
	const JCharacterRange&	range,
	const Case				caseSensitive
	)
	const
{
	return EndsWith(str.itsBytes + range.first-1, str.CharacterToUtf8ByteRange(range), caseSensitive);
}

inline bool
JString::EndsWith
	(
	const JUtf8Byte*	str,
	const Case			caseSensitive
	)
	const
{
	return EndsWith(str, JUtf8ByteRange(1, strlen(str)), caseSensitive);
}

inline bool
JString::EndsWith
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const Case			caseSensitive
	)
	const
{
	return EndsWith(str, JUtf8ByteRange(1, byteCount), caseSensitive);
}

inline bool
JString::EndsWith
	(
	const JUtf8Character&	c,
	const Case			caseSensitive
	)
	const
{
	return EndsWith(c.GetBytes(), JUtf8ByteRange(1, c.GetByteCount()), caseSensitive);
}

inline bool
JString::EndsWith
	(
	const std::string&	str,
	const Case			caseSensitive
	)
	const
{
	return EndsWith(str.data(), JUtf8ByteRange(1, str.length()), caseSensitive);
}

inline bool
JString::EndsWith
	(
	const std::string&		str,
	const JUtf8ByteRange&	range,
	const Case				caseSensitive
	)
	const
{
	return EndsWith(str.data(), range, caseSensitive);
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
	JString sum(str);
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

<!--
	*** We have to use the version of JString::Compare() that takes byte counts
		because JStrings can contain NULLs.
-->
 ******************************************************************************/

inline bool
operator==
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JString::Compare(s1.GetRawBytes(), s1.GetByteCount(), s2.GetRawBytes(), s2.GetByteCount()) == 0);
}

inline bool
operator==
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return (JString::Compare(s.GetRawBytes(), s.GetByteCount(), str, strlen(str)) == 0);
}

inline bool
operator==
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetRawBytes(), s.GetByteCount(), str, strlen(str)) == 0);
}

inline bool
operator==
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JString::Compare(s.GetRawBytes(), s.GetByteCount(), str.data(), str.length()) == 0);
}

inline bool
operator==
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JString::Compare(s.GetRawBytes(), s.GetByteCount(), str.data(), str.length()) == 0);
}

/******************************************************************************
 Comparison (case-insensitive)

<!--
	*** We have to use the version of JString::Compare() that takes byte counts
		because JStrings can contain NULLs.
-->
 ******************************************************************************/

inline std::weak_ordering
jIntToWeakOrdering
	(
	const int r
	)
{
	return (r < 0 ? std::weak_ordering::less :
			r > 0 ? std::weak_ordering::greater : std::weak_ordering::equivalent);
}

inline std::weak_ordering
operator<=>
	(
	const JString& s1,
	const JString& s2
	)
{
	return jIntToWeakOrdering(JString::Compare(s1.GetRawBytes(), s1.GetByteCount(), s2.GetRawBytes(), s2.GetByteCount(), JString::kIgnoreCase));
}

inline std::weak_ordering
operator<=>
	(
	const JString&		s,
	const JUtf8Byte*	str
	)
{
	return jIntToWeakOrdering(JString::Compare(s.GetRawBytes(), s.GetByteCount(), str, strlen(str), JString::kIgnoreCase));
}

inline std::weak_ordering
operator<=>
	(
	const JUtf8Byte*	str,
	const JString&		s
	)
{
	return jIntToWeakOrdering(JString::Compare(s.GetRawBytes(), s.GetByteCount(), str, strlen(str), JString::kIgnoreCase));
}

inline std::weak_ordering
operator<=>
	(
	const JString&		s,
	const std::string&	str
	)
{
	return jIntToWeakOrdering(JString::Compare(s.GetRawBytes(), s.GetByteCount(), str.data(), str.length(), JString::kIgnoreCase));
}

inline std::weak_ordering
operator<=>
	(
	const std::string&	str,
	const JString&		s
	)
{
	return jIntToWeakOrdering(JString::Compare(s.GetRawBytes(), s.GetByteCount(), str.data(), str.length(), JString::kIgnoreCase));
}

/******************************************************************************
 Compare (static)

	Replaces strcmp(): + if s1>s2, 0 if s1==s2, - if s1<s2

<!--
	*** We have to use the version of JString::Compare() that takes byte counts
		because JStrings can contain NULLs.
-->
 ******************************************************************************/

inline int
JString::Compare
	(
	const JString&	s1,
	const JString&	s2,
	const Case		caseSensitive
	)
{
	return JString::Compare(s1.GetRawBytes(), s1.GetByteCount(), s2.GetRawBytes(), s2.GetByteCount(), caseSensitive);
}

inline int
JString::Compare
	(
	const JString&		s1,
	const JUtf8Byte*	s2,
	const Case			caseSensitive
	)
{
	return JString::Compare(s1.GetRawBytes(), s1.GetByteCount(), s2, strlen(s2), caseSensitive);
}

inline int
JString::Compare
	(
	const JUtf8Byte*	s1,
	const JString&		s2,
	const Case			caseSensitive
	)
{
	return JString::Compare(s1, strlen(s1), s2.GetRawBytes(), s2.GetByteCount(), caseSensitive);
}

inline int
JString::Compare
	(
	const JUtf8Byte*	s1,
	const JUtf8Byte*	s2,
	const Case			caseSensitive
	)
{
	return Compare(s1, strlen(s1), s2, strlen(s2), caseSensitive);
}

inline int
JString::Compare
	(
	const JString&		s1,
	const std::string&	s2,
	const Case			caseSensitive
	)
{
	return JString::Compare(s1.GetRawBytes(), s1.GetByteCount(), s2.data(), s2.length(), caseSensitive);
}

inline int
JString::Compare
	(
	const std::string&	s1,
	const JString&		s2,
	const Case			caseSensitive
	)
{
	return JString::Compare(s1.data(), s1.length(), s2.GetRawBytes(), s2.GetByteCount(), caseSensitive);
}

/******************************************************************************
 Set

 ******************************************************************************/

inline void
JString::Set
	(
	const JUtf8Byte* str
	)
{
	CopyToPrivateBuffer(str, strlen(str));
}

inline void
JString::Set
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	CopyToPrivateBuffer(str, byteCount);
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
		CopyToPrivateBuffer(str + range.first-1, range.GetCount());
		}
}

inline void
JString::Set
	(
	const std::string& str
	)
{
	CopyToPrivateBuffer(str.data(), str.length());
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
		CopyToPrivateBuffer(str.data() + range.first-1, range.GetCount());
		}
}

inline void
JString::Set
	(
	const JUtf8Character& c
	)
{
	CopyToPrivateBuffer(c.GetBytes(), c.GetByteCount());
}

/******************************************************************************
 Assignment operator

	We do not copy itsBlockSize because we assume the client has set it
	appropriately.

 ******************************************************************************/

inline JString&
JString::operator=
	(
	const JString& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	Set(source);
	return *this;
}

inline JString&
JString::operator=
	(
	const JUtf8Byte* str
	)
{
	Set(str);
	return *this;
}

inline JString&
JString::operator=
	(
	const std::string& str
	)
{
	Set(str);
	return *this;
}

inline JString&
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

	Returns true if we can convert ourselves to a number.

 ******************************************************************************/

inline bool
JString::IsFloat()
	const
{
	JFloat value;
	return ConvertToFloat(itsBytes, itsByteCount, &value);
}

inline bool
JString::IsInteger
	(
	const JSize base
	)
	const
{
	JInteger value;
	return ConvertToInteger(itsBytes, itsByteCount, &value, base);
}

inline bool
JString::IsUInt
	(
	const JSize base
	)
	const
{
	JUInt value;
	return ConvertToUInt(itsBytes, itsByteCount, &value, base);
}

inline bool
JString::IsHexValue()
	const
{
	return IsHexValue(itsBytes, itsByteCount);
}

// static

inline bool
JString::IsFloat
	(
	const JUtf8Byte* str
	)
{
	JFloat value;
	return ConvertToFloat(str, strlen(str), &value);
}

inline bool
JString::IsFloat
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	JFloat value;
	return ConvertToFloat(str, byteCount, &value);
}

inline bool
JString::IsInteger
	(
	const JUtf8Byte*	str,
	const JSize			base
	)
{
	JInteger value;
	return ConvertToInteger(str, strlen(str), &value, base);
}

inline bool
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

inline bool
JString::IsUInt
	(
	const JUtf8Byte*	str,
	const JSize			base
	)
{
	JUInt value;
	return ConvertToUInt(str, strlen(str), &value, base);
}

inline bool
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

inline bool
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

inline bool
JString::ConvertToFloat
	(
	JFloat* value
	)
	const
{
	return ConvertToFloat(itsBytes, itsByteCount, value);
}

inline bool
JString::ConvertToInteger
	(
	JInteger*	value,
	const JSize	base
	)
	const
{
	return ConvertToInteger(itsBytes, itsByteCount, value, base);
}

inline bool
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

inline bool
JString::ConvertToFloat
	(
	const JUtf8Byte*	str,
	JFloat*				value
	)
{
	return ConvertToFloat(str, strlen(str), value);
}

inline bool
JString::ConvertToInteger
	(
	const JUtf8Byte*	str,
	JInteger*			value,
	const JSize			base
	)
{
	return ConvertToInteger(str, strlen(str), value, base);
}

inline bool
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
 Split

 ******************************************************************************/

inline void
JString::Split
	(
	const JUtf8Byte*	separator,
	JPtrArray<JString>*	partList,
	const JSize			limit,
	const bool			includeSeparators
	)
	const
{
	Split(JString(separator, kNoCopy), partList, limit, includeSeparators);
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

/******************************************************************************
 GetUnterminatedBytes (protected)

 ******************************************************************************/

inline const JUtf8Byte*
JString::GetUnterminatedBytes()
	const
{
	return itsBytes;
}

#endif
