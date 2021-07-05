/******************************************************************************
 CBDScanner.cpp

	D lexer for keyword styling.

	BASE CLASS = public CBDFlexLexer

	Copyright © 1997 by Dustin Laurence.
	Copyright © 2021 by John Lindal.

 *****************************************************************************/

#include "CBDScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBDScanner::CBDScanner()
	:
	CBDFlexLexer(),
	itsResetFlag(kJFalse),
	itsCurrentRange()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBDScanner::~CBDScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBDScanner::BeginScan
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
