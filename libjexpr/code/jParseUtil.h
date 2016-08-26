/******************************************************************************
 jParseUtil.h

	Interface for jParseUtil module.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jParseUtil
#define _H_jParseUtil

#include <jTypes.h>

JBoolean JFindLastOperator(const JCharacter* sourceStart, const JSize sourceLength,
						   const JCharacter* substr, JSize* offset);

JBoolean JFindFirstOperator(const JCharacter* sourceStart, const JSize sourceLength,
							const JCharacter* substr, JSize* offset);

JSize    JStripParentheses(const JCharacter** expr, const JSize origLength);

JBoolean JStringsEqual(const JCharacter* s1, const JSize s1Length, const JCharacter* s2);
JBoolean JStringBeginsWith(const JCharacter* sourceStart, const JSize sourceLength,
						   const JCharacter* substr);
JBoolean JStringContains(const JCharacter* sourceStart, const JSize sourceLength,
						 const JCharacter* substr, JSize* offset);
JBoolean JStringEndsWith(const JCharacter* sourceStart, const JSize sourceLength,
						 const JCharacter* substr);

#endif
