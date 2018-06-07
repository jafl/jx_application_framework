/******************************************************************************
 CBINIScanner.cpp

	PHP .ini lexer for keyword styling.

	BASE CLASS = public CBINIFlexLexer

	Copyright (C) 2013 by John Lindal.

 *****************************************************************************/

#include "CBINIScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
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
	std::istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, nullptr);
}
