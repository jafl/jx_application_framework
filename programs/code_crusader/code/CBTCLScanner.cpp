/******************************************************************************
 CBTCLScanner.cpp

	TCL lexer for keyword styling.

	BASE CLASS = public CBTCLFlexLexer

	Copyright (C) 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBTCLScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBTCLScanner::CBTCLScanner()
	:
	CBTCLFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBTCLScanner::~CBTCLScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBTCLScanner::BeginScan
	(
	std::istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
