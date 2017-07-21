/******************************************************************************
 JXStaticText.cpp

	Maintains a text string.  Note that the font changing routines are
	merely for convenience.  Full control over the styles is just a matter
	of using the JTextEditor functions.

	BASE CLASS = JXTEBase

	Copyright (C) 1996-2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStaticText.h>
#include <jXConstants.h>
#include <strstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStaticText::JXStaticText
	(
	const JCharacter*	text,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kStaticText, JI2B(w==0), kJFalse, NULL,
			 enclosure, hSizing, vSizing, x,y,
			 (w>0 ? w : 100), (h>0 ? h : 100))
{
	JXStaticTextX(text, w,h);
	SetBorderWidth(0);		// after margin width has been reduced
}

JXStaticText::JXStaticText
	(
	const JCharacter*	text,
	const JBoolean		wordWrap,
	const JBoolean		selectable,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase((selectable ? kSelectableText : kStaticText),
			 !wordWrap, kJFalse, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y,
			 (w>0 ? w : 100), (h>0 ? h : 100))
{
	assert( !wordWrap || w > 0 );
	JXStaticTextX(text, w,h);
}

// private

void
JXStaticText::JXStaticTextX
	(
	const JCharacter*	text,
	const JCoordinate	origW,
	const JCoordinate	origH
	)
{
	itsCenterHorizFlag = kJFalse;
	itsCenterVertFlag  = kJFalse;

	TESetLeftMarginWidth(kMinLeftMarginWidth);

	if (JCompareMaxN(text, "<html>", 6, kJFalse))
		{
		std::istrstream input(text, strlen(text));
		ReadHTML(input);
		}
	else
		{
		SetText(text);
		}

	JCoordinate w = origW;
	JCoordinate h = origH;
	if (w == 0)
		{
		w = GetMinBoundsWidth() + 2*GetBorderWidth();
		SetSize(w, GetFrameHeight());
		}
	if (h == 0)
		{
		h = GetMinBoundsHeight() + 2*GetBorderWidth();
		SetSize(w, h);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStaticText::~JXStaticText()
{
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JXStaticText::ToString()
	const
{
	return JXTEBase::ToString() + ": " + GetText();
}

/******************************************************************************
 SetToLabel

	Don't wrap text.  Center vertically (and optionally horizontally) in
	frame.  Useful for field labels in dialogs.

 ******************************************************************************/

void
JXStaticText::SetToLabel
	(
	const JBoolean centerHorizontally
	)
{
	if (!itsCenterVertFlag)
		{
		itsCenterHorizFlag = centerHorizontally;
		itsCenterVertFlag  = kJTrue;
		SetBreakCROnly(kJTrue);
		ShouldAllowUnboundedScrolling(kJTrue);
		Center();
		}
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXStaticText::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTEBase::BoundsResized(dw,dh);

	if (itsCenterHorizFlag || itsCenterVertFlag)
		{
		Center();
		}
}

/******************************************************************************
 Center (private)

 ******************************************************************************/

void
JXStaticText::Center()
{
	JCoordinate x = 0, y = 0;

	if (itsCenterHorizFlag)
		{
		const JCoordinate f = GetFrameWidth();
		const JCoordinate b = GetMinBoundsWidth();
		if (f > b)
			{
			x = - (f - b)/2;
			}
		}

	if (itsCenterVertFlag)
		{
		const JCoordinate f = GetFrameHeight();
		const JCoordinate b = GetMinBoundsHeight();
		if (f > b)
			{
			y = - (f - b)/2;
			}
		}

	ScrollTo(x, y);
}

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

 ******************************************************************************/

JCoordinate
JXStaticText::GetFTCMinContentSize
	(
	const JBoolean horizontal
	)
	const
{
	return (horizontal ?
			(WillBreakCROnly() && !GetText().IsEmpty() ?
				TEGetMinPreferredGUIWidth() : GetBoundsWidth()) :
			GetBoundsHeight());
}
