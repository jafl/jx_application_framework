/******************************************************************************
 CBINIScanner.cpp

	PHP .ini lexer for keyword styling.

	BASE CLASS = public CBINIFlexLexer

	Copyright © 2013 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBINIScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <ctype.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBINIScanner::CBINIScanner()
	:
	CBINIFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBINIScanner::~CBINIScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBINIScanner::BeginScan
	(
	istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
