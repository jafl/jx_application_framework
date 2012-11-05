/******************************************************************************
 CBJavaScanner.cpp

	Java lexer for keyword styling.

	BASE CLASS = public CBJavaFlexLexer

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cbStdInc.h>
#include "CBJavaScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBJavaScanner::CBJavaScanner()
	:
	CBJavaFlexLexer(),
	itsResetFlag(kJFalse),
	itsCurrentRange()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBJavaScanner::~CBJavaScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBJavaScanner::BeginScan
	(
	istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
