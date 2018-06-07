/******************************************************************************
 CBJavaScriptScanner.cpp

	JavaScript lexer for keyword styling.

	BASE CLASS = public CBJavaScriptFlexLexer

	Copyright (C) 2006 by John Lindal.

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
	std::istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));
	itsProbableOperatorFlag = kJFalse;

	switch_streams(&input, nullptr);
}
