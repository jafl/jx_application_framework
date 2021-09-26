/******************************************************************************
 TestString.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestString
#define _H_TestString

#include <jx-af/jcore/JString.h>

class TestString : public JString
{
public:

	bool	TestSearchForward(const JUtf8Byte* str, const JSize byteCount,
							  const JString::Case caseSensitive, JIndex* byteIndex) const;
	bool	TestSearchBackward(const JUtf8Byte* str, const JSize byteCount,
							   const JString::Case caseSensitive, JIndex* byteIndex) const;
};

#endif
