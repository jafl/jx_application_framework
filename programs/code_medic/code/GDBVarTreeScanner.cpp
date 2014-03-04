/******************************************************************************
 GDBVarTreeScanner.cpp

	BASE CLASS = public GDBVarFlexLexer

	Copyright © 1998 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include <GDBVarTreeScanner.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBVarTreeScanner::GDBVarTreeScanner
	(
	const JCharacter* text
	)
	:
	GDBVarFlexLexer(),
	itsInputBuffer(text),
	itsBytesRead(0),
	itsBracketCount(0)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBVarTreeScanner::~GDBVarTreeScanner()
{
}

/******************************************************************************
 LexerInput (virtual protected)

 *****************************************************************************/

FlexLexerInputSize
GDBVarTreeScanner::LexerInput
	(
	char*				flexBuf,
	FlexLexerInputSize	flexBufSize
	)
{
	assert(flexBuf != NULL);
	assert(flexBufSize > 0);

	const FlexLexerInputSize charLeft = itsInputBuffer.GetLength() - itsBytesRead;
	const FlexLexerInputSize numCopy  = (charLeft > flexBufSize) ? flexBufSize : charLeft;

	if (numCopy > 0)
		{
		memcpy(flexBuf, itsInputBuffer.GetCString() + itsBytesRead, numCopy);
		itsBytesRead += numCopy;
		}

	return numCopy;
}
