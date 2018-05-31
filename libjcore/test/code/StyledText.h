/******************************************************************************
 StyledText.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_StyledText
#define _H_StyledText

#include <JStyledText.h>

class StyledText : public JStyledText
{
public:

	StyledText(const JBoolean useMultipleUndo = kJFalse);

	virtual ~StyledText();

	// expose protected functionality for testing

	TextIndex	GetWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetPartialWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetPartialWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetParagraphStart(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetParagraphEnd(const JIndex charIndex, const JIndex byteIndex) const;

private:

	// not allowed

	StyledText(const StyledText& source);
	const StyledText& operator=(const StyledText& source);
};

#endif
