/******************************************************************************
 CBEiffelScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBEiffelFlexLexer

	Copyright © 1997 by Dustin Laurence.
	Copyright © 2004 by John Lindal.

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
	itsResetFlag(kJFalse)
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
CBEiffelScanner::Undo
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
