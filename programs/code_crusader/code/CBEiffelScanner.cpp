/******************************************************************************
 CBEiffelScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBEiffelFlexLexer

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2004 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBEiffelScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBEiffelScanner::CBEiffelScanner()
	:
	CBEiffelFlexLexer(),
	itsResetFlag(kJFalse),
	itsCurrentRange()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBEiffelScanner::~CBEiffelScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBEiffelScanner::BeginScan
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
CBEiffelScanner::Undo
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
