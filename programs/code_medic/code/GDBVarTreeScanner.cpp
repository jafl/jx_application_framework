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

size_t
GDBVarTreeScanner::LexerInput
	(
	char*  flexBuf,
	size_t flexBufSize
	)
{
	assert(flexBuf != NULL);
	assert(flexBufSize > 0);

	const int charLeft   = itsInputBuffer.GetLength() - itsBytesRead;
	const size_t numCopy = (charLeft > flexBufSize) ? flexBufSize : charLeft;

	if (numCopy > 0)
		{
		const JCharacter* buf = itsInputBuffer;
		(void) memcpy(flexBuf, buf + itsBytesRead, numCopy);
		itsBytesRead += numCopy;
		}

	return numCopy;
}
