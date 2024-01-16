/******************************************************************************
 JXStaticText.cpp

	Maintains a text string.  Note that the font changing routines are
	merely for convenience.  Full control over the styles is available
	via the JTextEditor base class.

	BASE CLASS = JXTEBase

	Copyright (C) 1996-2017 by John Lindal.

 ******************************************************************************/

#include "JXStaticText.h"
#include "JXStyledText.h"
#include "jXConstants.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStaticText::JXStaticText
	(
	const JString&		text,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kStaticText, jnew JXStyledText(false, false, enclosure->GetFontManager()), true,
			 w==0, nullptr,
			 enclosure, hSizing, vSizing, x,y,
			 (w>0 ? w : 100), (h>0 ? h : 100))
{
	JXStaticTextX(text, w,h);
	SetBorderWidth(0);		// after margin width has been reduced
}

JXStaticText::JXStaticText
	(
	const JString&		text,
	const bool			wordWrap,
	const bool			selectable,
	const bool			allowStyles,
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
			 jnew JXStyledText(false, allowStyles, enclosure->GetFontManager()), true,
			 !wordWrap, scrollbarSet,
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
	const JString&		text,
	const JCoordinate	origW,
	const JCoordinate	origH
	)
{
	itsCenterHorizFlag = false;
	itsCenterVertFlag  = false;

	TESetLeftMarginWidth(kMinLeftMarginWidth);

	GetText()->SetText(text);

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
	return JXTEBase::ToString() + ": " + GetText().GetText();
}

/******************************************************************************
 SetToLabel

	Don't wrap text.  Center vertically (and optionally horizontally) in
	frame.  Useful for field labels in dialogs.

 ******************************************************************************/

void
JXStaticText::SetToLabel
	(
	const bool centerHorizontally
	)
{
	if (!itsCenterVertFlag)
	{
		itsCenterHorizFlag = centerHorizontally;
		itsCenterVertFlag  = true;
		SetBreakCROnly(true);
		ShouldAllowUnboundedScrolling(true);
		Center();
	}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXStaticText::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXTEBase::BoundsMoved(dx,dy);

	if (itsCenterHorizFlag || itsCenterVertFlag)
	{
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
	const bool horizontal
	)
	const
{
	return (horizontal ?
			(WillBreakCROnly() && !GetText().IsEmpty() && GetHSizing() != kHElastic ?
				TEGetMinPreferredGUIWidth() : GetBoundsWidth()) :
			GetBoundsHeight());
}
