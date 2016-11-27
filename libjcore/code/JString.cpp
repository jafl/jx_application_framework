/******************************************************************************
 JString.cpp

	This class was not designed to be a base class!  If you need to override it,
	be sure to make the destructor virtual.

	JStrings can contain NULL's, if they are constructed with the
	JString(const JUtf8Byte* str, const JSize byteCount) constructor.
	You must remember not to call Clib functions on the const JUtf8Byte* in
	this case.

	Note that operator== is case sensitive, as one would expect.  To avoid the
	UNIX method of sorting capitalized names separately in front of lowercase
	names, operator< and operator> are not case sensitive.  One should therefore
	not mix == with < and > when comparing strings.

	Since strstream doesn't provide the control we need when converting a number
	to a string, we use the NumConvert and StrUtil modules.  We include them at
	the end of the file so they are completely hidden and JString is self-contained.

	BASE CLASS = none

	Copyright (C) 1994-2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JString.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <JMinMax.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>
#include <strstream>
#include <iomanip>
#include <unicode/ucol.h>
#include <jErrno.h>
#include <jAssert.h>

JSize JString::theDefaultBlockSize = 256;

// private routines

static void	double2str(double doubleVal, int afterDec, int sigDigitCount,
					   int expMin, char *returnStr);

/******************************************************************************
 Constructor

 ******************************************************************************/

JString::JString()
	:
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(theDefaultBlockSize),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	itsBytes = jnew JUtf8Byte [ itsAllocCount+1 ];
	assert( itsBytes != NULL );
	itsBytes[0] = '\0';
}

JString::JString
	(
	const JString&			str,
	const JCharacterRange&	charRange
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	JUtf8ByteRange byteRange = str.CharacterToUtf8ByteRange(charRange);
	CopyToPrivateString(str.GetBytes() + byteRange.first-1, byteRange.GetLength());
}

// This unfortunately allows implicit conversion, but not being able to
// write [JString s = "abc"] is just too annoying.

JString::JString
	(
	const JUtf8Byte* str
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	CopyToPrivateString(str, strlen(str));
}

JString::JString
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	CopyToPrivateString(byteCount > 0 ? str : "", byteCount);		// allow (NULL,0)
}

JString::JString
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	CopyToPrivateString(str + range.first-1, range.GetLength());
}

// This unfortunately allows round-about implicit conversion, but not being
// able to write [JString s = "abc"] is just too annoying.

JString::JString
	(
	const std::string& s
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	CopyToPrivateString(s.data(), s.length());
}

JString::JString
	(
	const std::string&		s,
	const JUtf8ByteRange&	range
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	CopyToPrivateString(s.data() + range.first-1, range.GetLength());
}

JString::JString
	(
	const JUInt64	number,
	const Base		base,
	const JBoolean	pad
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	if (number == 0)
		{
		CopyToPrivateString("0", 1);
		}
	else if (base == 2)
		{
		CopyToPrivateString("", 0);

		JUInt64 v = number;
		do
			{
			if (v & 0x01)
				{
				Prepend("1");
				}
			else
				{
				Prepend("0");
				}
			v >>= 1;
			}
			while (v != 0);

		while (pad && itsByteCount % 8 > 0)
			{
			Prepend("0");
			}
		}
	else
		{
		std::ostringstream s;
		s << std::setbase(base) << number;
		const std::string s1 = s.str();
		CopyToPrivateString(s1.data(), s1.length());
		}

	if (base == 16)
		{
		if (pad && itsByteCount % 2 == 1)
			{
			Prepend("0");
			}

		ToUpper();
		Prepend("0x");
		}
}

JString::JString
	(
	const JFloat			number,
	const JInteger			precision,
	const ExponentDisplay	expDisplay,
	const JInteger			exponent,
	const JInteger			sigDigitCount
	)
	:
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(theDefaultBlockSize),
	itsBlockSize(theDefaultBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	assert( precision >= -1 );

	itsBytes = jnew JUtf8Byte [ itsAllocCount+1 ];
	assert( itsBytes != NULL );
	double2str(number, precision, sigDigitCount,
			   (expDisplay == kUseGivenExponent ? exponent : expDisplay),
			   itsBytes);

	itsByteCount      = strlen(itsBytes);
	itsCharacterCount = CountCharacters(itsBytes, itsByteCount);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JString::JString
	(
	const JString& source
	)
	:
	itsBytes(NULL),		// makes delete [] safe inside CopyToPrivateString
	itsByteCount(0),
	itsCharacterCount(0),
	itsAllocCount(0),
	itsBlockSize(source.itsBlockSize),
	itsUCaseMap(NULL),
	itsFirstIterator(NULL)
{
	CopyToPrivateString(source.itsBytes, source.itsByteCount);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JString::~JString()
{
	jdelete [] itsBytes;
	ucasemap_close(itsUCaseMap);

	// TODO: invalidate iterators
}

/******************************************************************************
 CopyToPrivateString (private)

	Copy the given string into our private string.

	*** This function requires that str != itsBytes

 ******************************************************************************/

void
JString::CopyToPrivateString
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	assert( itsBytes == NULL || !(itsBytes <= str && str < itsBytes + itsByteCount) );

	// ensure sufficient space

	if (itsAllocCount < byteCount || itsAllocCount == 0)
		{
		itsAllocCount = byteCount + itsBlockSize;

		JUtf8Byte* newString = jnew JUtf8Byte [ itsAllocCount + 1 ];
		assert( newString != NULL );

		jdelete [] itsBytes;
		itsBytes = newString;
		}

	// copy normalized characters to the new string

	if (str != NULL)
		{
		itsByteCount      = CopyNormalizedBytes(str, byteCount, itsBytes, itsAllocCount+1);
		itsCharacterCount = CountCharacters(itsBytes, itsByteCount);
		}
	else
		{
		itsBytes[0]       = 0;
		itsByteCount      = 0;
		itsCharacterCount = 0;
		}

	// TODO: notify iterators
}

/******************************************************************************
 GetFirstCharacter

 ******************************************************************************/

JUtf8Character
JString::GetFirstCharacter()
	const
{
	assert( itsCharacterCount > 0 );
	return JUtf8Character(itsBytes);
}

/******************************************************************************
 GetLastCharacter

 ******************************************************************************/

JUtf8Character
JString::GetLastCharacter()
	const
{
	assert( itsCharacterCount > 0 );

	const unsigned char* s = (const unsigned char*) (itsBytes + itsByteCount - 1);
	while (((unsigned char) '\x80') <= *s && *s <= (unsigned char) '\xBF')
		{
		s--;
		}

	return JUtf8Character((const JUtf8Byte*) s);
}

/******************************************************************************
 AllocateBytes

	This allocates a new pointer, which the caller is responsible
	for deleting via "delete []".

 ******************************************************************************/

JUtf8Byte*
JString::AllocateBytes()
	const
{
	JUtf8Byte* str = jnew JUtf8Byte [ itsByteCount + 1 ];
	assert( str != NULL );

	memcpy(str, itsBytes, itsByteCount+1);

	return str;
}

/******************************************************************************
 NotifyIterators (private)

 ******************************************************************************/

void
JString::NotifyIterators
	(
	const JBroadcaster::Message& message
	)
{
	if (itsFirstIterator != NULL)
		{
//		itsFirstIterator->StringChanged(message);	// TODO
		}
}

/******************************************************************************
 Prepend

 ******************************************************************************/

void
JString::Prepend
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	InsertBytes(1, str, byteCount);
}

