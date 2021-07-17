/******************************************************************************
 CBPerlScanner.cpp

	Perl lexer for keyword styling.

	BASE CLASS = public CBPerlFlexLexer

	Copyright © 2003 by John Lindal.

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
	itsResetFlag(false)
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
	const JStyledText*				text,
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsCurrentText = text;
	itsResetFlag   = true;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);
	itsProbableOperatorFlag = false;
	itsHereDocTag.Clear();
	itsHereDocType = kDoubleQuoteString;

	switch_streams(&input, nullptr);
}

/******************************************************************************
 SlurpQuoted

	count is the number of times to slurp up to and including endChar.
	suffixList is the characters that can be appended, e.g., s/a/b/g

	Returns false if EOF was encountered.

 *****************************************************************************/

#define ClassName CBPerlScanner
#include "CBSTSlurpQuoted.th"
#undef ClassName

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
