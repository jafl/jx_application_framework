/******************************************************************************
 JXPasswordInput.cpp

	Maintains a line of invisible text for entering a password.  Hashes are
	drawn in place of characters.  If Caps Lock is on, the hashes are drawn
	in red to provide a warning because one usually doesn't want all caps.

	BASE CLASS = JXInputField

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPasswordInput.h>
#include <JXWindowPainter.h>
#include <JXDisplay.h>
#include <JColormap.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JCharacter kPlaceholderChar = '#';

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

	itsFakeString = new JString;
	assert( itsFakeString != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPasswordInput::~JXPasswordInput()
{
	delete itsFakeString;
}

/******************************************************************************
 ClearPassword

	Not inline to avoid generating static string data in every object file.

 ******************************************************************************/

void
JXPasswordInput::ClearPassword()
{
	SetText("");
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

void
JXPasswordInput::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JBoolean active = TEIsActive();
	const JBoolean hasSel = HasSelection();

	p.ShiftOrigin(kMinLeftMarginWidth, 0);

	// fake selection

	JRect r = rect;
	r.Shrink(1,1);
	if (active && hasSel && TESelectionIsActive())
		{
		p.SetPenColor(GetSelectionColor());
		p.SetFilling(kJTrue);
		p.JPainter::Rect(r);
		p.SetFilling(kJFalse);
		}
	else if (active && hasSel)
		{
		p.SetPenColor(GetSelectionOutlineColor());
		p.JPainter::Rect(r);
		}

	// generate fake string to match text length

	const JSize textLength = GetTextLength();
	if (itsFakeString->GetLength() > textLength)
		{
		itsFakeString->RemoveSubstring(textLength+1, itsFakeString->GetLength());
		}
	else
		{
		while (itsFakeString->GetLength() < textLength)
			{
			itsFakeString->AppendCharacter(kPlaceholderChar);
			}
		}

	// warn if Caps Lock is on

	if (((GetDisplay())->GetLatestKeyModifiers()).shiftLock())
		{
		p.SetFontStyle(JFontStyle(kJFalse, kJFalse, 0, kJFalse,
					   (p.GetColormap())->GetRedColor()));
		}

	// fake text

	p.JPainter::String(GetBounds(), *itsFakeString);

	// fake caret at end of text

	if (active && !hasSel && TECaretIsVisible())
		{
		const JCoordinate x = p.GetStringWidth(*itsFakeString);

		p.SetPenColor(GetCaretColor());
		p.Line(x, 1, x, p.GetLineHeight());
		}

	// clean up

	p.ShiftOrigin(-kMinLeftMarginWidth, 0);
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
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJLeftArrow || key == kJRightArrow ||
		key == kJUpArrow   || key == kJDownArrow)
		{
		if (modifiers.shift())
			{
			SelectAll();
			}
		else
			{
			SetCaretLocation(GetTextLength()+1);
			}
		}
	else
		{
		JXInputField::HandleKeyPress(key, modifiers);
		}
}
