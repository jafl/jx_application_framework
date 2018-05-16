/******************************************************************************
 StyledText.cpp

	Class to test JStyledText.

	BASE CLASS = JStyledText

	Written by John Lindal.

 ******************************************************************************/

#include "StyledText.h"
#include "JFontManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StyledText::StyledText
	(
	const JBoolean useMultipleUndo
	)
	:
	JStyledText(useMultipleUndo, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StyledText::~StyledText()
{
}

/******************************************************************************
 GetWordStart

 ******************************************************************************/

JIndex
StyledText::GetWordStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetWordStart(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetWordEnd

 ******************************************************************************/

JIndex
StyledText::GetWordEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetWordEnd(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetPartialWordStart

 ******************************************************************************/

JIndex
StyledText::GetPartialWordStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetPartialWordStart(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetPartialWordEnd

 ******************************************************************************/

JIndex
StyledText::GetPartialWordEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetPartialWordEnd(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetParagraphStart

 ******************************************************************************/

JIndex
StyledText::GetParagraphStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetParagraphStart(TextIndex(charIndex, byteIndex)).charIndex;
}

/******************************************************************************
 GetParagraphEnd

 ******************************************************************************/

JIndex
StyledText::GetParagraphEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetParagraphEnd(TextIndex(charIndex, byteIndex)).charIndex;
}
