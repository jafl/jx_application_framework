/******************************************************************************
 CBSampleText.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSampleText
#define _H_CBSampleText

#include <JXInputField.h>

class CBSampleText : public JXInputField
{
public:

	CBSampleText(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~CBSampleText();

	void	ShowRightMargin(const JBoolean show, const JColorIndex color);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

private:

	JBoolean	itsDrawRightMarginFlag;
	JColorIndex	itsRightMarginColor;

private:

	// not allowed

	CBSampleText(const CBSampleText& source);
	const CBSampleText& operator=(const CBSampleText& source);
};


/******************************************************************************
 ShowRightMargin

 ******************************************************************************/

inline void
CBSampleText::ShowRightMargin
	(
	const JBoolean		show,
	const JColorIndex	color
	)
{
	itsDrawRightMarginFlag = show;
	itsRightMarginColor    = color;
	Refresh();
}

#endif
