/******************************************************************************
 JUtf8Character.cpp

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JUtf8Character.h>
#include <jAssert.h>

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
	itsBytes[0]  = asciiCharacter;
	itsByteCount = 1;
}

void
JUtf8Character::Set
	(
	const JUtf8Byte* utf8Character
	)
{
	unsigned char* c = (unsigned char*) utf8Character;
	if (c[0] <= '\x7F')
		{
		itsByteCount = 1;
		itsBytes[0]  = utf8Character[0];
		}
	else if ('\xC2' <= c[0] && c[0] <= '\xDF' &&
			 '\x80' <= c[1] && c[1] <= '\xBF')
		{
		itsByteCount = 2;
		itsBytes[0]  = utf8Character[0];
		itsBytes[1]  = utf8Character[1];
		}
	else if ('\xE0' <= c[0] && c[0] <= '\xEF' &&
			 '\x80' <= c[1] && c[1] <= '\xBF' &&
			 '\x80' <= c[2] && c[2] <= '\xBF')
		{
		itsByteCount = 3;
		itsBytes[0]  = utf8Character[0];
		itsBytes[1]  = utf8Character[1];
		itsBytes[2]  = utf8Character[2];
		}
	else if ('\xF0' <= c[0] && c[0] <= '\xF4' &&
			 '\x80' <= c[1] && c[1] <= '\xBF' &&
			 '\x80' <= c[2] && c[2] <= '\xBF' &&
			 '\x80' <= c[3] && c[3] <= '\xBF')
		{
		itsByteCount = 4;
		itsBytes[0]  = utf8Character[0];
		itsBytes[1]  = utf8Character[1];
		itsBytes[2]  = utf8Character[2];
		itsBytes[3]  = utf8Character[3];
		}
	else
		{
		assert( 0 /* invalid UTF-8 byte sequence */ );
		}
}

/******************************************************************************
 IsValid (static)

 ******************************************************************************/

JBoolean
JUtf8Character::IsValid
	(
	const JUtf8Byte* utf8Character
	)
{
	unsigned char* c = (unsigned char*) utf8Character;
	if (c[0] <= '\x7F')
		{
		return kJTrue;
		}
	else if ('\xC2' <= c[0] && c[0] <= '\xDF' &&
			 '\x80' <= c[1] && c[1] <= '\xBF')
		{
		return kJTrue;
		}
	else if ('\xE0' <= c[0] && c[0] <= '\xEF' &&
			 '\x80' <= c[1] && c[1] <= '\xBF' &&
			 '\x80' <= c[2] && c[2] <= '\xBF')
		{
		return kJTrue;
		}
	else if ('\xF0' <= c[0] && c[0] <= '\xF4' &&
			 '\x80' <= c[1] && c[1] <= '\xBF' &&
			 '\x80' <= c[2] && c[2] <= '\xBF' &&
			 '\x80' <= c[3] && c[3] <= '\xBF')
		{
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetCharacterByteCount (static)

 ******************************************************************************/

JSize
JUtf8Character::GetCharacterByteCount
	(
	const JUtf8Byte* utf8Character
	)
{
	unsigned char* c = (unsigned char*) utf8Character;
	if (c[0] <= (unsigned char) '\x7F')
		{
		return 1;
		}
	else if (((unsigned char) '\xC2') <= c[0] && c[0] <= (unsigned char) '\xDF' &&
			 ((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char)'\xBF')
		{
		return 2;
		}
	else if (((unsigned char) '\xE0') <= c[0] && c[0] <= (unsigned char)'\xEF' &&
			 ((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char)'\xBF' &&
			 ((unsigned char) '\x80') <= c[2] && c[2] <= (unsigned char)'\xBF')
		{
		return 3;
		}
	else if (((unsigned char) '\xF0') <= c[0] && c[0] <= (unsigned char)'\xF4' &&
			 ((unsigned char) '\x80') <= c[1] && c[1] <= (unsigned char)'\xBF' &&
			 ((unsigned char) '\x80') <= c[2] && c[2] <= (unsigned char)'\xBF' &&
			 ((unsigned char) '\x80') <= c[3] && c[3] <= (unsigned char)'\xBF')
		{
		return 4;
		}
	else
		{
		assert( 0 /* invalid UTF-8 byte sequence */ );
		cerr << "Invalid UTF-8 sequence:"
			<< std::hex
			<< " " << (int) c[0]
			<< " " << (int) c[1]
			<< " " << (int) c[2]
			<< " " << (int) c[3]
			<< endl;
		return 1;
		}
}

/******************************************************************************
 Character types

	TODO: utf8

 ******************************************************************************/

JBoolean
JUtf8Character::IsPrint()
	const
{
	assert( itsByteCount == 1 );
	return JI2B( isprint(itsBytes[0]) );
}

JBoolean
JUtf8Character::IsAlnum()
	const
{
	assert( itsByteCount == 1 );
	return JI2B( IsAlpha() || isdigit(itsBytes[0]) );
}

JBoolean
JUtf8Character::IsAlpha()
	const
{
	assert( itsByteCount == 1 );
	return JI2B( isalpha(itsBytes[0]) );
}

JBoolean
JUtf8Character::IsUpper()
	const
{
	assert( itsByteCount == 1 );
	return JI2B( isupper(itsBytes[0]) );
}

JBoolean
JUtf8Character::IsLower()
	const
{
	assert( itsByteCount == 1 );
	return JI2B( islower(itsBytes[0]) );
}

/******************************************************************************
 Case conversion

	TODO: utf8

 ******************************************************************************/

JUtf8Character
JUtf8Character::ToUpper()
	const
{
	assert( itsByteCount == 1 );
	return JUtf8Character(toupper(itsBytes[0]));
}

JUtf8Character
JUtf8Character::ToLower()
	const
{
	assert( itsByteCount == 1 );
	return JUtf8Character(tolower(itsBytes[0]));
}
