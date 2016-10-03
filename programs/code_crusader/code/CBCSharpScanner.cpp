/******************************************************************************
 CBCSharpScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBCSharpFlexLexer

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2004 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBCSharpScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBCSharpScanner::CBCSharpScanner()
	:
	CBCSharpFlexLexer(),
	itsResetFlag(kJFalse),
	itsIsDocCommentFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBCSharpScanner::~CBCSharpScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBCSharpScanner::BeginScan
	(
	istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}

/******************************************************************************
 Undo (protected)

	*** Only use this to back up to where StartToken() will be called!

 *****************************************************************************/

void
CBCSharpScanner::Undo
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
CBCSharpScanner::SavePPNameRange()
{
	itsPPNameRange = itsCurrentRange;

	JIndex i=0;
	while (yytext[i] != '#')
		{
		i++;
		}
	assert( i < (JSize) yyleng );

	itsPPNameRange.first += i;
}
