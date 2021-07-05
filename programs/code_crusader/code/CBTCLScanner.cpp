/******************************************************************************
 CBTCLScanner.cpp

	TCL lexer for keyword styling.

	BASE CLASS = public CBTCLFlexLexer

	Copyright © 2001 by John Lindal.

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
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	switch_streams(&input, nullptr);
}
