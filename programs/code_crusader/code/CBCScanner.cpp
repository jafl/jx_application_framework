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
	std::istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, nullptr);
}

/******************************************************************************
 Undo (protected)

	*** Only use this to back up to where StartToken() will be called!

 *****************************************************************************/

void
CBCScanner::Undo
	(
	const JIndexRange&	range,
	const JString&		text
	)
{
	for (JIndex i=text.GetLength(); i>=1; i--)
		{
		yyunput(text.GetCharacter(i), yytext);
		}

	itsCurrentRange.first = itsCurrentRange.last = range.first - 1;
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

	itsPPNameRange.first += i;
}
