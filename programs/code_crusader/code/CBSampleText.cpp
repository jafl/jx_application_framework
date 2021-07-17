/******************************************************************************
 CBSampleText.cpp

	BASE CLASS = JXInputField

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBSampleText.h"
#include <JXWindowPainter.h>
#include <JXColorManager.h>
#include <JFontManager.h>
#include <jAssert.h>

static const JString kSampleText("I program therefore I am.", JString::kNoCopy);
static const JSize kRightMarginWidth = kSampleText.GetCharacterCount();

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSampleText::CBSampleText
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	GetText()->SetText(kSampleText);

	itsDrawRightMarginFlag = false;
	itsRightMarginColor    = JColorManager::GetBlackColor();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSampleText::~CBSampleText()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CBSampleText::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsDrawRightMarginFlag)
		{
		const JCoordinate x =
			TEGetLeftMarginWidth() +
			(kRightMarginWidth *
			 GetText()->GetDefaultFont().GetCharWidth(
					GetFontManager(), JUtf8Character(' ')));

		const JColorID saveColor = p.GetPenColor();
		p.SetPenColor(itsRightMarginColor);
		p.Line(x, rect.top, x, rect.bottom);
		p.SetPenColor(saveColor);
		}

	JXInputField::Draw(p, rect);
}
