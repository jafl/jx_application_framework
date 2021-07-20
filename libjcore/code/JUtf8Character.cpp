/******************************************************************************
 JUtf8Character.cpp

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "JUtf8Character.h"
#include <unicode/uchar.h>
#include <iomanip>
#include "jAssert.h"

const JUInt32 JUtf8Character::kUtf32SubstitutionCharacter       = 0x0000FFFD;
const JUtf8Character JUtf8Character::kUtf8SubstitutionCharacter("\xEF\xBF\xBD");

bool JUtf8Character::theIgnoreBadUtf8Flag = false;

/******************************************************************************
 Constructor

 ******************************************************************************/

JUtf8Character::JUtf8Character()
	:
	itsByteCount(0)
{
	itsBytes[0] = 0;
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
		memcpy(itsBytes, source.itsBytes, itsByteCount+1);
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
		if (!theIgnoreBadUtf8Flag)
			{
			std::cerr << "Replaced invalid UTF-8 byte sequence with substitution U+FFFD" << std::endl;
			}
		utf8Character = kUtf8SubstitutionCharacter.GetBytes();
		byteCount     = kUtf8SubstitutionCharacter.GetByteCount();
		}

	memcpy(itsBytes, utf8Character, byteCount);
	itsBytes[ byteCount ] = '\0';
	itsByteCount = byteCount;
}

/******************************************************************************
 IsCompleteCharacter (static)

 ******************************************************************************/

bool
JUtf8Character::IsCompleteCharacter
	(
	const JUtf8Byte*	utf8Character,
	const JSize			byteCount,
	JSize*				characterByteCount
	)
{
	auto* c = (unsigned char*) utf8Character;
	if (c[0] == 0)
		{
		*characterByteCount = 0;
		return false;
		}
	else if (c[0] <= (unsigned char) '\x7F')
		{
		*characterByteCount = 1;
		return byteCount >= 1;
		}
	else if (((unsigned char) '\xC2') <= c[0] && c[0] <= (unsigned char) '\xDF')
		{
		*characterByteCount = 2;
		return byteCount >= 2;
		}
	else if (((unsigned char) '\xE0') <= c[0] && c[0] <= (unsigned char) '\xEF')
		{
		*characterByteCount = 3;
		return byteCount >= 3;
		}
	else if (((unsigned char) '\xF0') <= c[0] && c[0] <= (unsigned char) '\xF4')
		{
		*characterByteCount = 4;
		return byteCount >= 4;
		}
	else
		{
		*characterByteCount = 1;
		return byteCount >= 1;
		}
}

/******************************************************************************
 GetCharacterByteCount (static)

 ******************************************************************************/

