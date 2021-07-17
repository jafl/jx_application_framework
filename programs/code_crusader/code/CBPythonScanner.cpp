/******************************************************************************
 CBPythonScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBPythonFlexLexer

	Copyright © 1997 by Dustin Laurence.
	Copyright © 2004 by John Lindal.

 *****************************************************************************/

#include "CBPythonScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBPythonScanner::CBPythonScanner()
	:
	CBPythonFlexLexer(),
	itsResetFlag(false),
	itsDoubleQuoteFlag(false)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBPythonScanner::~CBPythonScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBPythonScanner::BeginScan
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
CBPythonScanner::Undo
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
