/******************************************************************************
 JXStaticText.cpp

	Maintains a text string.  Note that the font changing routines are
	merely for convenience.  Full control over the styles is just a matter
	of using the JTextEditor functions.

	BASE CLASS = JXTEBase

	Copyright (C) 1996 by John Lindal. All rights reserved.

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
	JXTEBase(kStaticText, JI2B(w==0), kJFalse, NULL,
			 enclosure, hSizing, vSizing, x,y,
			 (w>0 ? w : 100), (h>0 ? h : 100))
{
	JXStaticTextX(text, w,h);
	SetBorderWidth(0);		// after margin width has been reduced
}

JXStaticText::JXStaticText
	(
	const JString&		text,
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
	const JString&		text,
	const JCoordinate	origW,
	const JCoordinate	origH
	)
{
	itsIsLabelFlag = kJFalse;

	TESetLeftMarginWidth(kMinLeftMarginWidth);

	if (JString::CompareMaxNBytes(text.GetBytes(), "<html>", 6, kJFalse))
		{
		std::istrstream input(text.GetBytes(), text.GetByteCount());
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
 SetToLabel

	Don't wrap text.  Center vertically in frame.  Useful for field labels
	in dialogs.

 ******************************************************************************/

void
JXStaticText::SetToLabel()
{
	if (!itsIsLabelFlag)
		{
		itsIsLabelFlag = kJTrue;
		SetBreakCROnly(kJTrue);
		ShouldAllowUnboundedScrolling(kJTrue);
		CenterVertically();
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

	if (itsIsLabelFlag)
		{
		CenterVertically();
		}
}

/******************************************************************************
 CenterVertically (private)

 ******************************************************************************/

void
JXStaticText::CenterVertically()
{
	const JCoordinate f = GetFrameHeight();
	const JCoordinate b = GetMinBoundsHeight();
	if (f > b)
		{
		ScrollTo(0, - (f - b)/2);
		}
}