/******************************************************************************
 Append

 ******************************************************************************/

void
JString::Append
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	InsertBytes(itsByteCount+1, str, byteCount);
}

/******************************************************************************
 InsertBytes (private)

	Insert the given bytes into ourselves starting at the specified index.

	// TODO: CopyNormalizedBytes

 ******************************************************************************/

void
JString::InsertBytes
	(
	const JIndex		insertionIndex,
	const JUtf8Byte*	stringToInsert,
	const JSize			insertLength
	)
{
	assert( 0 < insertionIndex && insertionIndex <= itsByteCount + 1 );

	// if the string to insert is empty, we don't need to do anything

	if (insertLength == 0)
		{
		return;
		}

	// If we don't have space for the result, we have to reallocate.

	JUtf8Byte* insertionPtr               = NULL;
	const JUnsignedOffset insertionOffset = insertionIndex - 1;

	if (itsAllocCount < itsByteCount + insertLength)
		{
		itsAllocCount = itsByteCount + insertLength + itsBlockSize;

		// allocate space for the combined string

		JUtf8Byte* newString = jnew JUtf8Byte [ itsAllocCount + 1 ];
		assert( newString != NULL );

		insertionPtr = newString + insertionOffset;

		// copy our characters in front of the insertion point

		memcpy(newString, itsBytes, insertionOffset);

		// copy our characters after the insertion point, -including the termination-

		memcpy(insertionPtr + insertLength, itsBytes + insertionOffset,
			   itsByteCount - insertionOffset + 1);

		// throw out our original string and save the new one

		jdelete [] itsBytes;
		itsBytes = newString;
		}

	// Otherwise, just shift characters to make space for the result.

	else if (insertionOffset < itsByteCount+1)
		{
		insertionPtr = itsBytes + insertionOffset;

		// shift characters after the insertion point, *including the termination*

		memmove(insertionPtr + insertLength, insertionPtr,
				itsByteCount - insertionOffset + 1);
		}

	// copy the characters from the string to insert

	memcpy(insertionPtr, stringToInsert, insertLength);

	// update our string length

	itsByteCount      += insertLength;
	itsCharacterCount += CountCharacters(stringToInsert, insertLength);

	// TODO: notify iterators
}

/******************************************************************************
 ReplaceBytes (private)

	Replace the specified range with the given bytes.

	// TODO: CopyNormalizedBytes

 ******************************************************************************/

