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

int
GDBVarTreeScanner::LexerInput
	(
	char* flexBuf,
	int   flexBufSize
	)
{
	size_t numCopy;

	assert(flexBuf != NULL);
	assert(flexBufSize > 0);

	int charLeft = itsInputBuffer.GetLength() - itsBytesRead;
	numCopy = (charLeft > flexBufSize) ? flexBufSize : charLeft;

	if (numCopy > 0)
		{
		const JCharacter* buf = itsInputBuffer;
		(void) memcpy(flexBuf, buf + itsBytesRead, numCopy);
		itsBytesRead += numCopy;
		}

	return numCopy;
}
