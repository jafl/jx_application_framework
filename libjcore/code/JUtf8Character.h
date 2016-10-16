/******************************************************************************
 JUtf8Character.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JUtf8Character
#define _H_JUtf8Character

#include <jTypes.h>

const JSize kJMaxUtf8CharacterLength = 4;

class JUtf8Character
{
public:

	JUtf8Character();
	JUtf8Character(const JUtf8Byte asciiCharacter);
	JUtf8Character(const JUtf8Byte* utf8Character);
	JUtf8Character(const JUtf8Character& source);

	const JUtf8Character& operator=(const JUtf8Character& source);
	const JUtf8Character& operator=(const JUtf8Byte asciiCharacter);
	const JUtf8Character& operator=(const JUtf8Byte* utf8Character);

	JSize				GetByteCount() const;
	const JUtf8Byte*	GetBytes() const;

	void	Set(const JUtf8Character& source);
	void	Set(const JUtf8Byte asciiCharacter);
	void	Set(const JUtf8Byte* utf8Character);

	JBoolean	IsPrint() const;
	JBoolean	IsAlnum() const;
	JBoolean	IsAlpha() const;
	JBoolean	IsUpper() const;
	JBoolean	IsLower() const;

	JUtf8Character	ToUpper() const;
	JUtf8Character	ToLower() const;

	static JBoolean	IsValid(const JUtf8Byte* utf8Character);
	static JSize	GetCharacterByteCount(const JUtf8Byte* utf8Character);

private:

	unsigned char	itsByteCount;
	JUtf8Byte		itsBytes[ kJMaxUtf8CharacterLength ];
};


/******************************************************************************
 GetByteCount

 ******************************************************************************/

inline JSize
JUtf8Character::GetByteCount()
	const
{
	return itsByteCount;
}

/******************************************************************************
 GetBytes

 ******************************************************************************/

inline const JUtf8Byte*
JUtf8Character::GetBytes()
	const
{
	return itsBytes;
}

/******************************************************************************
 Assignment operator

	We do not copy itsBlockSize because we assume the client has set them
	appropriately.

 ******************************************************************************/

inline const JUtf8Character&
JUtf8Character::operator=
	(
	const JUtf8Character& source
	)
{
	Set(source);
	return *this;
}

inline const JUtf8Character&
JUtf8Character::operator=
	(
	const JUtf8Byte asciiCharacter
	)
{
	Set(asciiCharacter);
	return *this;
}

inline const JUtf8Character&
JUtf8Character::operator=
	(
	const JUtf8Byte* utf8Character
	)
{
	Set(utf8Character);
	return *this;
}

/******************************************************************************
 Comparison operators

 ******************************************************************************/

inline int
operator==
	(
	const JUtf8Character& c1,
	const JUtf8Character& c2
	)
{
	return (c1.GetByteCount() == c2.GetByteCount() &&
			memcmp(c1.GetBytes(), c2.GetBytes(), c1.GetByteCount()) == 0);
}

inline int
operator!=
	(
	const JUtf8Character& c1,
	const JUtf8Character& c2
	)
{
	return !(c1 == c2);
}

#endif
