/******************************************************************************
 CBJavaScriptScanner.cpp

	JavaScript lexer for keyword styling.

	BASE CLASS = public CBJavaScriptFlexLexer

	Copyright © 2006 by John Lindal.

 *****************************************************************************/

#include "CBJavaScriptScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBJavaScriptScanner::CBJavaScriptScanner()
	:
	CBJavaScriptFlexLexer(),
	itsResetFlag(kJFalse),
	itsCurrentRange()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBJavaScriptScanner::~CBJavaScriptScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBJavaScriptScanner::BeginScan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);
	itsProbableOperatorFlag = kJFalse;

	switch_streams(&input, nullptr);
}