void
JString::ReplaceBytes
	(
	const JIndex		firstByteIndex,
	const JIndex		lastByteIndex,
	const JUtf8Byte*	stringToInsert,
	const JSize			insertLength
	)
{
	assert( firstByteIndex <= lastByteIndex );
	assert( ByteIndexValid(firstByteIndex) );
	assert( ByteIndexValid(lastByteIndex) );

	const JSize replaceLength = lastByteIndex - firstByteIndex + 1;
	const JSize newLength     = itsByteCount - replaceLength + insertLength;

	const JSize len1 = firstByteIndex - 1;
	const JSize len2 = insertLength;
	const JSize len3 = itsByteCount - lastByteIndex;

	itsCharacterCount -= CountCharacters(itsBytes + len1, replaceLength);

	// If we don't have space, or would use too much space, reallocate.

	if (itsAllocCount < newLength || itsAllocCount > newLength + itsBlockSize)
		{
		itsAllocCount = newLength + itsBlockSize;

		// allocate space for the result

		JUtf8Byte* newString = jnew JUtf8Byte[ itsAllocCount+1 ];
		assert( newString != NULL );

		// place the characters in front and behind

		memcpy(newString, itsBytes, len1);
		memcpy(newString + len1 + len2, itsBytes + lastByteIndex, len3);

		// throw out the original string and save the new one

		jdelete [] itsBytes;
		itsBytes = newString;
		}

	// Otherwise, shift characters to make space.

	else if (len3 > 0)
		{
		memmove(itsBytes + len1 + len2, itsBytes + lastByteIndex, len3);
		}

	// insert the new characters

	memcpy(itsBytes + len1, stringToInsert, len2);

	// terminate

	itsBytes[ newLength ] = '\0';
	itsByteCount          = newLength;
	itsCharacterCount    += CountCharacters(stringToInsert, insertLength);

	// TODO: notify iterators
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JString::Clear()
{
	// there is nothing to do if we are already empty

	if (itsByteCount == 0)
		{
		return;
		}

	// If we are using too much memory, reallocate.

	if (itsAllocCount > itsBlockSize)
		{
		itsAllocCount = itsBlockSize;

		// throw out the old data

		jdelete [] itsBytes;

		// Having just released a block of memory at least as large as the
		// one we are requesting, the system must really be screwed if this
		// call to new doesn't work.

		itsBytes = jnew JUtf8Byte [ itsAllocCount + 1 ];
		assert( itsBytes != NULL );
		}

	// clear the string

	itsBytes[0]       = '\0';
	itsByteCount      = 0;
	itsCharacterCount = 0;

	// TODO: notify iterators
}

/******************************************************************************
 TrimWhitespace

	Trim leading and trailing whitespace from ourselves.

	TODO: utf8

 ******************************************************************************/

void
JString::TrimWhitespace()
{
	// there is nothing to do if we are already empty

	if (itsByteCount == 0)
		{
		return;
		}

	// if there is no blank space to trim, we can stop now

	if (!isspace(itsBytes[0]) &&
		!isspace(itsBytes[ itsByteCount-1 ]))
		{
		return;
		}

	JSize wsCount = 0;

	// find last non-blank character

	JIndex lastByteIndex = itsByteCount;
	while (lastByteIndex > 0 && isspace(itsBytes[ lastByteIndex-1 ]))
		{
		lastByteIndex--;
		wsCount++;
		}

	// if we are only blank space, we can just clear ourselves

	if (lastByteIndex == 0)
		{
		Clear();
		return;
		}

	// find first non-blank character (it does exist since lastByteIndex > 0)

	JIndex firstByteIndex = 1;
	while (isspace(itsBytes[ firstByteIndex-1 ]))
		{
		firstByteIndex++;
		wsCount++;
		}

	// If we are using too much memory, reallocate.

	const JSize newLength = lastByteIndex - firstByteIndex + 1;

	if (itsAllocCount > newLength + itsBlockSize)
		{
		itsAllocCount = newLength + itsBlockSize;

		// allocate space for the new string + termination

		JUtf8Byte* newString = jnew JUtf8Byte[ itsAllocCount+1 ];
		assert( newString != NULL );

		// copy the non-blank characters to the new string

		memcpy(newString, itsBytes + firstByteIndex-1, newLength);

		// throw out our original string and save the new one

		jdelete [] itsBytes;
		itsBytes = newString;
		}

	// Otherwise, just shift the characters.

	else
		{
		memmove(itsBytes, itsBytes + firstByteIndex-1, newLength);
		}

	// terminate

	itsBytes[ newLength ] = '\0';
	itsByteCount          = newLength;
	itsCharacterCount    -= wsCount;

	// TODO: notify iterators
}

/******************************************************************************
 ToLower

	Convert all characters to lower case.

 ******************************************************************************/

void
JString::ToLower()
{
	FoldCase(kJFalse);
}

/******************************************************************************
 ToUpper

	Convert all characters to upper case.

 ******************************************************************************/

void
JString::ToUpper()
{
	FoldCase(kJTrue);
}

/******************************************************************************
 FoldCase (private)

 ******************************************************************************/

void
JString::FoldCase
	(
	const JBoolean upper
	)
{
	if (itsByteCount == 0)
		{
		return;
		}

	UErrorCode err;

	if (itsUCaseMap == NULL)
		{
		err         = U_ZERO_ERROR;
		itsUCaseMap = ucasemap_open(NULL, U_FOLD_CASE_DEFAULT, &err);
		assert( err == U_ZERO_ERROR );
		}

	JUInt32 newLength;
	err = U_ZERO_ERROR;
	if (upper)
		{
		newLength = ucasemap_utf8ToUpper(itsUCaseMap, NULL, 0, itsBytes, itsByteCount, &err);
		}
	else
		{
		newLength = ucasemap_utf8ToLower(itsUCaseMap, NULL, 0, itsBytes, itsByteCount, &err);
		}
	assert( err == U_BUFFER_OVERFLOW_ERROR );

	itsAllocCount = newLength + itsBlockSize;

	// allocate space for the result

	JUtf8Byte* newString = jnew JUtf8Byte[ itsAllocCount+1 ];
	assert( newString != NULL );

	// get the new string

	err = U_ZERO_ERROR;
	if (upper)
		{
		ucasemap_utf8ToUpper(itsUCaseMap, newString, itsAllocCount+1, itsBytes, itsByteCount, &err);
		}
	else
		{
		ucasemap_utf8ToLower(itsUCaseMap, newString, itsAllocCount+1, itsBytes, itsByteCount, &err);
		}
	assert( err == U_ZERO_ERROR );

	// throw out the original string and save the new one

	jdelete [] itsBytes;
	itsBytes = newString;

	// TODO: notify iterators
}

/******************************************************************************
 SearchForward (private)

	Return the byte index corresponding to the start of the next occurrence
	of the given sequence in our string, starting at *byteIndex.

	In:  *byteIndex is first location to consider
	Out: If function returns kJTrue, *byteIndex is location of next occurrence.
		 Otherwise, *byteIndex is beyond end of string.

 ******************************************************************************/

JBoolean
JString::SearchForward
	(
	const JUtf8Byte*	str,
	const JSize			strLength,
	const JBoolean		caseSensitive,
	JIndex*				byteIndex
	)
	const
{
	if (IsEmpty())
		{
		*byteIndex = 1;
		return kJFalse;
		}
	else if (*byteIndex > itsByteCount)
		{
		return kJFalse;
		}

	assert( *byteIndex != 0 );

	// if the given string is longer than we are, we can't contain it

	if (itsByteCount - *byteIndex + 1 < strLength)
		{
		*byteIndex = itsByteCount+1;
		return kJFalse;
		}

	// search forward for a match

	UErrorCode err  = U_ZERO_ERROR;
	UCollator* coll = ucol_open(NULL, &err);
	if (coll == NULL)
		{
		return kJFalse;
		}

	if (!caseSensitive)
		{
		ucol_setStrength(coll, UCOL_PRIMARY);
		}

	for (JIndex i=*byteIndex; i<=itsByteCount - strLength + 1; i++)
		{
		const UCollationResult r = ucol_strcollUTF8(coll, itsBytes + i-1, strLength, str, strLength, &err);
		if (r == 0)
			{
			*byteIndex = i;
			ucol_close(coll);
			return kJTrue;
			}
		}

	ucol_close(coll);

	// if we fall through, there was no match

	*byteIndex = itsByteCount+1;
	return kJFalse;
}

/******************************************************************************
 SearchBackward (private)

	Return the byte index corresponding to the start of the previous
	occurrence of the given sequence in our string, starting at *byteIndex.

	In:  *byteIndex is first location to consider
	Out: If function returns kJTrue, *byteIndex is location of prev occurrence.
		 Otherwise, *byteIndex is zero.

 ******************************************************************************/

JBoolean
JString::SearchBackward
	(
	const JUtf8Byte*	str,
	const JSize			strLength,
	const JBoolean		caseSensitive,
	JIndex*				byteIndex
	)
	const
{
	if (IsEmpty() || *byteIndex == 0)
		{
		*byteIndex = 0;
		return kJFalse;
		}

	assert( *byteIndex <= itsByteCount );

	// if the given string runs past our end, back up *byteIndex

	const JSize spaceAtEnd = itsByteCount - *byteIndex + 1;
	if (spaceAtEnd < strLength && itsByteCount >= strLength)
		{
		*byteIndex = itsByteCount - strLength + 1;
		}
	else if (spaceAtEnd < strLength)
		{
		*byteIndex = 0;
		return kJFalse;
		}

	// search backward for a match

	UErrorCode err  = U_ZERO_ERROR;
	UCollator* coll = ucol_open(NULL, &err);
	if (coll == NULL)
		{
		return kJFalse;
		}

	if (!caseSensitive)
		{
		ucol_setStrength(coll, UCOL_PRIMARY);
		}

	for (JIndex i=*byteIndex; i>=1; i--)
		{
		const UCollationResult r = ucol_strcollUTF8(coll, itsBytes + i-1, strLength, str, strLength, &err);
		if (r == 0)
			{
			*byteIndex = i;
			ucol_close(coll);
			return kJTrue;
			}
		}

	ucol_close(coll);

	// if we fall through, there was no match

	*byteIndex = 0;
	return kJFalse;
}

/******************************************************************************
 BeginsWith

 ******************************************************************************/

JBoolean
JString::BeginsWith
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	if (range.GetLength() == 0)
		{
		return kJTrue;
		}
	else
		{
		JIndex i = 1;
		return SearchBackward(str + range.first-1, range.GetLength(), caseSensitive, &i);
		}
}

