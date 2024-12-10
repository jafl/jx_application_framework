/******************************************************************************
 JUtf8ByteBuffer.cpp

	Manages an incomplete array of UTF-8 bytes.

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include "JUtf8ByteBuffer.h"
#include "JString.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JUtf8ByteBuffer::JUtf8ByteBuffer
	(
	const JSize minSize
	)
	:
	JArray<JUtf8Byte>(minSize)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUtf8ByteBuffer::~JUtf8ByteBuffer()
{
}

/******************************************************************************
 Append

 ******************************************************************************/

void
JUtf8ByteBuffer::Append
	(
	const std::span<const JUtf8Byte>& data
	)
{
	for (auto c: data)
	{
		AppendItem(c);
	}
}

/******************************************************************************
 ExtractCharacters

	Extracts as many valid characters as possible and also includes bytes
	that are clearly invalid, to avoid getting stuck.

 ******************************************************************************/

inline bool
isNull
	(
	const JUtf8Byte*	c,
	JSize*				byteCount
	)
{
	*byteCount = 1;
	return *c == '\0';
}

JString
JUtf8ByteBuffer::ExtractCharacters()
{
	const JUtf8Byte* p = GetCArray();

	JSize remainingCount = GetItemCount();
	JSize validCount     = 0;
	JSize byteCount;
	while (remainingCount > 0 &&
		   (isNull(p + validCount, &byteCount) ||
			JUtf8Character::IsCompleteCharacter(p + validCount, remainingCount, &byteCount) ||
			remainingCount >= JUtf8Character::kMaxByteCount))
	{
		validCount     += byteCount;
		remainingCount -= byteCount;
	}

	const JString s(p, validCount);
	RemoveNextItems(1, validCount);
	return s;
}
