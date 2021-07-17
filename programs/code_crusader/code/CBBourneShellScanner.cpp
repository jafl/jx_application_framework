/******************************************************************************
 CBBourneShellScanner.cpp

	bash lexer for keyword styling.

	BASE CLASS = public CBBourneShellFlexLexer

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#include "CBBourneShellScanner.h"
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBBourneShellScanner::CBBourneShellScanner()
	:
	CBBourneShellFlexLexer(),
	itsResetFlag(false)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBBourneShellScanner::~CBBourneShellScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBBourneShellScanner::BeginScan
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
