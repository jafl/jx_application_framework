/******************************************************************************
 CBBourneShellScanner.cpp

	bash lexer for keyword styling.

	BASE CLASS = public CBBourneShellFlexLexer

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBBourneShellScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBBourneShellScanner::CBBourneShellScanner()
	:
	CBBourneShellFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBBourneShellScanner::~CBBourneShellScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBBourneShellScanner::BeginScan
	(
	istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
