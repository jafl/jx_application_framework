/******************************************************************************
 jTextUtil.h

   Copyright (C) 2017 by John Lindal.

 *****************************************************************************/

#ifndef _H_jTextUtil
#define _H_jTextUtil

#include "JStyledText.h"

extern const JRegex theUNIXTerminalFormatPattern;

JFont	JCalcWSFont(const JFont& prevFont, const JFont& nextFont);

JSize	JAnalyzeWhitespace(const JString& buffer, const JSize tabWidth,
						   const bool defaultUseSpaces,
						   bool* useSpaces, bool* isMixed);

void	JReadUNIXManOutput(std::istream& input, JStyledText* tb);
void	JReadLimitedMarkdown(const JString& text, JStyledText* st);

bool					JStripUNIXTerminalFormatting(JString* text);
JStyledText::TextRange	JPasteUNIXTerminalOutput(const JString& text,
												 const JStyledText::TextIndex& pasteIndex,
												 JStyledText* tb);

#endif
