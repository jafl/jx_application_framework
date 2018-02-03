/******************************************************************************
 CBCShellScanner.cpp

	csh lexer for keyword styling.

	BASE CLASS = public CBCShellFlexLexer

	Copyright (C) 2001 by John Lindal.

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
	std::istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
