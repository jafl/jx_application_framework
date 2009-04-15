/******************************************************************************
 JString.h

	Interface for the JString class

	Copyright © 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JString
#define _H_JString

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>
#include <JIndexRange.h>
#include <string.h>

class JString
{
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
	JString(const JString& source);
	JString(const JCharacter* str);
	JString(const JCharacter* str, const JSize length);
	JString(const JCharacter* str, const JIndexRange& range);
	JString(const JFloat number, const JInteger precision = kPrecisionAsNeeded,
			const ExponentDisplay expDisplay = kStandardExponent,
			const JInteger exponent = 0, const JInteger sigDigitCount = 0);
	JString(const JUInt number, const Base base, const JBoolean pad = kJFalse);
	JString(const std::string& s);

	~JString();

	const JString& operator=(const JString& str);
	const JString& operator=(const JCharacter* str);
	const JString& operator=(const std::string& str);

	JString& operator+=(const JString& str);
	JString& operator+=(const JCharacter* str);
	JString& operator+=(const std::string& str);

	operator const JCharacter*() const;

	void	Set(const JString& str);
	void	Set(const JCharacter* str);
	void	Set(const JCharacter* str, const JSize length);
	void	Set(const JCharacter* str, const JIndexRange& range);
	void	Set(const std::string& str);
	void	Set(const std::string& str, const JIndexRange& range);

	JBoolean			ContainsNULL() const;
	const JCharacter*	GetCString() const;
	JCharacter*			AllocateCString() const;	// client must call delete [] when finished with it

	void	InsertSubstring(const JString& str, const JIndex insertionIndex);
	void	InsertSubstring(const JCharacter* str, const JIndex insertionIndex);
	void	InsertSubstring(const JCharacter* str, const JSize length,
							const JIndex insertionIndex);
	void	InsertSubstring(const std::string& str, const JIndex insertionIndex);
	void	InsertCharacter(const JCharacter c, const JIndex insertionIndex);

	void	Prepend(const JString& str);
	void	Prepend(const JCharacter* str);
	void	Prepend(const JCharacter* str, const JSize length);
	void	Prepend(const std::string& str);
	void	PrependCharacter(const JCharacter c);

	void	Append(const JString& str);
	void	Append(const JCharacter* str);
	void	Append(const JCharacter* str, const JSize length);
	void	Append(const std::string& str);
	void	AppendCharacter(const JCharacter c);

	JBoolean	IsEmpty() const;
	JSize		GetLength() const;
	JBoolean	IndexValid(const JIndex index) const;
	JBoolean	RangeValid(const JIndexRange& range) const;

	JCharacter	GetCharacter(const JIndex index) const;
	void		SetCharacter(const JIndex index, const JCharacter c);

	JCharacter	GetFirstCharacter() const;
	JCharacter	GetLastCharacter() const;

	JCharacter	GetCharacterFromEnd(const JIndex index) const;
	void		SetCharacterFromEnd(const JIndex index, const JCharacter c);

	void	Clear();
	void	TrimWhitespace();
	void	ToLower();
	void	ToUpper();

	JBoolean	Contains(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JCharacter* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JCharacter* str, const JSize length,
						 const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const std::string& str, const JBoolean caseSensitive = kJTrue) const;

	JBoolean	LocateSubstring(const JString& str, JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter* str, JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter* str, const JSize length,
								JIndex* startIndex) const;
	JBoolean	LocateSubstring(const std::string& str, JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JString& str, const JBoolean caseSensitive,
								JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter* str, const JBoolean caseSensitive,
								JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter* str, const JSize length,
								const JBoolean caseSensitive, JIndex* startIndex) const;
	JBoolean	LocateSubstring(const std::string& str, const JBoolean caseSensitive,
								JIndex* startIndex) const;

	JBoolean	LocateNextSubstring(const JString& str, JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter* str, JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter* str, const JSize length,
									JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const std::string& str, JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JString& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter* str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter* str, const JSize length,
									const JBoolean caseSensitive, JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const std::string& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;

	JBoolean	LocatePrevSubstring(const JString& str, JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter* str, JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter* str, const JSize length,
									JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const std::string& str, JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JString& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter* str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter* str, const JSize length,
									const JBoolean caseSensitive, JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const std::string& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;

	JBoolean	LocateLastSubstring(const JString& str, JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter* str, JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter* str, const JSize length,
									JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const std::string& str, JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JString& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter* str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter* str, const JSize length,
									const JBoolean caseSensitive, JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const std::string& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;

	JBoolean	BeginsWith(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JCharacter* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JCharacter* str, const JSize length,
						   const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const std::string& str, const JBoolean caseSensitive = kJTrue) const;

	JBoolean	EndsWith(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JCharacter* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JCharacter* str, const JSize length,
						 const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const std::string& str, const JBoolean caseSensitive = kJTrue) const;

	JString		GetSubstring(const JIndex firstCharIndex, const JIndex lastCharIndex) const;
	JString		GetSubstring(const JIndexRange& range) const;	// allows empty range

	void		Extract(const JArray<JIndexRange>& rangeList,
						JPtrArray<JString>* substringList) const;

	void		ReplaceSubstring(const JIndex firstCharIndex,
								 const JIndex lastCharIndex,
								 const JString& str);
	void		ReplaceSubstring(const JIndex firstCharIndex,
								 const JIndex lastCharIndex,
								 const JCharacter* str);
	void		ReplaceSubstring(const JIndex firstCharIndex,
								 const JIndex lastCharIndex,
								 const JCharacter* str, const JSize length);
	void		ReplaceSubstring(const JIndex firstCharIndex,
								 const JIndex lastCharIndex,
								 const std::string& str);
	void		ReplaceSubstring(const JIndexRange& range,
								 const JString& str, JIndexRange* newRange);
	void		ReplaceSubstring(const JIndexRange& range,
								 const JCharacter* str, JIndexRange* newRange);
	void		ReplaceSubstring(const JIndexRange& range,
								 const JCharacter* str, const JSize length,
								 JIndexRange* newRange);
	void		ReplaceSubstring(const JIndexRange& range,
								 const std::string& str, JIndexRange* newRange);

	void		RemoveSubstring(const JIndex firstCharIndex, const JIndex lastCharIndex);
	void		RemoveSubstring(const JIndexRange& range);

	JBoolean	MatchCase(const JString& source, const JIndexRange& sourceRange);
	JBoolean	MatchCase(const JCharacter* source, const JIndexRange& sourceRange);
	JBoolean	MatchCase(const std::string& source, const JIndexRange& sourceRange);
	JBoolean	MatchCase(const JString& source, const JIndexRange& sourceRange,
						  const JIndexRange& destRange);
	JBoolean	MatchCase(const JCharacter* source, const JIndexRange& sourceRange,
						  const JIndexRange& destRange);
	JBoolean	MatchCase(const std::string& source, const JIndexRange& sourceRange,
						  const JIndexRange& destRange);

	JBoolean	IsFloat() const;
	JBoolean	ConvertToFloat(JFloat* value) const;

	JBoolean	IsInteger(const JSize base = 10) const;
	JBoolean	ConvertToInteger(JInteger* value, const JSize base = 10) const;

	JBoolean	IsUInt(const JSize base = 10) const;
	JBoolean	IsHexValue() const;
	JBoolean	ConvertToUInt(JUInt* value, const JSize base = 10) const;

	JString		EncodeBase64();
	JBoolean	DecodeBase64(JString* str);

	void		Read(istream& input, const JSize count);
	void		ReadDelimited(istream& input);
	void		Print(ostream& output) const;

	JSize		GetBlockSize() const;
	void		SetBlockSize(const JSize blockSize);

	static JBoolean	IsFloat(const JCharacter* str);
	static JBoolean	ConvertToFloat(const JCharacter* str, JFloat* value);

	static JBoolean	IsInteger(const JCharacter* str, const JSize base = 10);
	static JBoolean	ConvertToInteger(const JCharacter* str,
									 JInteger* value, const JSize base = 10);

	static JBoolean	IsUInt(const JCharacter* str, const JSize base = 10);
	static JBoolean	IsHexValue(const JCharacter* str);
	static JBoolean	ConvertToUInt(const JCharacter* str,
								  JUInt* value, const JSize base = 10);

private:

	JCharacter* itsString;			// characters
	JSize		itsStringLength;	// number of characters used
	JSize		itsAllocLength;		// number of characters we have space for
	JSize		itsBlockSize;		// size by which to shrink and grow allocation

private:

	void		CopyToPrivateString(const JCharacter* str);
	void		CopyToPrivateString(const JCharacter* str, const JSize length);

	JCharacter	PrivateGetCharacter(const JIndex index) const;
	JCharacter*	GetCharacterPtr(const JIndex index) const;

	static JBoolean	ConvertToFloat(const JCharacter* str, const JSize length,
								   JFloat* value);
	static JBoolean	ConvertToInteger(const JCharacter* str, const JSize length,
									 JInteger* value, const JSize base = 10);
	static JBoolean	ConvertToUInt(const JCharacter* str, const JSize length,
								  JUInt* value, const JSize base = 10);
	static JBoolean	IsHexValue(const JCharacter* str, const JSize length);
	static JBoolean	CompleteConversion(const JCharacter* startPtr, const JSize length,
									   const JCharacter* convEndPtr);
};


// declarations of global functions for dealing with strings

int JStringCompare(const JCharacter* s1, const JCharacter* s2,
				   const JBoolean caseSensitive = kJTrue);
int JStringCompare(const JCharacter* s1, const JSize length1,
				   const JCharacter* s2, const JSize length2,
				   const JBoolean caseSensitive = kJTrue);

JBoolean	JCompareMaxN(const JCharacter* s1, const JCharacter* s2, const JSize N,
						 const JBoolean caseSensitive = kJTrue);
JBoolean	JCompareMaxN(const JCharacter* s1, const JSize length1,
						 const JCharacter* s2, const JSize length2,
						 const JSize N, const JBoolean caseSensitive = kJTrue);

JSize		JCalcMatchLength(const JCharacter* s1, const JCharacter* s2,
							 const JBoolean caseSensitive = kJTrue);

JBoolean	JCopyMaxN(const JCharacter* source, const JIndex maxBytes,
					  JCharacter* destination);

JBoolean	JMatchCase(const JCharacter* source, const JIndexRange& sourceRange,
					   JCharacter* dest, const JIndexRange& destRange);

JBoolean	JIsPrint(const JCharacter c);
JBoolean	JIsAlnum(const JCharacter c);
JBoolean	JIsAlpha(const JCharacter c);
JBoolean	JIsUpper(const JCharacter c);
JBoolean	JIsLower(const JCharacter c);

JCharacter	JToUpper(const JCharacter c);
JCharacter	JToLower(const JCharacter c);

JBoolean	JGetDiacriticalMap(const JCharacter** map, const JIndex** markType);
void		JSetDiacriticalMap(const JCharacter*  map, const JIndex*  markType);


/******************************************************************************
 Cast to JCharacter*

 ******************************************************************************/

