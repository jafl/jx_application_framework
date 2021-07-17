/******************************************************************************
 StyledText.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_StyledText
#define _H_StyledText

#include <JStyledText.h>
#include <JSTStyler.h>

class StyledText : public JStyledText
{
public:

	StyledText(const bool useMultipleUndo = false);

	virtual ~StyledText();

	void	SetStyler(JSTStyler* styler);

	// expose protected functionality for testing

	TextIndex	GetWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetPartialWordStart(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetPartialWordEnd(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetParagraphStart(const JIndex charIndex, const JIndex byteIndex) const;
	TextIndex	GetParagraphEnd(const JIndex charIndex, const JIndex byteIndex) const;

protected:

	virtual void	AdjustStylesBeforeBroadcast(const JString& text,
												JRunArray<JFont>* styles,
												TextRange* recalcRange,
												TextRange* redrawRange,
												const bool deletion) override;

private:

	JSTStyler*						itsStyler;			// not owned; can be nullptr
	JArray<JSTStyler::TokenData>*	itsTokenStartList;

private:

	// not allowed

	StyledText(const StyledText& source);
	const StyledText& operator=(const StyledText& source);
};


/******************************************************************************
 SetStyler

 ******************************************************************************/

inline void
StyledText::SetStyler
	(
	JSTStyler* styler
	)
{
	itsStyler         = styler;
	itsTokenStartList = itsStyler->NewTokenStartList();
}

/******************************************************************************
 GetWordStart

 ******************************************************************************/

inline JStyledText::TextIndex
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

inline JStyledText::TextIndex
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

inline JStyledText::TextIndex
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

inline JStyledText::TextIndex
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

inline JStyledText::TextIndex
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

inline JStyledText::TextIndex
StyledText::GetParagraphEnd
	(
	const JIndex charIndex,
	const JIndex byteIndex
	)
	const
{
	return JStyledText::GetParagraphEnd(TextIndex(charIndex, byteIndex));
}

#endif
