/******************************************************************************
 TestString.cpp

	BASE CLASS = JString

	Written by John Lindal.

 ******************************************************************************/

#include "TestString.h"
#include <jAssert.h>

/******************************************************************************
 TestSearchForward

 ******************************************************************************/

JBoolean
TestString::TestSearchForward
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		caseSensitive,
	JIndex*				byteIndex
	)
	const
{
	return SearchForward(str, byteCount, caseSensitive, byteIndex);
}

/******************************************************************************
 TestSearchBackward

 ******************************************************************************/

JBoolean
TestString::TestSearchBackward
	(
	const JUtf8Byte*	str,
	const JSize			byteCount,
	const JBoolean		caseSensitive,
	JIndex*				byteIndex
	)
	const
{
	return SearchBackward(str, byteCount, caseSensitive, byteIndex);
}
