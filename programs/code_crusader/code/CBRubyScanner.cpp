/******************************************************************************
 CBRubyScanner.cpp

	Ruby lexer for keyword styling.

	BASE CLASS = public CBRubyFlexLexer

	Copyright (C) 2003 by John Lindal.

 *****************************************************************************/

#include "CBRubyScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBRubyScanner::CBRubyScanner()
	:
	CBRubyFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBRubyScanner::~CBRubyScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBRubyScanner::BeginScan
	(
	const JStyledText*				text,
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsCurrentText = text;
	itsResetFlag   = kJTrue;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);
	itsProbableOperatorFlag = kJFalse;
	itsHereDocTag.Clear();
	itsHereDocType = kDoubleQuoteString;

	switch_streams(&input, nullptr);
}

/******************************************************************************
 SlurpQuoted

	count is the number of times to slurp up to and including endChar.
	suffixList is the characters that can be appended, e.g., /foo/i

	Returns kJFalse if EOF was encountered.

 *****************************************************************************/

#define ClassName CBRubyScanner
#include "CBSTSlurpQuoted.th"
#undef ClassName
