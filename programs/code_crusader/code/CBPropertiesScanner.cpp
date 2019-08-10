/******************************************************************************
 CBPropertiesScanner.cpp

	Java .properties lexer for keyword styling.

	BASE CLASS = public CBPropertiesFlexLexer

	Copyright (C) 2015 by John Lindal.

 *****************************************************************************/

#include "CBPropertiesScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBPropertiesScanner::CBPropertiesScanner()
	:
	CBPropertiesFlexLexer(),
	itsResetFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBPropertiesScanner::~CBPropertiesScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBPropertiesScanner::BeginScan
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
