/******************************************************************************
 CBTCLScanner.cpp

	TCL lexer for keyword styling.

	BASE CLASS = public CBTCLFlexLexer

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cbStdInc.h>
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
	istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
