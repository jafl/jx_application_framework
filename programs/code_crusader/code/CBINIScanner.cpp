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
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	switch_streams(&input, nullptr);
}
