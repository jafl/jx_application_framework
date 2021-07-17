/******************************************************************************
 CBCSharpScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBCSharpFlexLexer

	Copyright © 1997 by Dustin Laurence.
	Copyright © 2004 by John Lindal.

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
	itsResetFlag(false),
	itsIsDocCommentFlag(false)
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
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = true;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	switch_streams(&input, nullptr);
}

/******************************************************************************
 Undo (protected)

	*** Only use this to back up to where StartToken() will be called!

 *****************************************************************************/

void
CBCSharpScanner::Undo
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
CBCSharpScanner::SavePPNameRange()
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
}
