/******************************************************************************
 CBEiffelScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBEiffelFlexLexer

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2004 by John Lindal.

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
CBEiffelScanner::Undo
	(
	const JUtf8ByteRange&	range,
	const JString&			text
	)
{
	for (JUnsignedOffset i=text.GetByteCount()-1; i>=0; i--)
		{
		yyunput(text.GetBytes()[i], yytext);
		}

	itsCurrentRange.first = itsCurrentRange.last = range.first - 1;
}
