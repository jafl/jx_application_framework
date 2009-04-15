/******************************************************************************
 JXLinkText.cpp

	Implements algorithms required to make portions of the text act
	like buttons.

	Derived classes must implement:

		GetLinkCount
			Returns the number of links in the text.

		GetLinkRange
			Returns the character range of the specified link.

		LinkClicked
			Take whatever action is appropriate for the specified link.

	BASE CLASS = JXTEBase

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXLinkText.h>
#include <JXDisplay.h>
#include <X11/cursorfont.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXLinkText::JXLinkText
	(
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
	JXTEBase(kSelectableText, kJFalse, kJTrue, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	itsLinkCursor     = (GetDisplay())->CreateBuiltInCursor("XC_hand2", XC_hand2);
	itsMouseDownIndex = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXLinkText::~JXLinkText()
{
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
JXLinkText::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	const JSize linkCount = GetLinkCount();
	if (linkCount > 0)
		{
		const CaretLocation caretLoc = CalcCaretLocation(pt);

		JIndexRange selRange;
		if (!GetSelection(&selRange) || !selRange.Contains(caretLoc.charIndex))
			{
			for (JIndex i=1; i<=linkCount; i++)
				{
				const JIndexRange linkRange = GetLinkRange(i);
				if (linkRange.Contains(caretLoc.charIndex))
					{
					DisplayCursor(itsLinkCursor);
					return;
					}
				}
			}
		}

	JXTEBase::AdjustCursor(pt, modifiers);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXLinkText::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXTEBase::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);

	itsMouseDownIndex = 0;
	if (button == kJXLeftButton && clickCount == 1 &&
		!modifiers.shift() && !modifiers.meta() && !modifiers.control() &&
		!GetCaretLocation(&itsMouseDownIndex))
		{
		itsMouseDownIndex = 0;
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXLinkText::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXTEBase::HandleMouseUp(pt, button, buttonStates, modifiers);

	JIndex charIndex;
	if (GetCaretLocation(&charIndex) && charIndex == itsMouseDownIndex)
		{
		const JSize linkCount = GetLinkCount();
		for (JIndex i=1; i<=linkCount; i++)
			{
			const JIndexRange linkRange = GetLinkRange(i);
			if (linkRange.Contains(itsMouseDownIndex))
				{
				LinkClicked(i);
				break;
				}
			}
		}

	itsMouseDownIndex = 0;
}
