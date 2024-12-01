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

	static const JSize kMaxByteCount = 4;
	static const JUInt32 kUtf32SubstitutionCharacter;
	static const JUtf8Character kUtf8SubstitutionCharacter;

public:

	JUtf8Character();

	explicit JUtf8Character(const JUtf8Byte asciiCharacter);	// avoid sneaky conversion from int or float
	explicit JUtf8Character(const JUtf8Byte* utf8Character);

	JUtf8Character& operator=(const JUtf8Byte asciiCharacter);
	JUtf8Character& operator=(const JUtf8Byte* utf8Character);

	bool				IsBlank() const;
	bool				IsAscii() const;
	JSize				GetByteCount() const;
	const JUtf8Byte*	GetBytes() const;
	JUtf8Byte*			AllocateBytes() const;	// client must call delete [] when finished with it
	JUInt32				GetUtf32() const;

	void	Set(const JUtf8Character& source);
	void	Set(const JUtf8Byte asciiCharacter);
	void	Set(const JUtf8Byte* utf8Character);

	bool	IsPrint() const;
	bool	IsAlnum() const;
	bool	IsDigit() const;
	bool	IsAlpha() const;
	bool	IsLower() const;
	bool	IsUpper() const;
	bool	IsSpace() const;
	bool	IsControl() const;

	JUtf8Character	ToLower() const;
	JUtf8Character	ToUpper() const;

	void	PrintHex(std::ostream& output) const;

	static bool	IsValid(const JUtf8Byte* utf8Character);
	static bool	IsCompleteCharacter(const JUtf8Byte* utf8Character, const JSize byteCount, JSize* characterByteCount);
	static bool	GetCharacterByteCount(const JUtf8Byte* utf8Character, JSize* byteCount);
	static bool	GetPrevCharacterByteCount(const JUtf8Byte* lastByte, JSize* byteCount);

	static JUtf8Character	Utf32ToUtf8(const JUInt32 c);
	static JUInt32			Utf8ToUtf32(const JUtf8Byte* c, JSize* returnByteCount = nullptr);

	static bool	IgnoreBadUtf8();
	static void	SetIgnoreBadUtf8(const bool ignore);

private:

	unsigned char	itsByteCount;
	JUtf8Byte		itsBytes[ kMaxByteCount+1 ];

	static bool	theIgnoreBadUtf8Flag;	// do not print errors
};


/******************************************************************************
 IsBlank

 ******************************************************************************/

inline bool
JUtf8Character::IsBlank()
	const
{
	return itsBytes[0] == 0;
}

/******************************************************************************
 IsAscii

 ******************************************************************************/

inline bool
JUtf8Character::IsAscii()
	const
{
	return itsByteCount == 1;
}

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

inline bool
JUtf8Character::IsValid
	(
	const JUtf8Byte* utf8Character
	)
{
	JSize byteCount;
	return GetCharacterByteCount(utf8Character, &byteCount);
}

/******************************************************************************
 IgnoreBadUtf8 (static)

 ******************************************************************************/

inline bool
JUtf8Character::IgnoreBadUtf8()
{
	return theIgnoreBadUtf8Flag;
}

inline void
JUtf8Character::SetIgnoreBadUtf8
	(
	const bool ignore
	)
{
	theIgnoreBadUtf8Flag = ignore;
}

/******************************************************************************
 Assignment operator

	We do not copy block size because we assume the client has set them
	appropriately.

 ******************************************************************************/

inline JUtf8Character&
JUtf8Character::operator=
	(
	const JUtf8Byte asciiCharacter
	)
{
	Set(asciiCharacter);
	return *this;
}

inline JUtf8Character&
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

inline bool
operator==
	(
	const JUtf8Character& c1,
	const JUtf8Character& c2
	)
{
	return (c1.GetByteCount() == c2.GetByteCount() &&
			memcmp(c1.GetBytes(), c2.GetBytes(), c1.GetByteCount()) == 0);
}

inline bool
operator==
	(
	const JUtf8Byte			c1,
	const JUtf8Character&	c2
	)
{
	return (c2.GetByteCount() == 1 && c1 == c2.GetBytes()[0]);
}

inline bool
operator==
	(
	const JUtf8Character&	c1,
	const JUtf8Byte			c2
	)
{
	return (c1.GetByteCount() == 1 && c1.GetBytes()[0] == c2);
}

inline bool
operator==
	(
	const JUtf8Byte*		c1,
	const JUtf8Character&	c2
	)
{
	return (strlen(c1) == c2.GetByteCount() &&
			memcmp(c1, c2.GetBytes(), c2.GetByteCount()) == 0);
}

inline bool
operator==
	(
	const JUtf8Character&	c1,
	const JUtf8Byte*		c2
	)
{
	return (c1.GetByteCount() == strlen(c2) &&
			memcmp(c1.GetBytes(), c2, c1.GetByteCount()) == 0);
}

#endif
