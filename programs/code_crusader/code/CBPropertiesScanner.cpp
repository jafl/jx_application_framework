/******************************************************************************
 CBPropertiesScanner.cpp

	Java .properties lexer for keyword styling.

	BASE CLASS = public CBPropertiesFlexLexer

	Copyright © 2015 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CBPropertiesScanner.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <ctype.h>
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
	istream& input
	)
{
	itsResetFlag = kJTrue;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));

	switch_streams(&input, NULL);
}
