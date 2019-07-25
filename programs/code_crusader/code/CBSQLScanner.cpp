/******************************************************************************
 CBSQLScanner.cpp

	bash lexer for keyword styling.

	BASE CLASS = public CBSQLFlexLexer

	Copyright (C) 2017 by John Lindal.

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
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	switch_streams(&input, nullptr);
}