inline
JString::operator const JCharacter*()
	const
{
	return itsString;
}

/******************************************************************************
 ContainsNULL

 ******************************************************************************/

inline JBoolean
JString::ContainsNULL()
	const
{
	return JConvertToBoolean( itsStringLength != strlen(itsString) );
}

/******************************************************************************
 GetCString

 ******************************************************************************/

inline const JCharacter*
JString::GetCString()
	const
{
	return itsString;
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

inline JBoolean
JString::IsEmpty()
	const
{
	return JConvertToBoolean( itsStringLength == 0 );
}

/******************************************************************************
 JStringEmpty

 ******************************************************************************/

inline JBoolean
JStringEmpty
	(
	const JCharacter* s
	)
{
	return JConvertToBoolean( s == NULL || s[0] == '\0' );
}

/******************************************************************************
 GetLength

 ******************************************************************************/

inline JSize
JString::GetLength()
	const
{
	return itsStringLength;
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
	return JString::ConvertToFloat(itsString, itsStringLength, value);
}

inline JBoolean
JString::ConvertToInteger
	(
	JInteger*	value,
	const JSize	origBase
	)
	const
{
	return JString::ConvertToInteger(itsString, itsStringLength, value, origBase);
}

inline JBoolean
JString::ConvertToUInt
	(
	JUInt*		value,
	const JSize	origBase
	)
	const
{
	return JString::ConvertToUInt(itsString, itsStringLength, value, origBase);
}

// static

inline JBoolean
JString::ConvertToFloat
	(
	const JCharacter*	str,
	JFloat*				value
	)
{
	return JString::ConvertToFloat(str, strlen(str), value);
}

inline JBoolean
JString::ConvertToInteger
	(
	const JCharacter*	str,
	JInteger*			value,
	const JSize			origBase
	)
{
	return JString::ConvertToInteger(str, strlen(str), value, origBase);
}

inline JBoolean
JString::ConvertToUInt
	(
	const JCharacter*	str,
	JUInt*				value,
	const JSize			origBase
	)
{
	return JString::ConvertToUInt(str, strlen(str), value, origBase);
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
	return ConvertToFloat(&value);
}

inline JBoolean
JString::IsInteger
	(
	const JSize base
	)
	const
{
	JInteger value;
	return ConvertToInteger(&value, base);
}

inline JBoolean
JString::IsUInt
	(
	const JSize base
	)
	const
{
	JUInt value;
	return ConvertToUInt(&value, base);
}

inline JBoolean
JString::IsHexValue()
	const
{
	return IsHexValue(itsString, itsStringLength);
}

// static

inline JBoolean
JString::IsFloat
	(
	const JCharacter* str
	)
{
	JFloat value;
	return ConvertToFloat(str, &value);
}

inline JBoolean
JString::IsInteger
	(
	const JCharacter*	str,
	const JSize			base
	)
{
	JInteger value;
	return ConvertToInteger(str, &value, base);
}

inline JBoolean
JString::IsUInt
	(
	const JCharacter*	str,
	const JSize			base
	)
{
	JUInt value;
	return ConvertToUInt(str, &value, base);
}

inline JBoolean
JString::IsHexValue
	(
	const JCharacter* str
	)
{
	return JString::IsHexValue(str, strlen(str));
}

/******************************************************************************
 GetFirstCharacter

 ******************************************************************************/

inline JCharacter
JString::GetFirstCharacter()
	const
{
	return GetCharacter(1);
}

/******************************************************************************
 GetLastCharacter

 ******************************************************************************/

inline JCharacter
JString::GetLastCharacter()
	const
{
	return GetCharacter(itsStringLength);
}

/******************************************************************************
 LocateSubstring

 ******************************************************************************/

inline JBoolean
JString::LocateSubstring
	(
	const JString&	str,
	JIndex*			startIndex
	)
	const
{
	return LocateSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString::LocateSubstring
	(
	const JCharacter*	str,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str, strlen(str), kJTrue, startIndex);
}

inline JBoolean
JString::LocateSubstring
	(
	const JCharacter*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString::LocateSubstring
	(
	const std::string&	str,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str.data(), str.length(), kJTrue, startIndex);
}

inline JBoolean
JString::LocateSubstring
	(
	const JString&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocateSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString::LocateSubstring
	(
	const JCharacter*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str, strlen(str), caseSensitive, startIndex);
}

inline JBoolean
JString::LocateSubstring
	(
	const std::string&	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str.data(), str.length(), caseSensitive, startIndex);
}

