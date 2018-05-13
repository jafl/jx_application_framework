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

	StyledText();

	virtual ~StyledText();

	// expose protected functionality for testing

	JIndex	GetWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetPartialWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetPartialWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetParagraphStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetParagraphEnd(const JIndex charIndex, const JIndex byteIndex) const;

private:

	// not allowed

	StyledText(const StyledText& source);
	const StyledText& operator=(const StyledText& source);
};

#endif
