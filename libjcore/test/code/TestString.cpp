/******************************************************************************
 TestString.cpp

	BASE CLASS = JString

	Written by John Lindal.

 ******************************************************************************/

#include "TestString.h"
#include "jAssert.h"

/******************************************************************************
 TestSearchForward

 ******************************************************************************/

bool
TestString::TestSearchForward
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JString::Case	caseSensitive,
	JIndex*				byteIndex
	)
	const
{
	return SearchForward(str, byteCount, caseSensitive, byteIndex);
}

/******************************************************************************
 TestSearchBackward

 ******************************************************************************/

bool
TestString::TestSearchBackward
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JString::Case	caseSensitive,
	JIndex*				byteIndex
	)
	const
{
	return SearchBackward(str, byteCount, caseSensitive, byteIndex);
}
