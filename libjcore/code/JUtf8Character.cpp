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
	memset(itsBytes, 0, kJMaxUtf8CharacterLength);
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
		memset(itsBytes, 0, kJMaxUtf8CharacterLength);

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
	memset(itsBytes, 0, kJMaxUtf8CharacterLength);
	itsBytes[0]  = asciiCharacter;
	itsByteCount = 1;
}

void
JUtf8Character::Set
	(
	const JUtf8Byte* utf8Character
	)
{
	memset(itsBytes, 0, kJMaxUtf8CharacterLength);

	if (utf8Character[0] <= '\x7F')
		{
		itsByteCount = 1;
		}
	else if ('\xC2' <= utf8Character[0] && utf8Character[0] <= '\xDF')
		{
		itsByteCount = 2;
		}
	else if ('\xE0' <= utf8Character[0] && utf8Character[0] <= '\xEF')
		{
		itsByteCount = 3;
		}
	else if ('\xF0' <= utf8Character[0] && utf8Character[0] <= '\xF4')
		{
		itsByteCount = 4;
		}
	else
		{
		assert( 0 /* invalid UTF-8 byte sequence */ );
		}

	memcpy(itsBytes, utf8Character, itsByteCount);
}
