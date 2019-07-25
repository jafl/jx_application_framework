/******************************************************************************
 CBCScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBCFlexLexer

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#include "CBCScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBCScanner::CBCScanner()
	:
	CBCFlexLexer(),
	itsResetFlag(kJFalse),
	itsCurrentRange()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBCScanner::~CBCScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBCScanner::BeginScan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	switch_streams(&input, nullptr);
}

/******************************************************************************
 Undo (protected)

	*** Only use this to back up to where StartToken() will be called!

 *****************************************************************************/

void
CBCScanner::Undo
	(
	const JStyledText::TextRange&	range,
	const JSize						prevCharByteCount,
	const JString&					text
	)
{
	for (JUnsignedOffset i=text.GetByteCount()-1; i>=0; i--)
		{
		yyunput(text.GetBytes()[i], yytext);
		}

	itsCurrentRange.charRange.first =
	itsCurrentRange.charRange.last  = range.charRange.first - 1;

	itsCurrentRange.byteRange.first = range.byteRange.first - prevCharByteCount;
	itsCurrentRange.byteRange.last  = range.byteRange.first - 1;
}

/******************************************************************************
 SavePPNameRange (private)

	Saves the range containing the name of the preprocessor directive.

 *****************************************************************************/

void
CBCScanner::SavePPNameRange()
{
	itsPPNameRange = itsCurrentRange;

	JIndex i=0;
	while (yytext[i] != '#' && yytext[i] != ':' && yytext[i] != '=')
		{
		i++;
		}
	assert( i < (JSize) yyleng );

	itsPPNameRange.charRange.first += JString::CountCharacters(yytext, i);
	itsPPNameRange.byteRange.first += i;
}
