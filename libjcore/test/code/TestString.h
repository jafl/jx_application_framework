/******************************************************************************
 TestString.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestString
#define _H_TestString

#include <JString.h>

class TestString : public JString
{
public:

	JBoolean	TestSearchForward(const JUtf8Byte* str, const JSize byteCount,
								  const JBoolean caseSensitive, JIndex* byteIndex) const;
	JBoolean	TestSearchBackward(const JUtf8Byte* str, const JSize byteCount,
								   const JBoolean caseSensitive, JIndex* byteIndex) const;
};

#endif
