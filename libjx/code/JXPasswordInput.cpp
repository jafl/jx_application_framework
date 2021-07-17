/******************************************************************************
 JXPasswordInput.cpp

	Maintains a line of invisible text for entering a password.  Hashes are
	drawn in place of characters.  If Caps Lock is on, the hashes are drawn
	in red to provide a warning because one usually doesn't want all caps.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXPasswordInput.h"
#include "JXWindowPainter.h"
#include "JXDisplay.h"
#include "JXImageCache.h"
#include "JXImage.h"
#include "JXColorManager.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

const JCoordinate kDotDiameter = 8;
const JCoordinate kDotMargin   = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPasswordInput::JXPasswordInput
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
	SetIsRequired();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPasswordInput::~JXPasswordInput()
{
}

/******************************************************************************
 ClearPassword

	Not inline to avoid generating static string data in every object file.

 ******************************************************************************/

void
JXPasswordInput::ClearPassword()
{
	GetText()->SetText(JString::empty);
}

/******************************************************************************
 TERefreshRect (virtual protected)

	Override to refresh the entire widget, not just the line.

 ******************************************************************************/

void
JXPasswordInput::TERefreshRect
	(
	const JRect& rect
	)
{
	Refresh();
}

/******************************************************************************
 Draw (virtual protected)

	Draw hashes to indicate the length of the string.

 ******************************************************************************/

#include <jx_caps_lock_on.xpm>

void
JXPasswordInput::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const bool active = TEIsActive();
	const bool hasSel = HasSelection();

	p.ShiftOrigin(kMinLeftMarginWidth, 0);

	// fake selection

	JRect r = rect;
	r.Shrink(1,1);
	r.right -= kMinLeftMarginWidth;
	if (active && hasSel && TESelectionIsActive())
		{
		p.SetPenColor(GetSelectionColor());
		p.SetFilling(true);
		p.JPainter::Rect(r);
		p.SetFilling(false);
		}
	else if (active && hasSel)
		{
		p.SetPenColor(GetSelectionOutlineColor());
		p.JPainter::Rect(r);
		}

	// fake text

	p.SetPenColor(JColorManager::GetBlackColor());
	p.SetFilling(true);

	const JRect b = GetBounds();

	r.left  = kDotMargin;
	r.right = r.left + kDotDiameter;
	r.top = r.bottom = b.ycenter();
	r.Expand(0, kDotDiameter / 2);

	const JSize textLength = GetText()->GetText().GetCharacterCount();
	for (JIndex i=1; i<=textLength; i++)
		{
		p.JPainter::Ellipse(r);
		r.Shift(kDotDiameter + kDotMargin, 0);
		}

	p.SetFilling(false);

	// fake caret at end of text

	if (active && !hasSel && TECaretIsVisible())
		{
		p.SetPenColor(GetCaretColor());
		p.Line(r.left, 1, r.left, p.GetLineHeight());
		}

	// clean up

	p.ShiftOrigin(-kMinLeftMarginWidth, 0);

	// warn if Caps Lock is on

	if ((GetDisplay()->GetLatestKeyModifiers()).shiftLock())
		{
		JXImage* img = (GetDisplay()->GetImageCache())->GetImage(jx_caps_lock_on);
		p.JPainter::Image(*img, img->GetBounds(),
						  b.right - img->GetWidth() - 1,
						  b.ycenter() - img->GetHeight()/2);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXPasswordInput::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	SelectAll();
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXPasswordInput::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXPasswordInput::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
}

/******************************************************************************
 HandleKeyPress (virtual)

	We handle all the edit shortcuts here because we won't always have
	an Edit menu.

 ******************************************************************************/

void
JXPasswordInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJLeftArrow || c == kJRightArrow ||
		c == kJUpArrow   || c == kJDownArrow)
		{
		if (modifiers.shift())
			{
			SelectAll();
			}
		else
			{
			GoToEndOfLine();
			}
		}
	else
		{
		JXInputField::HandleKeyPress(c, keySym, modifiers);
		}
}
