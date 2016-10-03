/******************************************************************************
 CBSampleText.cpp

	BASE CLASS = JXInputField

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSampleText.h"
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <JFontManager.h>
#include <jAssert.h>

static const JCharacter* kSampleText = "I program therefore I am.";
static const JSize kRightMarginWidth = strlen(kSampleText);

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
	SetText(kSampleText);

	itsDrawRightMarginFlag = kJFalse;
	itsRightMarginColor    = GetColormap()->GetBlackColor();
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
			(kRightMarginWidth * GetDefaultFont().GetCharWidth(' '));

		const JColorIndex saveColor = p.GetPenColor();
		p.SetPenColor(itsRightMarginColor);
		p.Line(x, rect.top, x, rect.bottom);
		p.SetPenColor(saveColor);
		}

	JXInputField::Draw(p, rect);
}