/******************************************************************************
 Contains

 ******************************************************************************/

JBoolean
JString::Contains
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	if (range.GetLength() == 0)
		{
		return kJTrue;
		}
	else
		{
		JIndex i = 1;
		return SearchForward(str + range.first-1, range.GetLength(), caseSensitive, &i);
		}
}

/******************************************************************************
 EndsWith

 ******************************************************************************/

JBoolean
JString::EndsWith
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range,
	const JBoolean			caseSensitive
	)
	const
{
	if (range.GetLength() == 0)
		{
		return kJTrue;
		}
	else if (itsByteCount >= range.GetLength())
		{
		JIndex i = itsByteCount - range.GetLength() + 1;
		return SearchForward(str + range.first-1, range.GetLength(), caseSensitive, &i);
		}
	else
		{
		return JI2B(Compare(itsBytes, itsByteCount,
							str + range.first-1, range.GetLength(),
							caseSensitive) == 0);
		}
}

/******************************************************************************
 ConvertTo*

	We cannot provide a public interface that takes JUtf8Byte* + byteCount
	because strto*() does not provide this interface.  We use it internally
	as an optimization since JStrings know their length.

	If we tried to provide the JUtf8Byte* + byteCount interface, we could
	not stop strto*() from gobbling digits beyond byteCount, which would
	obviously be an error.

 ******************************************************************************/

/******************************************************************************
 ConvertToFloat (static private)

	Convert string to a floating point value.  Returns kJTrue if successful.

	This function accepts hex values like 0x2A.

 ******************************************************************************/

JBoolean
JString::ConvertToFloat
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	JFloat*				value
	)
{
	if (IsHexValue(str, byteCount))
		{
		JUInt v;
		const JBoolean ok = ConvertToUInt(str, byteCount, &v);
		*value = v;
		return ok;
		}

	jclear_errno();
	char* endPtr;
	*value = strtod(str, &endPtr);
	if (jerrno_is_clear() && CompleteConversion(str, byteCount, endPtr))
		{
		return kJTrue;
		}
	else
		{
		*value = 0.0;
		return kJFalse;
		}
}

/******************************************************************************
 ConvertToInteger (static private)

	Convert string to a signed integer.  Returns kJTrue if successful.

	base must be between 2 and 36 inclusive.
	If the string begins with 0x or 0X, base is forced to 16.

 ******************************************************************************/

JBoolean
JString::ConvertToInteger
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	JInteger*			value,
	const JSize			origBase
	)
{
	JSize base = origBase;
	if (IsHexValue(str, byteCount))
		{
		base = 0;	// let strtol notice "0x"
		}

	jclear_errno();
	char* endPtr;
	*value = strtol(str, &endPtr, base);
	if (jerrno_is_clear() && CompleteConversion(str, byteCount, endPtr))
		{
		return kJTrue;
		}
	else
		{
		*value = 0;
		return kJFalse;
		}
}

/******************************************************************************
 ConvertToUInt (static private)

	Convert string to an unsigned integer.  Returns kJTrue if successful.

	base must be between 2 and 36 inclusive.
	If the string begins with 0x or 0X, base is forced to 16.

 ******************************************************************************/

JBoolean
JString::ConvertToUInt
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	JUInt*				value,
	const JSize			origBase
	)
{
	JSize base = origBase;
	if (IsHexValue(str, byteCount))
		{
		base = 0;	// let strtoul notice "0x"
		}
	else
		{
		// We do not let strtoul() wrap negative numbers.

		JIndex i=0;
		while (i < byteCount && isspace(str[i]))
			{
			i++;
			}
		if (i < byteCount && str[i] == '-')
			{
			*value = 0;
			return kJFalse;
			}
		}

	jclear_errno();
	char* endPtr;
	*value = strtoul(str, &endPtr, base);
	if (jerrno_is_clear() && CompleteConversion(str, byteCount, endPtr))
		{
		return kJTrue;
		}
	else
		{
		*value = 0;
		return kJFalse;
		}
}

/******************************************************************************
 IsHexValue (static private)

	Returns kJTrue if string begins with whitespace+"0x".

 ******************************************************************************/

JBoolean
JString::IsHexValue
	(
	const JUtf8Byte*	str,
	const JSize			byteCount
	)
{
	JIndex i=0;
	while (i < byteCount && isspace(str[i]))
		{
		i++;
		}

	return JI2B(i < byteCount-2 && str[i] == '0' &&
				(str[i+1] == 'x' || str[i+1] == 'X'));
}

/******************************************************************************
 CompleteConversion (static private)

	Returns kJTrue if convEndPtr is beyond the end of the string or the
	remainder is only whitespace.

 ******************************************************************************/

JBoolean
JString::CompleteConversion
	(
	const JUtf8Byte*	startPtr,
	const JSize			byteCount,
	const JUtf8Byte*	convEndPtr
	)
{
	if (convEndPtr == startPtr)		// avoid behavior guaranteed by strto*()
		{
		return kJFalse;
		}

	const JUtf8Byte* endPtr = startPtr + byteCount;
	while (convEndPtr < endPtr)
		{
		if (!isspace(*convEndPtr))
			{
			return kJFalse;
			}
		convEndPtr++;
		}
	return kJTrue;
}

/******************************************************************************
 EncodeBase64

 ******************************************************************************/

JString
JString::EncodeBase64()
	const
{
	std::istrstream input(itsBytes, itsByteCount);
	std::ostrstream output;
	JEncodeBase64(input, output);
	return JString(output.str(), output.pcount());
}

/******************************************************************************
 DecodeBase64

 ******************************************************************************/

