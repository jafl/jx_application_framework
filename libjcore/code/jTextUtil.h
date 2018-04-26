/******************************************************************************
 jTextUtil.h

   Copyright (C) 2017 by John Lindal.

 *****************************************************************************/

#ifndef _H_jTextUtil
#define _H_jTextUtil

#include "JStyledTextBuffer.h"

class JString;
class JStyledTextBuffer;

JFont	CalcWSFont(const JFont& prevFont, const JFont& nextFont);

JSize JAnalyzeWhitespace(const JString& buffer, const JSize tabWidth,
						 const JBoolean defaultUseSpaces,
						 JBoolean* useSpaces, JBoolean* isMixed);

void	JReadUNIXManOutput(std::istream& input, JStyledTextBuffer* tb);
JSize	JPasteUNIXTerminalOutput(const JString& text,
								 const JStyledTextBuffer::TextIndex& pasteIndex,
								 JStyledTextBuffer* tb);

#endif
