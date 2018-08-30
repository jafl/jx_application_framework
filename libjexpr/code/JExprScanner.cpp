/******************************************************************************
 JExprScanner.cpp

	BASE CLASS = public JExprFlexLexer

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JExprScanner.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JExprScanner::JExprScanner
	(
	const JString& text
	)
	:
	JExprFlexLexer(),
	itsInputBuffer(text),
	itsBytesRead(0)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JExprScanner::~JExprScanner()
{
}

/******************************************************************************
 LexerInput (virtual protected)

 *****************************************************************************/

FlexLexerInputSize
JExprScanner::LexerInput
	(
	char*				flexBuf,
	FlexLexerInputSize	flexBufSize
	)
{
	assert(flexBuf != NULL);
	assert(flexBufSize > 0);

	const FlexLexerInputSize bytesLeft = itsInputBuffer.GetByteCount() - itsBytesRead;
	const FlexLexerInputSize numCopy   = (bytesLeft > flexBufSize) ? flexBufSize : bytesLeft;

	if (numCopy > 0)
		{
		memcpy(flexBuf, itsInputBuffer.GetRawBytes() + itsBytesRead, numCopy);
		itsBytesRead += numCopy;
		}

	return numCopy;
}
