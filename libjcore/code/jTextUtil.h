/******************************************************************************
 jTextUtil.h

   Copyright (C) 2017 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_jTextUtil
#define _H_jTextUtil

#include "JFont.h"

class JString;
class JTextEditor;

JFont	CalcWSFont(const JFont& prevFont, const JFont& nextFont);

JSize JAnalyzeWhitespace(const JString& buffer, const JSize tabWidth,
						 const JBoolean defaultUseSpaces,
						 JBoolean* useSpaces, JBoolean* isMixed);

void	JReadUNIXManOutput(std::istream& input, JTextEditor* te);
JSize	JPasteUNIXTerminalOutput(const JString& text, JTextEditor* te);

#endif
