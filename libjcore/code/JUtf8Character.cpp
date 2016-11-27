/******************************************************************************
 JUtf8Character.cpp

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JUtf8Character.h>
#include <unicode/uchar.h>
#include <iomanip>
#include <jAssert.h>

const JUInt32 JUtf8Character::kUtf32SubstitutionCharacter = 0x0000FFFD;
const JUtf8Byte* kUtf8SubstitutionCharacter               = "\xEF\xBF\xBD";
const JSize kUtf8SubstitutionCharacterByteLength          = 3;

/******************************************************************************
 Constructor

 ******************************************************************************/

JUtf8Character::JUtf8Character()
	:
	itsByteCount(0)
{
}

JUtf8Character::JUtf8Character
	(
	const JUtf8Byte asciiCharacter
	)
{
	Set(asciiCharacter);
}

JUtf8Character::JUtf8Character
	(
	const JUtf8Byte* utf8Character
	)
{
	Set(utf8Character);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JUtf8Character::JUtf8Character
	(
	const JUtf8Character& source
	)
{
	Set(source);
}

/******************************************************************************
 Set

 ******************************************************************************/

void
JUtf8Character::Set
	(
	const JUtf8Character& source
	)
{
	if (this != &source)
		{
		itsByteCount = source.itsByteCount;
		memcpy(itsBytes, source.itsBytes, itsByteCount);
		}
}

void
JUtf8Character::Set
	(
	const JUtf8Byte asciiCharacter
	)
{
	JUtf8Byte s[] = { asciiCharacter, 0 };
	Set(s);
}

void
JUtf8Character::Set
	(
	const JUtf8Byte* utf8Character
	)
{
	JSize byteCount;
	if (!GetCharacterByteCount(utf8Character, &byteCount))
		{
		cerr << "Replaced invalid UTF-8 byte sequence with substitution U+FFFD" << endl;
		utf8Character = kUtf8SubstitutionCharacter;
		byteCount     = kUtf8SubstitutionCharacterByteLength;
		}

	memcpy(itsBytes, utf8Character, byteCount);
	itsByteCount = byteCount;
}

/******************************************************************************
 GetCharacterByteCount (static)

 ******************************************************************************/

JBoolean
JUtf8Character::GetCharacterByteCount
	(
	const JUtf8Byte*	utf8Character,
	JSize*				byteCount
	)
{
	unsigned char* c = (unsigned char*) utf8Character;
	JBoolean ok;
	if (c[0] == 0)
		{
		*byteCount = 0;
		ok = kJTrue;
		}
	else if (c[0] <= (unsigned char) '\x7F')
		{
		*byteCount = 1;
		ok = kJTrue;
		}
	else if (((unsigned char) '\xC2') <= c[0] && c[0] <= (unsigned char) '\xDF')
		{
		*byteCount = 2;
		ok = JI2B(((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char) '\xBF');
		}
	else if (((unsigned char) '\xE0') <= c[0] && c[0] <= (unsigned char) '\xEF')
		{
		*byteCount = 3;
		ok = JI2B(((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char) '\xBF' &&
				  ((unsigned char) '\x80') <= c[2] && c[2] <= (unsigned char) '\xBF');
		}
	else if (((unsigned char) '\xF0') <= c[0] && c[0] <= (unsigned char) '\xF4')
		{
		*byteCount = 4;
		ok = JI2B(((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char) '\xBF' &&
				  ((unsigned char) '\x80') <= c[2] && c[2] <= (unsigned char) '\xBF' &&
				  ((unsigned char) '\x80') <= c[3] && c[3] <= (unsigned char) '\xBF');
		}
	else
		{
		*byteCount = 1;
		ok         = kJFalse;
		}

	if (!ok)
		{
		cerr << "Invalid UTF-8 byte sequence: " << std::hex;
		for (JIndex i=0; i<*byteCount; i++)
			{
			cerr << (int) c[i] << ' ';
			}
		cerr << std::dec << endl;
		}
	return ok;
}

/******************************************************************************
 Utf32ToUtf8 (static)

	Adapted from LLVM source code.

 ******************************************************************************/

JUtf8Character
JUtf8Character::Utf32ToUtf8
	(
	const JUInt32 origChar
	)
{
	JUInt32 ch = origChar;

	JSize byteCount = 0;
	if (ch == 0)
		{
		byteCount = 0;
		}
	else if (ch < (JUInt32) 0x80)
		{
		byteCount = 1;
		}
	else if (ch < (JUInt32) 0x800)
		{
		byteCount = 2;
		}
	else if (ch < (JUInt32) 0x10000)
		{
		byteCount = 3;
		}
	else if (ch <= (JUInt32) 0x0010FFFF)
		{
		byteCount = 4;
		}
	else
		{
		byteCount = 3;
		ch = kUtf32SubstitutionCharacter;
		}

	JUtf8Character c;
	c.itsByteCount = byteCount;

	const unsigned char firstByteMark[] = { 0x00, 0xC0, 0xE0, 0xF0 };
	const JUInt32 byteMask              = 0xBF;
	const JUInt32 byteMark              = 0x80; 

	switch (byteCount)	// note: everything falls through
		{
		case 4: 
			c.itsBytes[3] = (unsigned char) ((ch | byteMark) & byteMask);
			ch >>= 6;

		case 3:
			c.itsBytes[2] = (unsigned char) ((ch | byteMark) & byteMask);
			ch >>= 6;

		case 2: 
			c.itsBytes[1] = (unsigned char) ((ch | byteMark) & byteMask);
			ch >>= 6;

		case 1: 
			c.itsBytes[0] = (unsigned char) (ch | firstByteMark[byteCount-1]);
	}

	return c;
}

/******************************************************************************
 Utf8ToUtf32 (static)

	Adapted from LLVM source code.

 ******************************************************************************/

JUInt32
JUtf8Character::Utf8ToUtf32
	(
	const JUtf8Byte*	bytes,
	JSize*				returnByteCount
	)
{
	JUInt32 ch = 0;

	JSize byteCount;
	const JBoolean ok = GetCharacterByteCount(bytes, &byteCount);
	if (returnByteCount != NULL)
		{
		*returnByteCount = byteCount;
		}

	if (!ok)
		{
		return kUtf32SubstitutionCharacter;
		}
	else if (byteCount == 0)
		{
		return 0;
		}

	switch (byteCount)	// note: everything falls through
		{
		case 4:
			ch += (unsigned char) *bytes++;
			ch <<= 6;

		case 3:
			ch += (unsigned char) *bytes++;
			ch <<= 6;

		case 2:
			ch += (unsigned char) *bytes++;
			ch <<= 6;

		case 1:
			ch += (unsigned char) *bytes++;
		}

	const JUInt32 offset[] =
		{
		0x00000000UL,
		0x00003080UL,
		0x000E2080UL, 
		0x03C82080UL
	};
	ch -= offset[ byteCount-1 ];

	if (0xD800 <= ch && ch <= 0xDFFF)		// illegal character range
		{
		ch = kUtf32SubstitutionCharacter;
		}

	return ch;
}

/******************************************************************************
 Character types

 ******************************************************************************/

JBoolean
JUtf8Character::IsPrint()
	const
{
	return JI2B( u_isprint(GetUtf32()) );
}

JBoolean
JUtf8Character::IsAlnum()
	const
{
	const JUInt32 c = GetUtf32();
	return JI2B( u_isUAlphabetic(c) || u_isdigit(c) );
}

JBoolean
JUtf8Character::IsAlpha()
	const
{
	return JI2B( u_isUAlphabetic(GetUtf32()) );
}

JBoolean
JUtf8Character::IsLower()
	const
{
	return JI2B( u_isULowercase(GetUtf32()) );
}

JBoolean
JUtf8Character::IsUpper()
	const
{
	return JI2B( u_isUUppercase(GetUtf32()) );
}

JBoolean
JUtf8Character::IsSpace()
	const
{
	return JI2B( u_isUWhiteSpace(GetUtf32()) );
}

/******************************************************************************
 Case conversion

	Only do this if you truly have a single character.  Otherwise, use
	JString versions, because case conversion can modify more than one
	character in some scripts.

 ******************************************************************************/

JUtf8Character
JUtf8Character::ToLower()
	const
{
	return Utf32ToUtf8(u_tolower(GetUtf32()));
}

JUtf8Character
JUtf8Character::ToUpper()
	const
{
	return Utf32ToUtf8(u_toupper(GetUtf32()));
}

/******************************************************************************
 AllocateNullTerminatedBytes

	This allocates a new pointer, which the caller is responsible
	for deleting via "delete []".

 ******************************************************************************/

JUtf8Byte*
JUtf8Character::AllocateNullTerminatedBytes()
	const
{
	JUtf8Byte* s = jnew JUtf8Byte[ itsByteCount+1 ];
	assert( s != NULL );
	memcpy(s, itsBytes, itsByteCount);
	s[ itsByteCount+1 ] = '\0';
	return s;
}

/******************************************************************************
 PrintHex

	Display the hex bytes.

 ******************************************************************************/

void
JUtf8Character::PrintHex
	(
	ostream& output
	)
	const
{
	output << std::hex;
	for (JIndex i=0; i<itsByteCount; i++)
		{
		output << (int) (unsigned char) itsBytes[i] << ' ';
		}
	output << std::dec;
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

ostream&
operator<<
	(
	ostream&				output,
	const JUtf8Character&	c
	)
{
	for (JIndex i=1; i<=c.itsByteCount; i++)
		{
		output << std::hex << c.itsBytes[i-1];
		}

	// allow chaining

	return output;
}
