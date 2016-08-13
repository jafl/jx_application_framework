/******************************************************************************
 CBCShellScanner.cpp

	csh lexer for keyword styling.

	BASE CLASS = public CBCShellFlexLexer

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBCShellScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBCShellScanner::CBCShellScanner()
	:
	CBCShellFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBCShellScanner::~CBCShellScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBCShellScanner::BeginScan
	(
	istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
