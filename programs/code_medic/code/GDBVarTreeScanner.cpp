/******************************************************************************
 GDBVarTreeScanner.cpp

	BASE CLASS = public GDBVarFlexLexer

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include <GDBVarTreeScanner.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBVarTreeScanner::GDBVarTreeScanner
	(
	const JString& text
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
	assert(flexBuf != nullptr);
	assert(flexBufSize > 0);

	const FlexLexerInputSize charLeft = itsInputBuffer.GetByteCount() - itsBytesRead;
	const FlexLexerInputSize numCopy  = (charLeft > flexBufSize) ? flexBufSize : charLeft;

	if (numCopy > 0)
		{
		memcpy(flexBuf, itsInputBuffer.GetBytes() + itsBytesRead, numCopy);
		itsBytesRead += numCopy;
		}

	return numCopy;
}
