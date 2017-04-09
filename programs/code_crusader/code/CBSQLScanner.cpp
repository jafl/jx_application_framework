/******************************************************************************
 CBSQLScanner.cpp

	bash lexer for keyword styling.

	BASE CLASS = public CBSQLFlexLexer

	Copyright (C) 2017 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBSQLScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBSQLScanner::CBSQLScanner()
	:
	CBSQLFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBSQLScanner::~CBSQLScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBSQLScanner::BeginScan
	(
	std::istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
