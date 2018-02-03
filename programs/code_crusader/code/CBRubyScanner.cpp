/******************************************************************************
 CBRubyScanner.cpp

	Ruby lexer for keyword styling.

	BASE CLASS = public CBRubyFlexLexer

	Copyright (C) 2003 by John Lindal.

 *****************************************************************************/

#include "CBRubyScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <ctype.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBRubyScanner::CBRubyScanner()
	:
	CBRubyFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBRubyScanner::~CBRubyScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBRubyScanner::BeginScan
	(
	std::istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));
	itsProbableOperatorFlag = kJFalse;
	itsHereDocTag.Clear();
	itsHereDocType = kDoubleQuoteString;

	switch_streams(&input, NULL);
}

/******************************************************************************
 SlurpQuoted

	suffixList is the characters that can be appended, e.g., /foo/i

	Returns kJFalse if EOF was encountered.

 *****************************************************************************/

JBoolean
CBRubyScanner::SlurpQuoted
	(
	const JCharacter* suffixList
	)
{
	const int startChar = yyinput();
	if (startChar == EOF || startChar == 0)
		{
		return kJFalse;
		}
	itsCurrentRange.last++;

	int endChar = startChar;
	if (startChar == '(')
		{
		endChar = ')';
		}
	else if (startChar == '<')
		{
		endChar = '>';
		}
	else if (startChar == '[')
		{
		endChar = ']';
		}
	else if (startChar == '{')
		{
		endChar = '}';
		}

	JSize remainder = 1;
	while (1)
		{
		int c = yyinput();
		if (c == EOF || c == 0)
			{
			return kJFalse;
			}
		itsCurrentRange.last++;

		if (startChar != endChar && c == startChar)
			{
			remainder++;
			}
		else if (c == endChar)	// check for endChar before '\\' because %r\abc\ is legal
			{
			if (remainder > 0)
				{
				remainder--;
				}
			if (remainder == 0)
				{
				while (1)		// slurp in characters in suffixList
					{
					c = yyinput();
					if (c == EOF || c == 0)
						{
						return kJTrue;
						}
					else if (strchr(suffixList, c) == NULL)
						{
						yyunput(c, yytext);
						return kJTrue;
						}
					itsCurrentRange.last++;
					}
				}
			}
		else if (c == '\\')
			{
			c = yyinput();
			if (c == EOF || c == 0)
				{
				return kJFalse;
				}
			itsCurrentRange.last++;
			}
		}
}
