/******************************************************************************
 jTextUtil.h

   Copyright (C) 2017 by John Lindal.

 *****************************************************************************/

#ifndef _H_jTextUtil
#define _H_jTextUtil

#include "JStyledText.h"

JFont	CalcWSFont(const JFont& prevFont, const JFont& nextFont);

JSize	JAnalyzeWhitespace(const JString& buffer, const JSize tabWidth,
						   const JBoolean defaultUseSpaces,
						   JBoolean* useSpaces, JBoolean* isMixed);

void	JReadUNIXManOutput(std::istream& input, JStyledText* tb);

JStyledText::TextRange	JPasteUNIXTerminalOutput(const JString& text,
												 const JStyledText::TextIndex& pasteIndex,
												 JStyledText* tb);

#endif