JBoolean
JString::DecodeBase64
	(
	JString* str
	)
	const
{
	std::istrstream input(itsBytes, itsByteCount);
	std::ostrstream output;
	if (JDecodeBase64(input, output))
		{
		str->Set(output.str(), output.pcount());
		return kJTrue;
		}
	else
		{
		str->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 Read

	Read the specified number of characters from the stream.
	This replaces the current contents of the string.

	// TODO: read characters instead of bytes
	// TODO: read only complete characters

 ******************************************************************************/

void
JString::Read
	(
	istream&	input,
	const JSize	count
	)
{
	if (itsAllocCount < count || itsAllocCount == 0)
		{
		itsAllocCount = count + itsBlockSize;

		// We allocate the new memory first.
		// If new fails, we still have the old string data.

		JUtf8Byte* newString = jnew JUtf8Byte [ itsAllocCount + 1 ];
		assert( newString != NULL );

		// now it's safe to throw out the old data

		jdelete [] itsBytes;
		itsBytes = newString;
		}

	input.read(itsBytes, count);
	itsByteCount             = input.gcount();
	itsBytes[ itsByteCount ] = '\0';
	itsCharacterCount        = CountCharacters(itsBytes, itsByteCount);

	// TODO: notify iterators
}

/******************************************************************************
 ReadDelimited

	Read a string of quote delimited characters from the stream.  Double
	quote (") and backslash (\) characters that are part of the string must
	be preceded by a backslash.  This replaces the current contents of the
	string.

 ******************************************************************************/

void
JString::ReadDelimited
	(
	istream& input
	)
{
	// skip white space

	input >> ws;

	// the string must start with a double quote

	JUtf8Byte c;
	input.get(c);
	if (c != '"')
		{
		input.putback(c);
		JSetState(input, ios::failbit);
		return;
		}

	// read until we hit a non-backslashed double quote

	Clear();

	const JSize bufSize = 100;
	JUtf8Byte buf[ bufSize ];

	JIndex i = 0;
	while (1)
		{
		input.get(c);
		if (input.fail())
			{
			break;
			}
		else if (input.eof())
			{
			JSetState(input, ios::failbit);
			break;
			}
		else if (c == '\\')
			{
			input.get(c);
			if (input.fail())
				{
				break;
				}
			}
		else if (c == '"')
			{
			break;
			}

		buf[i] = c;
		i++;
		if (i == bufSize)
			{
			Append(buf, bufSize);	// TODO: append only complete characters
			i = 0;
			}
		}

	Append(buf, i);
}

/******************************************************************************
 Print

	Display the text in such a way that the user can understand it.
	(i.e. don't display it in quotes, and don't use the internal \")

 ******************************************************************************/

void
JString::Print
	(
	ostream& output
	)
	const
{
	output.write(itsBytes, itsByteCount);
}

/******************************************************************************
 PrintHex

	Display the hex bytes.

 ******************************************************************************/

void
JString::PrintHex
	(
	ostream& output
	)
	const
{
	for (JIndex i=0; i<itsByteCount; i++)
		{
		output << std::hex << (int) (unsigned char) itsBytes[i] << std::dec << ' ';
		}
}

/******************************************************************************
 IsValid (static)

 ******************************************************************************/

JBoolean
JString::IsValid
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range
	)
{
	JSize charCount = 0;
	JSize byteCount;
	JIndex i;
	for (i = range.first-1; i < range.last; )
		{
		if (!JUtf8Character::GetCharacterByteCount(str + i, &byteCount))
			{
			return kJFalse;
			}
		charCount++;
		i += byteCount;
		}

	return JI2B(i == range.last);
}

/******************************************************************************
 CountCharacters (static)

 ******************************************************************************/

JSize
JString::CountCharacters
	(
	const JUtf8Byte*		str,
	const JUtf8ByteRange&	range
	)
{
	JSize charCount = 0;
	JSize byteCount;
	for (JIndex i = range.first-1; i < range.last; )
		{
		// accept invalid byte sequences as single characters
		JUtf8Character::GetCharacterByteCount(str + i, &byteCount);
		charCount++;
		i += byteCount;
		}

	return charCount;
}

/******************************************************************************
 CharacterToUtf8ByteRange (static)

 ******************************************************************************/

JUtf8ByteRange
JString::CharacterToUtf8ByteRange
	(
	const JUtf8Byte*		str,
	const JCharacterRange&	range
	)
{
	JSize charCount = 0, startByte = 0;

	JIndex i = 1;
	JSize byteCount;
	while (str[ i-1 ] != 0)
		{
		if (charCount >= range.first-1)
			{
			startByte = i;
			break;
			}

		// accept invalid byte sequences as single characters
		JUtf8Character::GetCharacterByteCount(str + i-1, &byteCount);
		charCount++;
		i += byteCount;
		}

	if (startByte == 0)
		{
		return JUtf8ByteRange();
		}

	JUtf8ByteRange r(startByte, startByte);

	charCount = range.GetLength();
	for (JIndex i=1; i<=charCount; i++)
		{
		// accept invalid byte sequences as single characters
		JUtf8Character::GetCharacterByteCount(str + r.last-1, &byteCount);
		r.last += byteCount;
		}

	r.last--;
	return r;
}

/******************************************************************************
 Compare (static)

	Replaces strcmp(): + if s1>s2, 0 if s1==s2, - if s1<s2

 ******************************************************************************/

int
JString::Compare
	(
	const JUtf8Byte*	s1,
	const JSize			length1,
	const JUtf8Byte*	s2,
	const JSize			length2,
	const JBoolean		caseSensitive
	)
{
	UErrorCode err  = U_ZERO_ERROR;
	UCollator* coll = ucol_open(NULL, &err);
	if (coll == NULL)
		{
		return 0;
		}

	if (!caseSensitive)
		{
		ucol_setStrength(coll, UCOL_PRIMARY);
		}

	UCollationResult r = ucol_strcollUTF8(coll, s1, length1, s2, length2, &err);
	ucol_close(coll);

	return (int) r;
}

/******************************************************************************
 CompareMaxNBytes (static)

	Replaces strncmp(): + if s1>s2, 0 if s1==s2, - if s1<s2

 ******************************************************************************/

int
JString::CompareMaxNBytes
	(
	const JUtf8Byte*	s1,
	const JUtf8Byte*	s2,
	const JSize			N,
	const JBoolean		caseSensitive
	)
{
	UErrorCode err  = U_ZERO_ERROR;
	UCollator* coll = ucol_open(NULL, &err);
	if (coll == NULL)
		{
		return 0;
		}

	if (!caseSensitive)
		{
		ucol_setStrength(coll, UCOL_PRIMARY);
		}

	JSize M = N;
	for (JIndex i=0; i<N; i++)
		{
		if (s1[i] == 0 || s2[i] == 0)
			{
			M = i+1;
			break;
			}
		}

	const UCollationResult r = ucol_strcollUTF8(coll, s1, M, s2, M, &err);
	ucol_close(coll);

	return (int) r;
}

/******************************************************************************
 CalcCharacterMatchLength (static)

	Calculates the number of characters that match from the beginning
	of the given strings.

	CalcMatchLength("abc", "abd")          -> 2
	CalcMatchLength("abc", "xyz")          -> 0
	CalcMatchLength("abc", "aBd", kJFalse) -> 2

 *****************************************************************************/

JSize
JString::CalcCharacterMatchLength
	(
	const JString&	s1,
	const JString&	s2,
	const JBoolean	caseSensitive
	)
{
	UErrorCode err  = U_ZERO_ERROR;
	UCollator* coll = ucol_open(NULL, &err);
	if (coll == NULL)
		{
		return 0;
		}

	if (!caseSensitive)
		{
		ucol_setStrength(coll, UCOL_PRIMARY);
		}

	const JUtf8Byte* b1 = s1.GetBytes();
	const JUtf8Byte* b2 = s2.GetBytes();

	JIndex i = 0, i1 = 0, i2 = 0;
	while (b1[i1] != 0 && b2[i2] != 0)
		{
		JSize n1, n2;
		if (!JUtf8Character::GetCharacterByteCount(b1+i1, &n1) ||
			!JUtf8Character::GetCharacterByteCount(b2+i2, &n2))
			{
			ucol_close(coll);
			return 0;
			}

		const UCollationResult r = ucol_strcollUTF8(coll, b1+i1, n1, b2+i2, n2, &err);
		if (r != 0)
			{
			break;
			}

		i++;
		i1 += n1;
		i2 += n2;
		}

	ucol_close(coll);
	return i;
}

/******************************************************************************
 CopyNormalizedBytes (static)

	This function processes at most maxBytes from source and places the
	normalized characters into destination.  (Normalization greatly
	simplifies all further operations on the string.)

	The return value is the number of bytes inserted into destination.
	Because of normalization, this may be less than the number of input
	bytes processed.

	destination is guaranteed to be NULL terminated.  The name capacity
	should remind you that there must also be room for a null terminator -
	at most capacity-1 bytes are actually inserted.

 *****************************************************************************/

JSize
JString::CopyNormalizedBytes
	(
	const JUtf8Byte*	source,
	const JSize			maxBytes,
	JUtf8Byte*			destination,
	const JSize			capacity
	)
{
	JSize currByteCount;
	JUInt32 curr = JUtf8Character::Utf8ToUtf32(source, &currByteCount);
	if (curr == 0 || maxBytes == 0)
		{
		destination[0] = 0;
		return 0;
		}

	UErrorCode err            = U_ZERO_ERROR;
	const UNormalizer2* norm2 = unorm2_getNFCInstance(&err);	// do not close

	JIndex i = 0, j = 0;
	while (i < maxBytes && j < capacity)
		{
		JSize nextByteCount = 0;
		JUInt32 next        = 0;
		UChar32 composed    = U_SENTINEL;
		if (i + currByteCount < maxBytes)	// don't consider bytes beyond the end of source
			{
			next     = JUtf8Character::Utf8ToUtf32(source + i + currByteCount, &nextByteCount);
			composed = unorm2_composePair(norm2, curr, next);
			}

		const JUtf8Character currUtf8 = JUtf8Character::Utf32ToUtf8(composed == U_SENTINEL ? curr : composed);
		const JSize currUtf8Count     = currUtf8.GetByteCount();
		if (j + currUtf8Count >= capacity)
			{
			break;
			}
		memcpy(destination + j, currUtf8.GetBytes(), currUtf8Count);
		j += currUtf8Count;

		if (composed == U_SENTINEL)
			{
			i            += currByteCount;
			curr          = next;
			currByteCount = nextByteCount;
			}
		else
			{
			i   += currByteCount + nextByteCount;
			curr = JUtf8Character::Utf8ToUtf32(source + i, &currByteCount);
			}
		}

	destination[j] = '\0';
	return j;
}

/******************************************************************************
 MatchCase (private)

	Adjusts the case of destRange to match the case of sourceRange in
	source.  Returns kJTrue if any changes were made.

	If sourceRange and destRange have the same number of characters, we
	match the case of each character individually.

	Otherwise:

	If both first characters are letters, the first letter of destRange is
	adjusted to the same case as the first character of sourceRange.

	If the rest of sourceRange contains at least one alphabetic character
	and all its alphabetic characters have the same case, all the alphabetic
	characters in destRange are coerced to that case.

 *****************************************************************************/

JBoolean
JString::MatchCase
	(
	const JUtf8Byte*		source,
	const JUtf8ByteRange&	sourceRange,
	const JUtf8ByteRange&	destRange
	)
{
	if (JString::IsEmpty(source) || sourceRange.IsEmpty() ||
		IsEmpty()                || destRange.IsEmpty())
		{
		return kJFalse;
		}

	assert( RangeValid(destRange) );

	JBoolean changed      = kJFalse;
	const JSize sourceLen = CountCharacters(source, sourceRange);
	const JSize destLen   = CountCharacters(itsBytes, destRange);

	// if not the same length, match all but first character

	JUtf8Character c;
	if (destLen > 1 && sourceLen != destLen)
		{
		JBoolean hasUpper = kJFalse;
		JBoolean hasLower = kJFalse;
		for (JIndex i = sourceRange.first; i <= sourceRange.last; )
			{
			c.Set(source + i-1);
			if (c.IsLower())
				{
				hasLower = kJTrue;
				}
			else if (c.IsUpper())
				{
				hasUpper = kJTrue;
				}

			if (hasLower && hasUpper)
				{
				break;
				}

			i += c.GetByteCount();
			}

		// first character is fixed separately below

		if (hasLower != hasUpper)
			{
			JUtf8Character c1;
			for (JIndex i = destRange.first; i <= destRange.last; )
				{
				c.Set(itsBytes + i-1);
				if (hasLower && !hasUpper)
					{
					c1 = c.ToLower();
					}
				else
					{
					c1 = c.ToUpper();
					}

				if (c1 != c)
					{
					ReplaceBytes(i, i + c.GetByteCount() - 1, c1.GetBytes(), c1.GetByteCount());
					}
				i += c1.GetByteCount();
				}
			changed = kJTrue;
			}
		}

	// if not same length, match first character
	// else, match all characters

	const JIndex endIndex = (sourceLen == destLen ? sourceLen : 1);

	JIndex j = sourceRange.first;
	JIndex k = destRange.first;
	JUtf8Character c1, c2, c3;
	for (JIndex i=1; i<=endIndex; i++)
		{
		c1.Set(source + j-1);
		c2.Set(itsBytes + k-1);
		if (c1.IsLower() && c2.IsUpper())
			{
			c3 = c2.ToLower();
			ReplaceBytes(k, k + c2.GetByteCount() - 1, c3.GetBytes(), c3.GetByteCount());
			changed = kJTrue;
			}
		else if (c1.IsUpper() && c2.IsLower())
			{
			c3 = c2.ToUpper();
			ReplaceBytes(k, k + c2.GetByteCount() - 1, c3.GetBytes(), c3.GetByteCount());
			changed = kJTrue;
			}

		j += c1.GetByteCount();
		k += c2.GetByteCount();
		}

	if (changed)
		{
		// TODO: notify iterators
		}

	return changed;
}

/******************************************************************************
 Stream operators

	The string data is delimited by double quotes:  "this is a string".

	To include double quotes in a string, use \"
	To include a backslash in a string, use \\

	An exception is made if the streams are cin or cout.
	For input, characters are simply read until 'return' is pressed.
	For output, Print() is used.

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	JString&	aString
	)
{
	if (&input == &cin)
		{
		// Read characters until return is pressed.
		// Following convention, the return must be left in the stream.

		aString = JReadLine(cin);
		cin.putback('\n');
		aString.TrimWhitespace();
		}
	else
		{
		// Read quote delimited string of characters.

		aString.ReadDelimited(input);
		}

	// allow chaining

	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const JString&	aString
	)
{
	if (&output == &cout)
		{
		aString.Print(cout);
		return output;
		}
	else if (&output == &cerr)
		{
		aString.Print(cerr);
		return output;
		}

	// write the starting delimiter

	output << '"';

	// write the string data, substituting \" in place of "

	for (JIndex i=1; i<=aString.itsByteCount; i++)
		{
		const JUtf8Byte c = aString.itsBytes[ i-1 ];

		if (c == '"')
			{
			output << "\\\"";
			}
		else if (c == '\\')
			{
			output << "\\\\";
			}
		else
			{
			output << c;
			}
		}

	// write the ending delimiter

	output << '"';

	// allow chaining

	return output;
}

/*-----------------------------------------------------------------------------
 Routines required for number<->string conversion
 -----------------------------------------------------------------------------*/

// private routines

static void  Round(int start, int B, int D[], int *exp, const int sigDigitCount);
static void  Shift(int* start, int D[], int* exp, const int sigDigitCount);

static void  JInsertSubstring(char s1[], short pos1, const char *s2, short pos2, short pos3);
static void  JShiftSubstring(char s[], short pos1, short pos2, short shift);
static short JLocateSubstring(const char *s1, const char *s2);

inline void
JAppendChar(char s1[], char c)
{
	const int len = strlen(s1);
	s1[len]   = c;
	s1[len+1] = '\0';
}

/*-----------------------------------------------------------------------------
 NumConvert.c (encapsulated by JString class)

	Assembled routines for dealing with string<->number conversion.

	Copyright (C) 1992 John Lindal. All rights reserved.

 -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 double2str (adapted from FNUM in FORTRAN)

	Converts doubleVal to a string stored at returnStr.

	afterDec = # of digits after decimal point, doubleVal is rounded appropriately
			   -1 => as many as needed, -2 => truncate, 0 => round up

	sigDigitCount = # of significant digits
					0 => use as many as possible

	exp = desired exponent, decimal point is shifted appropriately
		  kStandardExponent => use exponent if magnitude >=7
		  kForceExponent    => force use of exponent

 -----------------------------------------------------------------------------*/

void
double2str
	(
	double	doubleVal,
	int		afterDec,
	int		sigDigitCount,
	int		expMin,
	char	returnStr[]
	)
{
int i,j;

	if (sigDigitCount <= 0 || sigDigitCount > JString::kDefSigDigitCount)
		{
		sigDigitCount = JString::kDefSigDigitCount;
		}

	JBoolean neg=kJFalse;
	if (doubleVal<0.0)					// can't take log of x<0
		{
		neg       = kJTrue;
		doubleVal = -doubleVal;
		}

	// get exponent (bump it up slightly if exact multiple of 10)

	int exp = (doubleVal == 0.0) ? 0 : JLFloor(log10(doubleVal*1.00001))+1;

	// compute digit values

	int      D[ JString::kDefSigDigitCount+1 ];
	JBoolean A[ JString::kDefSigDigitCount+1 ];

	double chewedVal = doubleVal / pow(10.0, exp-1);
	for (i=0; i<=sigDigitCount; i++)
		{
		A[i]      = kJTrue;
		D[i]      = JLFloor(chewedVal) - 10*JLFloor(chewedVal/10.0);
		chewedVal = (chewedVal - D[i]) * 10.0 ;
		}

	// adjust digits that are outside [0,9]

	Round(sigDigitCount, sigDigitCount, D, &exp, sigDigitCount);
	if (D[sigDigitCount] > 4)
		{
		Round(sigDigitCount-1, sigDigitCount, D, &exp, sigDigitCount);
		}
	else
		{
		D[sigDigitCount] = 0;
		}

	// round up if lots of 9's on end (egcs 1.1.2:  0.6, 9.995)

	j=0;
	for (i=sigDigitCount-1; i>=0; i--)
		{
		if (D[i]==9)
			{
			j++;
			}
		else
			{
			break;
			}
		}
	if (j >= 3)
		{
		Round(sigDigitCount-j, sigDigitCount, D, &exp, sigDigitCount);
		}

	// set the exponent

	// There is no point in displaying an exponent until the zeroes start
	// piling up on the end.
	const int expMax = sigDigitCount+1;

	exp--;
	if (expMin == JString::kStandardExponent)
		{
		expMin = (-expMax<=exp && exp<=expMax) ? 0 : exp;
		}
	else if (expMin == JString::kForceExponent)
		{
		expMin = exp;
		}
	else if (doubleVal == 0.0)
		{
		expMin = 0;
		}

	// adjust digits to fit specs

	if (afterDec != -1)
		{
		JBoolean truncate = kJFalse;
		if (afterDec == -2)
			{
			afterDec = 0;
			truncate = kJTrue;
			}

		// adjust for spec'd # digits after dp

		int start = 1 + afterDec + (exp-expMin);
		if (start < 1)
			{
			Shift(&start,D,&exp, sigDigitCount);
			}
		else if (start > sigDigitCount)
			{
			start = sigDigitCount;
			}

		// round or truncate

		if (!truncate && D[start]>4)
			{
			Round(start-1,0,D,&exp, sigDigitCount);
			}
		else
			{
			for (i=start; i<=sigDigitCount; i++)
				{
				D[i]=0;
				}
			}
		}

	// strip trailing zeros

	for (i=sigDigitCount; i>=0; i--)
		{
		if (D[i]==0)
			{
			A[i]=kJFalse;
			}
		else
			{
			break;
			}
		}

	// create string of digits

	for (i=0; i<=sigDigitCount; i++)
		{
		if (A[i])
			{
			returnStr[i]='0'+D[i];
			}
		else
			{
			returnStr[i]='\0';
			break;
			}
		}

	if (returnStr[0] == '\0')			// number has ended up as zero
		{
		exp    = 0;
		expMin = 0;
		}
	else if (returnStr[0] == '0' &&
			 returnStr[1] == '\0')		// "-0" is silly
		{
		neg=kJFalse;
		}

	// prepend leading zero's and decimal point

	if (expMin>exp)
		{
		if (expMin-exp>1)
			{
			for (j=0; j<=expMin-exp-2; j++)
				{
				JInsertSubstring(returnStr,0,"0",0,1);
				}
			}
		if (returnStr[0] == '\0')						// 0. is silly
			{
			returnStr[0] = '0';
			returnStr[1] = '\0';
			}
		else
			{
			JInsertSubstring(returnStr,0,"0.",0,2);		// prepend decimal point
			}
		}

	// append zero's or insert decimal point

	else
		{
		i=strlen(returnStr)-1;
		if (exp-expMin>i)
			{
			for (j=1; j<=exp-expMin-i; j++)
				{
				JAppendChar(returnStr, '0');
				}
			}
		else if (exp-expMin<i)
			{
			JInsertSubstring(returnStr,exp-expMin+1,".",0,1);
			}
		}

	exp=expMin;

	// append decimal point and trailing zeros

	i = JLocateSubstring(returnStr,".") + 1;
	const int decPt = (i == 0 ? 0 : strlen(returnStr)-i);
	if (afterDec > decPt)
		{
		if (decPt==0)
			{
			JAppendChar(returnStr, '.');
			}
		for (i=decPt+1; i<=afterDec; i++)
			{
			JAppendChar(returnStr, '0');
			}
		}

	// append exponent

	if (exp != 0)
		{
		JAppendChar(returnStr, 'e');
		if (exp>0)
			{
			JAppendChar(returnStr, '+');
			}
		else
			{
			JAppendChar(returnStr, '-');
			exp=-exp;
			}

		double2str(exp,0,0,0, returnStr + strlen(returnStr));
		}

	// prepend negative sign

	if (neg)
		{
		JInsertSubstring(returnStr,0,"-",0,1);
		}
}

/*-----------------------------------------------------------------------------
 Round (from ROUND in FORTRAN)

	Rounds D starting from start.

 -----------------------------------------------------------------------------*/

void
Round
	(
	int			start,
	int			B,
	int			D[],
	int*		exp,
	const int	sigDigitCount
	)
{
int i;

	// clear digits beyond where we will round

	for (i=start+1; i<=sigDigitCount; i++)
		{
		D[i]=0;
		}

	// round up digit # start

	if (start < sigDigitCount)
		{
		D[start]++;
		}

	// move 10 up to next digit

	if (start > 0)
		{
		for (i=start; i>=1; i--)
			{
			if (D[i]>9)
				{
				D[i]=0;
				D[i-1]++;
				}
			}
		}

	// make space for extra digit in front

	if (D[0] > 9)
		{
		for (i=sigDigitCount; i>=2; i--)
			{
			D[i]=D[i-1];
			}
		D[0]=1;
		D[1]=0;
		(*exp)++;
		}
}

/*-----------------------------------------------------------------------------
 Shift (from SHIFT in FORTRAN)

	Shifts digits in D array so that start can be increased to 1.
	This takes care of the situation when doubleVal=0.002 and afterDec=0.

 -----------------------------------------------------------------------------*/

void
Shift
	(
	int*		start,
	int			D[],
	int*		exp,
	const int	sigDigitCount
	)
{
int i;

	// calculate how far to shift each digit

	const int shift = 1 - *start;

	// shift digits

	if (shift < sigDigitCount+1)
		{
		for (i=sigDigitCount; i>=shift; i--)
			{
			D[i]=D[i-shift];
			}

		// set preceding digits to zero

		for (i=0; i<shift; i++)
			{
			D[i]=0;
			}
		}

	// shift too large, just clear all digits

	else
		{
		for (i=0; i<=sigDigitCount; i++)
			{
			D[i]=0;
			}
		}

	(*start)=1;			// shifting digits fixed start
	(*exp)+=shift;		// exponent has changed
}

/*-----------------------------------------------------------------------------
 JString.StrUtil.c (a stripped version required by NumConvert.c)

	Assembled routines for dealing with string conversion.
	These routines only require vanilla C.

	Copyright (C) 1992 John Lindal. All rights reserved.

 -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 JInsertSubstring (augments strcat in string.h)

	Inserts s2 into s1 at the specified postion in s1.
	pos# start at zero.  pos1 = -1 => append.  pos3 = -1 => to end of s2.
	Assumes there is enough space in s1 to hold s1+s2.

 -----------------------------------------------------------------------------*/

void JInsertSubstring(char s1[],short pos1,const char *s2,short pos2,short pos3)
{
short i,len1,len2;

	len1=strlen(s1); len2=strlen(s2);
	if (pos1==-1 || pos1>len1) pos1=len1;
	if (pos2<0) pos2=0;
	if (pos3==-1 || pos3>len2) pos3=len2;
	JShiftSubstring(s1,pos1,-1,pos3-pos2);
	for (i=pos2;i<pos3;i++) s1[pos1+i-pos2]=*(s2+i);
}

/*-----------------------------------------------------------------------------
 JShiftSubstring

	Shifts characters in s from pos1 to pos2 by shift positions.
	Overwrites old characters.  Gives up if you try to shift past left end of string.
	pos# start at zero.  pos2 = -1 => end.  shift > 0 => shift to right.
	Assumes there is enough space in s to hold shifted results.

 -----------------------------------------------------------------------------*/

void JShiftSubstring(char s[],short pos1,short pos2,short shift)
{
short i,len;

	if (shift==0) return;

	len=strlen(s);
	if (pos1<0) pos1=0;
	if (pos2==-1 || pos2>len) pos2=len;

	if (pos2<pos1 || pos1+shift<0) return;

	/* pos2=len => termination shifts too */

	if (shift<0)
		for (i=pos1;i<=pos2;i++) s[i+shift]=s[i];
	if (shift>0) {
		for (i=pos2;i>=pos1;i--) s[i+shift]=s[i];
		if (pos2+shift+1>len && pos2<len) s[pos2+shift+1]=0;	/* Terminate string */
		}
}

/*-----------------------------------------------------------------------------
 JLocateSubstring (equivalent to INDEX()-1 in FORTRAN)

	Returns the offset to the 1st occurrence of s2 inside s1.
	-1 => Not found.

 -----------------------------------------------------------------------------*/

short JLocateSubstring(const char *s1,const char *s2)
{
short i,len1,len2;

	len1=strlen(s1); len2=strlen(s2);

	for (i=0;i<=len1-len2;i++)
		if (strncmp(s1+i,s2,len2)==0) return i;

	return -1;
}
