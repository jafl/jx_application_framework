/******************************************************************************
 StyledTextBuffer.cpp

	Class to test JStyledTextBuffer.

	BASE CLASS = JStyledTextBuffer

	Written by John Lindal.

 ******************************************************************************/

#include "StyledTextBuffer.h"
#include "TEFontManager.h"
#include "TEColormap.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StyledTextBuffer::StyledTextBuffer()
	:
	JStyledTextBuffer(kFullEditor, kJTrue, kJFalse,
				jnew TEFontManager,
				jnew TEColormap,
				1,1,1,1,1, 500),
	itsClipText(NULL),
	itsClipStyle(NULL)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StyledTextBuffer::~StyledTextBuffer()
{
}

/******************************************************************************
 GetWordStart

 ******************************************************************************/

JIndex
StyledTextBuffer::GetWordStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledTextBuffer::GetWordStart(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetWordEnd

 ******************************************************************************/

JIndex
StyledTextBuffer::GetWordEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledTextBuffer::GetWordEnd(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetPartialWordStart

 ******************************************************************************/

JIndex
StyledTextBuffer::GetPartialWordStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledTextBuffer::GetPartialWordStart(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetPartialWordEnd

 ******************************************************************************/

JIndex
StyledTextBuffer::GetPartialWordEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledTextBuffer::GetPartialWordEnd(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetParagraphStart

 ******************************************************************************/

JIndex
StyledTextBuffer::GetParagraphStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledTextBuffer::GetParagraphStart(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetParagraphEnd

 ******************************************************************************/

JIndex
StyledTextBuffer::GetParagraphEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledTextBuffer::GetParagraphEnd(TextIndex(charIndex, byteIndex)).charIndex;
}
