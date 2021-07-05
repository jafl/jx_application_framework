/******************************************************************************
 CBCShellScanner.cpp

	csh lexer for keyword styling.

	BASE CLASS = public CBCShellFlexLexer

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#include "CBCShellScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBCShellScanner::CBCShellScanner()
	:
	CBCShellFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBCShellScanner::~CBCShellScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBCShellScanner::BeginScan
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