/******************************************************************************
 LocateNextSubstring

 ******************************************************************************/

inline JBoolean
JString::LocateNextSubstring
	(
	const JString&	str,
	JIndex*			startIndex
	)
	const
{
	return LocateNextSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString::LocateNextSubstring
	(
	const JCharacter*	str,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str, strlen(str), kJTrue, startIndex);
}

inline JBoolean
JString::LocateNextSubstring
	(
	const JCharacter*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString::LocateNextSubstring
	(
	const std::string&	str,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str.data(), str.length(), kJTrue, startIndex);
}

inline JBoolean
JString::LocateNextSubstring
	(
	const JString&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocateNextSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString::LocateNextSubstring
	(
	const JCharacter*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str, strlen(str), caseSensitive, startIndex);
}

inline JBoolean
JString::LocateNextSubstring
	(
	const std::string&	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str.data(), str.length(), caseSensitive, startIndex);
}

/******************************************************************************
 LocatePrevSubstring

 ******************************************************************************/

inline JBoolean
JString::LocatePrevSubstring
	(
	const JString&	str,
	JIndex*			startIndex
	)
	const
{
	return LocatePrevSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString::LocatePrevSubstring
	(
	const JCharacter*	str,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str, strlen(str), kJTrue, startIndex);
}

inline JBoolean
JString::LocatePrevSubstring
	(
	const JCharacter*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString::LocatePrevSubstring
	(
	const std::string&	str,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str.data(), str.length(), kJTrue, startIndex);
}

