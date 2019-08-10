/******************************************************************************
 CBPerlScanner.cpp

	Perl lexer for keyword styling.

	BASE CLASS = public CBPerlFlexLexer

	Copyright (C) 2003 by John Lindal.

 *****************************************************************************/

#include "CBPerlScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBPerlScanner::CBPerlScanner()
	:
	CBPerlFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBPerlScanner::~CBPerlScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBPerlScanner::BeginScan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);
	itsProbableOperatorFlag = kJFalse;
	itsHereDocTag.Clear();
	itsHereDocType = kDoubleQuoteString;

	switch_streams(&input, nullptr);
}

/******************************************************************************
 SlurpQuoted

	count is the number of times to slurp up to and including endChar.
	suffixList is the characters that can be appended, e.g., s/a/b/g

	Returns kJFalse if EOF was encountered.

 *****************************************************************************/

JBoolean
CBPerlScanner::SlurpQuoted
	(
	const JSize			count,
	const JUtf8Byte*	suffixList
	)
{
	assert( count > 0 );

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

	JSize remainder = count;
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
		else if (c == endChar)	// check for endChar before '\\' because qq\abc\ is legal
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
					else if (strchr(suffixList, c) == nullptr)
						{
						yyunput(c, yytext);
						return kJTrue;
						}
					itsCurrentRange.last++;
					}
				}
			else if (count > 1 && remainder < count)
				{
				remainder--;	// compensate for extra openChar in s(a)(b)g
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

/******************************************************************************
 SavePPNameRange (private)

	Saves the range containing the name of the preprocessor directive.

 *****************************************************************************/

void
CBPerlScanner::SavePPNameRange()
{
	itsPPNameRange = itsCurrentRange;

	JIndex i=0;
	while (yytext[i] != '#')
		{
		i++;
		}
	assert( i < (JSize) yyleng );

	itsPPNameRange.charRange.first += JString::CountCharacters(yytext, i);
	itsPPNameRange.byteRange.first += i;

	i++;	// move past #
	while (isspace(yytext[i]))
		{
		i++;
		}
	assert( i < (JSize) yyleng );

	while (!isspace(yytext[i]))
		{
		i++;
		}
	assert( i < (JSize) yyleng );

	itsPPNameRange.charRange.last = itsCurrentRange.charRange.first + JString::CountCharacters(yytext, i-1);
	itsPPNameRange.byteRange.last = itsCurrentRange.byteRange.first + i-1;
}