bool
JUtf8Character::GetCharacterByteCount
	(
	const JUtf8Byte*	utf8Character,
	JSize*				byteCount
	)
{
	auto* c = (unsigned char*) utf8Character;
	bool ok      = false;
	if (c[0] <= (unsigned char) '\x7F')
		{
		*byteCount = 1;
		ok = true;
		}
	else if (((unsigned char) '\xC2') <= c[0] && c[0] <= (unsigned char) '\xDF')
		{
		*byteCount = 2;
		ok = ((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char) '\xBF';
		}
	else if (((unsigned char) '\xE0') <= c[0] && c[0] <= (unsigned char) '\xEF')
		{
		*byteCount = 3;
		ok = ((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char) '\xBF' &&
			 ((unsigned char) '\x80') <= c[2] && c[2] <= (unsigned char) '\xBF';
		}
	else if (((unsigned char) '\xF0') <= c[0] && c[0] <= (unsigned char) '\xF4')
		{
		*byteCount = 4;
		ok = ((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char) '\xBF' &&
			 ((unsigned char) '\x80') <= c[2] && c[2] <= (unsigned char) '\xBF' &&
			 ((unsigned char) '\x80') <= c[3] && c[3] <= (unsigned char) '\xBF';
		}

	if (!ok)
		{
		*byteCount = 1;
		}

	if (!ok && !theIgnoreBadUtf8Flag)
		{
		std::cerr << "Invalid UTF-8 byte sequence: " << std::hex;
		for (JUnsignedOffset i=0; i<*byteCount; i++)
			{
			std::cerr << (int) c[i] << ' ';
			}
		std::cerr << std::dec << std::endl;
		}
	return ok;
}

/******************************************************************************
 GetPrevCharacterByteCount (static)

	Returns the length of the character ending with the specified byte.

 ******************************************************************************/

bool
JUtf8Character::GetPrevCharacterByteCount
	(
	const JUtf8Byte*	lastByte,
	JSize*				byteCount
	)
{
	const auto* s = (const unsigned char*) lastByte;
	while (((unsigned char) '\x80') <= *s && *s <= (unsigned char) '\xBF')
		{
		s--;
		}

	return GetCharacterByteCount((const JUtf8Byte*) s, byteCount);
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
	if (ch < (JUInt32) 0x80)
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
	JSize byteCount;
	const bool ok = GetCharacterByteCount(bytes, &byteCount);
	if (returnByteCount != nullptr)
		{
		*returnByteCount = byteCount;
		}

	if (!ok)
		{
		return kUtf32SubstitutionCharacter;
		}

	JUInt32 ch = 0;
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

bool
JUtf8Character::IsPrint()
	const
{
	return u_isprint(GetUtf32());
}

bool
JUtf8Character::IsAlnum()
	const
{
	const JUInt32 c = GetUtf32();
	return u_isUAlphabetic(c) || u_isdigit(c);
}

bool
JUtf8Character::IsDigit()
	const
{
	return u_isdigit(GetUtf32());
}

bool
JUtf8Character::IsAlpha()
	const
{
	return u_isUAlphabetic(GetUtf32());
}

bool
JUtf8Character::IsLower()
	const
{
	return u_isULowercase(GetUtf32());
}

bool
JUtf8Character::IsUpper()
	const
{
	return u_isUUppercase(GetUtf32());
}

bool
JUtf8Character::IsSpace()
	const
{
	return u_isUWhiteSpace(GetUtf32());
}

bool
JUtf8Character::IsControl()
	const
{
	return u_charType(GetUtf32()) == U_CONTROL_CHAR;
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
 AllocateBytes

	This allocates a new pointer, which the caller is responsible
	for deleting via "delete []".

 ******************************************************************************/

JUtf8Byte*
JUtf8Character::AllocateBytes()
	const
{
	auto* s = jnew JUtf8Byte[ itsByteCount+1 ];
	assert( s != nullptr );
	memcpy(s, itsBytes, itsByteCount);
	s[ itsByteCount ] = '\0';
	return s;
}

/******************************************************************************
 PrintHex

	Display the hex bytes.

 ******************************************************************************/

void
JUtf8Character::PrintHex
	(
	std::ostream& output
	)
	const
{
	output << std::hex;
	for (JUnsignedOffset i=0; i<itsByteCount; i++)
		{
		output << (int) (unsigned char) itsBytes[i] << ' ';
		}
	output << std::dec;
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&	input,
	JUtf8Character&	c
	)
{
	unsigned char b;
	input.read((char*) &b, 1);

	JSize byteCount;
	bool ok = true;
	if (b <= (unsigned char) '\x7F')
		{
		byteCount = 1;
		}
	else if (((unsigned char) '\xC2') <= b && b <= (unsigned char) '\xDF')
		{
		byteCount = 2;
		}
	else if (((unsigned char) '\xE0') <= b && b <= (unsigned char) '\xEF')
		{
		byteCount = 3;
		}
	else if (((unsigned char) '\xF0') <= b && b <= (unsigned char) '\xF4')
		{
		byteCount = 4;
		}
	else
		{
		ok = false;
		c  = JUtf8Character::kUtf8SubstitutionCharacter;
		}

	if (ok)
		{
		c.itsByteCount          = byteCount;
		c.itsBytes[0]           = b;
		c.itsBytes[ byteCount ] = 0;
		if (byteCount > 1)
			{
			input.read(c.itsBytes + 1, byteCount - 1);

			if (!JUtf8Character::IsValid(c.itsBytes))
				{
				c = JUtf8Character::kUtf8SubstitutionCharacter;
				}
			}
		}

	// allow chaining

	return input;
}

std::ostream&
operator<<
	(
	std::ostream&			output,
	const JUtf8Character&	c
	)
{
	output.write(c.itsBytes, c.itsByteCount);

	// allow chaining

	return output;
}