inline JBoolean
JString::LocatePrevSubstring
	(
	const JString&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocatePrevSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString::LocatePrevSubstring
	(
	const JCharacter*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str, strlen(str), caseSensitive, startIndex);
}

inline JBoolean
JString::LocatePrevSubstring
	(
	const std::string&	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str.data(), str.length(), caseSensitive, startIndex);
}

/******************************************************************************
 LocateLastSubstring

 ******************************************************************************/

inline JBoolean
JString::LocateLastSubstring
	(
	const JString&	str,
	JIndex*			startIndex
	)
	const
{
	return LocateLastSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString::LocateLastSubstring
	(
	const JCharacter*	str,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str, strlen(str), kJTrue, startIndex);
}

inline JBoolean
JString::LocateLastSubstring
	(
	const JCharacter*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString::LocateLastSubstring
	(
	const std::string&	str,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str.data(), str.length(), kJTrue, startIndex);
}

inline JBoolean
JString::LocateLastSubstring
	(
	const JString&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocateLastSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString::LocateLastSubstring
	(
	const JCharacter*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str, strlen(str), caseSensitive, startIndex);
}

inline JBoolean
JString::LocateLastSubstring
	(
	const std::string&	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str.data(), str.length(), caseSensitive, startIndex);
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
	JIndex i;
	return LocateSubstring(str, caseSensitive, &i);
}

