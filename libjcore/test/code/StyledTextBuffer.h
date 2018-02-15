/******************************************************************************
 StyledTextBuffer.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_StyledTextBuffer
#define _H_StyledTextBuffer

#include <JStyledTextBuffer.h>

class StyledTextBuffer : public JStyledTextBuffer
{
public:

	StyledTextBuffer();

	virtual ~StyledTextBuffer();

	// expose protected functionality for testing

	JIndex	GetWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetPartialWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetPartialWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetParagraphStart(const JIndex charIndex, const JIndex byteIndex) const;
	JIndex	GetParagraphEnd(const JIndex charIndex, const JIndex byteIndex) const;

private:

	// not allowed

	StyledTextBuffer(const StyledTextBuffer& source);
	const StyledTextBuffer& operator=(const StyledTextBuffer& source);
};

#endif
