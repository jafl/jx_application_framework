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

JStyledText::TextIndex
StyledText::GetWordStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetWordStart(TextIndex(charIndex, byteIndex));
}

/******************************************************************************
 GetWordEnd

 ******************************************************************************/

JStyledText::TextIndex
StyledText::GetWordEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetWordEnd(TextIndex(charIndex, byteIndex));
}

/******************************************************************************
 GetPartialWordStart

 ******************************************************************************/

JStyledText::TextIndex
StyledText::GetPartialWordStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetPartialWordStart(TextIndex(charIndex, byteIndex));
}

/******************************************************************************
 GetPartialWordEnd

 ******************************************************************************/

JStyledText::TextIndex
StyledText::GetPartialWordEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetPartialWordEnd(TextIndex(charIndex, byteIndex));
}

/******************************************************************************
 GetParagraphStart

 ******************************************************************************/

JStyledText::TextIndex
StyledText::GetParagraphStart
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetParagraphStart(TextIndex(charIndex, byteIndex));
}

/******************************************************************************
 GetParagraphEnd

 ******************************************************************************/

JStyledText::TextIndex
StyledText::GetParagraphEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetParagraphEnd(TextIndex(charIndex, byteIndex));
}
