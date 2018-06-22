/******************************************************************************
 JUtf8Character.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUtf8Character
#define _H_JUtf8Character

#include "jTypes.h"
#include <string.h>

class JUtf8Character
{
	friend std::istream& operator>>(std::istream&, JUtf8Character&);
	friend std::ostream& operator<<(std::ostream&, const JUtf8Character&);

public:

	enum
	{
		kMaxByteCount = 4
	};

	static const JUInt32 kUtf32SubstitutionCharacter;
	static const JUtf8Character kUtf8SubstitutionCharacter;

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
	JUtf8Byte*			AllocateBytes() const;	// client must call delete [] when finished with it
	JUInt32				GetUtf32() const;

	void	Set(const JUtf8Character& source);
	void	Set(const JUtf8Byte asciiCharacter);
	void	Set(const JUtf8Byte* utf8Character);

	JBoolean	IsPrint() const;
	JBoolean	IsAlnum() const;
	JBoolean	IsDigit() const;
	JBoolean	IsAlpha() const;
	JBoolean	IsLower() const;
	JBoolean	IsUpper() const;
	JBoolean	IsSpace() const;
	JBoolean	IsControl() const;

	JUtf8Character	ToLower() const;
	JUtf8Character	ToUpper() const;

	void	PrintHex(std::ostream& output) const;

	static JBoolean	IsValid(const JUtf8Byte* utf8Character);
	static JBoolean	IsCompleteCharacter(const JUtf8Byte* utf8Character, const JSize byteCount, JSize* characterByteCount);
	static JBoolean	GetCharacterByteCount(const JUtf8Byte* utf8Character, JSize* byteCount);
	static JBoolean	GetPrevCharacterByteCount(const JUtf8Byte* lastByte, JSize* byteCount);

	static JUtf8Character	Utf32ToUtf8(const JUInt32 c);
	static JUInt32			Utf8ToUtf32(const JUtf8Byte* c, JSize* returnByteCount = nullptr);

private:

	unsigned char	itsByteCount;
	JUtf8Byte		itsBytes[ kMaxByteCount+1 ];
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
 GetUtf32

 ******************************************************************************/

inline JUInt32
JUtf8Character::GetUtf32()
	const
{
	return (itsByteCount == 0 ? 0 : Utf8ToUtf32(itsBytes));
}

/******************************************************************************
 IsValid (static)

 ******************************************************************************/

inline JBoolean
JUtf8Character::IsValid
	(
	const JUtf8Byte* utf8Character
	)
{
	JSize byteCount;
	return GetCharacterByteCount(utf8Character, &byteCount);
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
operator==
	(
	const JUtf8Byte			c1,
	const JUtf8Character&	c2
	)
{
	return (c2.GetByteCount() == 1 && c1 == c2.GetBytes()[0]);
}

inline int
operator==
	(
	const JUtf8Character&	c1,
	const JUtf8Byte			c2
	)
{
	return (c1.GetByteCount() == 1 && c1.GetBytes()[0] == c2);
}

inline int
operator==
	(
	const JUtf8Byte*		c1,
	const JUtf8Character&	c2
	)
{
	return (strlen(c1) == c2.GetByteCount() &&
			memcmp(c1, c2.GetBytes(), c2.GetByteCount()) == 0);
}

inline int
operator==
	(
	const JUtf8Character&	c1,
	const JUtf8Byte*		c2
	)
{
	return (c1.GetByteCount() == strlen(c2) &&
			memcmp(c1.GetBytes(), c2, c1.GetByteCount()) == 0);
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

inline int
operator!=
	(
	const JUtf8Byte			c1,
	const JUtf8Character&	c2
	)
{
	return !(c1 == c2);
}

inline int
operator!=
	(
	const JUtf8Character&	c1,
	const JUtf8Byte			c2
	)
{
	return !(c1 == c2);
}

inline int
operator!=
	(
	const JUtf8Byte*		c1,
	const JUtf8Character&	c2
	)
{
	return !(c1 == c2);
}

inline int
operator!=
	(
	const JUtf8Character&	c1,
	const JUtf8Byte*		c2
	)
{
	return !(c1 == c2);
}

#endif