inline JBoolean
JString::Contains
	(
	const JCharacter*	str,
	const JBoolean		caseSensitive
	)
	const
{
	JIndex i;
	return LocateSubstring(str, caseSensitive, &i);
}

inline JBoolean
JString::Contains
	(
	const JCharacter*	str,
	const JSize			length,
	const JBoolean		caseSensitive
	)
	const
{
	JIndex i;
	return LocateSubstring(str, length, caseSensitive, &i);
}

inline JBoolean
JString::Contains
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
	const
{
	JIndex i;
	return LocateSubstring(str, caseSensitive, &i);
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
	return BeginsWith(str, str.itsStringLength, caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const JCharacter*	str,
	const JBoolean		caseSensitive
	)
	const
{
	return BeginsWith(str, strlen(str), caseSensitive);
}

inline JBoolean
JString::BeginsWith
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
	const
{
	return BeginsWith(str.data(), str.length(), caseSensitive);
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
	return EndsWith(str, str.itsStringLength, caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const JCharacter*	str,
	const JBoolean		caseSensitive
	)
	const
{
	return EndsWith(str, strlen(str), caseSensitive);
}

inline JBoolean
JString::EndsWith
	(
	const std::string&	str,
	const JBoolean		caseSensitive
	)
	const
{
	return EndsWith(str.data(), str.length(), caseSensitive);
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
	InsertSubstring(str.itsString, str.itsStringLength, itsStringLength+1);
	return *this;
}

inline JString&
JString::operator+=
	(
	const JCharacter* str
	)
{
	InsertSubstring(str, strlen(str), itsStringLength+1);
	return *this;
}

inline JString&
JString::operator+=
	(
	const std::string& str
	)
{
	InsertSubstring(str.data(), str.length(), itsStringLength+1);
	return *this;
}

/******************************************************************************
 InsertSubstring

 ******************************************************************************/

inline void
JString::InsertSubstring
	(
	const JString&	str,
	const JIndex	insertionIndex
	)
{
	InsertSubstring(str.itsString, str.itsStringLength, insertionIndex);
}

inline void
JString::InsertSubstring
	(
	const JCharacter*	str,
	const JIndex		insertionIndex
	)
{
	InsertSubstring(str, strlen(str), insertionIndex);
}

inline void
JString::InsertCharacter
	(
	const JCharacter	c,
	const JIndex		insertionIndex
	)
{
	JCharacter str[] = { c, '\0' };
	InsertSubstring(str, 1, insertionIndex);
}

inline void
JString::InsertSubstring
	(
	const std::string&	str,
	const JIndex		insertionIndex
	)
{
	InsertSubstring(str.data(), str.length(), insertionIndex);
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
	InsertSubstring(str.itsString, str.itsStringLength, 1);
}

inline void
JString::Prepend
	(
	const JCharacter* str
	)
{
	InsertSubstring(str, strlen(str), 1);
}

inline void
JString::Prepend
	(
	const JCharacter*	str,
	const JSize			length
	)
{
	InsertSubstring(str, length, 1);
}

inline void
JString::PrependCharacter
	(
	const JCharacter c
	)
{
	JCharacter str[] = { c, '\0' };
	InsertSubstring(str, 1, 1);
}

inline void
JString::Prepend
	(
	const std::string& str
	)
{
	InsertSubstring(str.data(), str.length(), 1);
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
	InsertSubstring(str.itsString, str.itsStringLength, itsStringLength+1);
}

inline void
JString::Append
	(
	const JCharacter* str
	)
{
	InsertSubstring(str, strlen(str), itsStringLength+1);
}

inline void
JString::Append
	(
	const JCharacter*	str,
	const JSize			length
	)
{
	InsertSubstring(str, length, itsStringLength+1);
}

inline void
JString::AppendCharacter
	(
	const JCharacter c
	)
{
	JCharacter str[] = { c, '\0' };
	InsertSubstring(str, 1, itsStringLength+1);
}

inline void
JString::Append
	(
	const std::string& str
	)
{
	InsertSubstring(str.data(), str.length(), itsStringLength+1);
}

/******************************************************************************
 ReplaceSubstring

 ******************************************************************************/

inline void
JString::ReplaceSubstring
	(
	const JIndex	firstCharIndex,
	const JIndex	lastCharIndex,
	const JString&	str
	)
{
	ReplaceSubstring(firstCharIndex, lastCharIndex, str.itsString, str.itsStringLength);
}

inline void
JString::ReplaceSubstring
	(
	const JIndex		firstCharIndex,
	const JIndex		lastCharIndex,
	const JCharacter*	str
	)
{
	ReplaceSubstring(firstCharIndex, lastCharIndex, str, strlen(str));
}

inline void
JString::ReplaceSubstring
	(
	const JIndexRange&	range,
	const JCharacter*	str,
	const JSize			length,
	JIndexRange*		newRange
	)
{
	ReplaceSubstring(range.first, range.last, str, length);
	if (newRange != NULL)
		{
		newRange->SetFirstAndLength(range.first, length);
		}
}

inline void
JString::ReplaceSubstring
	(
	const JIndex		firstCharIndex,
	const JIndex		lastCharIndex,
	const std::string&	str
	)
{
	ReplaceSubstring(firstCharIndex, lastCharIndex, str.data(), str.length());
}

inline void
JString::ReplaceSubstring
	(
	const JIndexRange&	range,
	const JString&		str,
	JIndexRange*		newRange
	)
{
	ReplaceSubstring(range, str.itsString, str.itsStringLength, newRange);
}

inline void
JString::ReplaceSubstring
	(
	const JIndexRange&	range,
	const JCharacter*	str,
	JIndexRange*		newRange
	)
{
	ReplaceSubstring(range, str, strlen(str), newRange);
}

inline void
JString::ReplaceSubstring
	(
	const JIndexRange&	range,
	const std::string&	str,
	JIndexRange*		newRange
	)
{
	ReplaceSubstring(range, str.data(), str.length(), newRange);
}

/******************************************************************************
 RemoveSubstring

 ******************************************************************************/

inline void
JString::RemoveSubstring
	(
	const JIndex firstCharIndex,
	const JIndex lastCharIndex
	)
{
	ReplaceSubstring(firstCharIndex, lastCharIndex, "");
}

inline void
JString::RemoveSubstring
	(
	const JIndexRange& range
	)
{
	if (!range.IsEmpty())
		{
		ReplaceSubstring(range.first, range.last, "");
		}
}

/******************************************************************************
 MatchCase

 ******************************************************************************/

inline JBoolean
JString::MatchCase
	(
	const JString&		source,
	const JIndexRange&	sourceRange
	)
{
	return MatchCase(source.itsString, sourceRange, JIndexRange(1, itsStringLength));
}

inline JBoolean
JString::MatchCase
	(
	const JCharacter*	source,
	const JIndexRange&	sourceRange
	)
{
	return MatchCase(source, sourceRange, JIndexRange(1, itsStringLength));
}

inline JBoolean
JString::MatchCase
	(
	const std::string&	source,
	const JIndexRange&	sourceRange
	)
{
	return MatchCase(source.data(), sourceRange, JIndexRange(1, itsStringLength));
}

inline JBoolean
JString::MatchCase
	(
	const JString&		source,
	const JIndexRange&	sourceRange,
	const JIndexRange&	destRange
	)
{
	return MatchCase(source.itsString, sourceRange, destRange);
}

inline JBoolean
JString::MatchCase
	(
	const std::string&	source,
	const JIndexRange&	sourceRange,
	const JIndexRange&	destRange
	)
{
	return MatchCase(source.data(), sourceRange, destRange);
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline JBoolean
JString::IndexValid
	(
	const JIndex index
	)
	const
{
	return JI2B( 1 <= index && index <= itsStringLength );
}

/******************************************************************************
 RangeValid

 ******************************************************************************/

inline JBoolean
JString::RangeValid
	(
	const JIndexRange& range
	)
	const
{
	return JI2B(IndexValid(range.first) &&
				(range.IsEmpty() || IndexValid(range.last)));
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
 Addition

	We provide enough types so no constructors are called.

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
	const JCharacter*	str
	)
{
	JString sum = s;
	sum += str;
	return sum;
}

inline JString
operator+
	(
	const JCharacter*	str,
	const JString&		s
	)
{
	JString sum = str;
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
	JString sum = str;
	sum += s;
	return sum;
}

/******************************************************************************
 Equality (case-sensitive)

	We provide three types so no constructors are called.
	We don't need access to the private data because we have a conversion operator.

	*** We have to use the version of JStringCompare() that takes lengths
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
	return (JStringCompare(s1, s1.GetLength(), s2, s2.GetLength(), kJTrue) == 0);
}

inline int
operator==
	(
	const JString&		s,
	const JCharacter*	str
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJTrue) == 0);
}

inline int
operator==
	(
	const JCharacter*	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJTrue) == 0);
}

inline int
operator==
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJTrue) == 0);
}

inline int
operator==
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJTrue) == 0);
}

// operator!=

inline int
operator!=
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JStringCompare(s1, s1.GetLength(), s2, s2.GetLength(), kJTrue) != 0);
}

inline int
operator!=
	(
	const JString&		s,
	const JCharacter*	str
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJTrue) != 0);
}

inline int
operator!=
	(
	const JCharacter*	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJTrue) != 0);
}

inline int
operator!=
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJTrue) != 0);
}

inline int
operator!=
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJTrue) != 0);
}

/******************************************************************************
 Comparison (case-insensitive)

	We provide three types so no constructors are called.

	*** We have to use the version of JStringCompare() that takes lengths
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
	return (JStringCompare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) < 0);
}

inline int
operator<
	(
	const JString&		s,
	const JCharacter*	str
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) < 0);
}

inline int
operator<
	(
	const JCharacter*	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) < 0);
}

inline int
operator<
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) < 0);
}

inline int
operator<
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) < 0);
}

// operator<=

inline int
operator<=
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JStringCompare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JString&		s,
	const JCharacter*	str
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JCharacter*	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) <= 0);
}

inline int
operator<=
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) <= 0);
}

// operator>

inline int
operator>
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JStringCompare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) > 0);
}

inline int
operator>
	(
	const JString&		s,
	const JCharacter*	str
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) > 0);
}

inline int
operator>
	(
	const JCharacter*	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) > 0);
}

inline int
operator>
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) > 0);
}

inline int
operator>
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) > 0);
}

// operator>=

inline int
operator>=
	(
	const JString& s1,
	const JString& s2
	)
{
	return (JStringCompare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JString&		s,
	const JCharacter*	str
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JCharacter*	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str, strlen(str), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JString&		s,
	const std::string&	str
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) >= 0);
}

inline int
operator>=
	(
	const std::string&	str,
	const JString&		s
	)
{
	return (JStringCompare(s, s.GetLength(), str.data(), str.length(), kJFalse) >= 0);
}

/******************************************************************************
 JStringCompare

	*** We have to use the version of JStringCompare() that takes lengths
		because JStrings can contain NULLs.

 ******************************************************************************/

inline int
JStringCompare
	(
	const JString&	s1,
	const JString&	s2,
	const JBoolean	caseSensitive
	)
{
	return JStringCompare(s1, s1.GetLength(), s2, s2.GetLength(), caseSensitive);
}

inline int
JStringCompare
	(
	const JString&		s1,
	const JCharacter*	s2,
	const JBoolean		caseSensitive
	)
{
	return JStringCompare(s1, s1.GetLength(), s2, strlen(s2), caseSensitive);
}

inline int
JStringCompare
	(
	const JCharacter*	s1,
	const JString&		s2,
	const JBoolean		caseSensitive
	)
{
	return JStringCompare(s1, strlen(s1), s2, s2.GetLength(), caseSensitive);
}

inline int
JStringCompare
	(
	const JString&		s1,
	const std::string&	s2,
	const JBoolean		caseSensitive
	)
{
	return JStringCompare(s1, s1.GetLength(), s2.data(), s2.length(), caseSensitive);
}

inline int
JStringCompare
	(
	const std::string&	s1,
	const JString&		s2,
	const JBoolean		caseSensitive
	)
{
	return JStringCompare(s1.data(), s1.length(), s2, s2.GetLength(), caseSensitive);
}

/******************************************************************************
 CopyToPrivateString (private)

	Copy the given string into our private string.

 ******************************************************************************/

inline void
JString::CopyToPrivateString
	(
	const JCharacter* str
	)
{
	CopyToPrivateString(str, strlen(str));
}

/******************************************************************************
 PrivateGetCharacter (private)

	*** This routine does no bounds checking!

 ******************************************************************************/

inline JCharacter
JString::PrivateGetCharacter
	(
	const JIndex index
	)
	const
{
	return itsString [ index - 1 ];
}

/******************************************************************************
 GetCharacterPtr (private)

	*** This routine does no bounds checking!

	Return a pointer to the character at the specified index.

 ******************************************************************************/

inline JCharacter*
JString::GetCharacterPtr
	(
	const JIndex index
	)
	const
{
	return itsString + index - 1;
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
	if (this->itsString != str.itsString)
		{
		CopyToPrivateString(str.itsString, str.itsStringLength);
		}
}

inline void
JString::Set
	(
	const JCharacter* str
	)
{
	if (this->itsString != str)
		{
		CopyToPrivateString(str);
		}
}

inline void
JString::Set
	(
	const JCharacter*	str,
	const JSize			length
	)
{
	if (this->itsString != str)
		{
		CopyToPrivateString(str, length);
		}
}

inline void
JString::Set
	(
	const JCharacter*	str,
	const JIndexRange&	range
	)
{
	if (this->itsString != str)
		{
		if (range.IsNothing())
			{
			Clear();
			}
		else
			{
			CopyToPrivateString(str + range.first-1, range.GetLength());
			}
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
	const std::string&	str,
	const JIndexRange&	range
	)
{
	if (range.IsNothing())
		{
		Clear();
		}
	else
		{
		CopyToPrivateString(str.data() + range.first-1, range.GetLength());
		}
}

/******************************************************************************
 Assignment operator

	We do not copy itsBlockSize because we assume the client has set them
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
	const JCharacter* str
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

#endif
