/******************************************************************************
 CBJavaScanner.cpp

	Java lexer for keyword styling.

	BASE CLASS = public CBJavaFlexLexer

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#include "CBJavaScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBJavaScanner::CBJavaScanner()
	:
	CBJavaFlexLexer(),
	itsResetFlag(false),
	itsCurrentRange()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBJavaScanner::~CBJavaScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBJavaScanner::BeginScan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = true;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	switch_streams(&input, nullptr);
}
