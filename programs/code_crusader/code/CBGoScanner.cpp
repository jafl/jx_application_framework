/******************************************************************************
 CBGoScanner.cpp

	Go lexer for keyword styling.

	BASE CLASS = public CBGoFlexLexer

	Copyright © 2021 by John Lindal.

 *****************************************************************************/

#include "CBGoScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBGoScanner::CBGoScanner()
	:
	CBGoFlexLexer(),
	itsResetFlag(kJFalse),
	itsCurrentRange()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBGoScanner::~CBGoScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBGoScanner::BeginScan
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
