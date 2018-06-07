/******************************************************************************
 CBPythonScanner.cpp

	C++ lexer for keyword styling.

	BASE CLASS = public CBPythonFlexLexer

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2004 by John Lindal.

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
	itsResetFlag(kJFalse),
	itsDoubleQuoteFlag(kJFalse)
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
CBPythonScanner::Undo
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
