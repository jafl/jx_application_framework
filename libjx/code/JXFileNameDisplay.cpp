/******************************************************************************
 JXFileNameDisplay.cpp

	Widget for displaying a file name.  It always scroll to display the
	end of the string since the file name is more important than the path.

	BASE CLASS = JXStaticText

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFileNameDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFileNameDisplay::JXFileNameDisplay
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
	JXStaticText(text, kJFalse, kJFalse, NULL,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
	SetBorderWidth(1);
	SetCaretLocation(GetTextLength() + 1);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileNameDisplay::~JXFileNameDisplay()
{
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXFileNameDisplay::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStaticText::ApertureResized(dw,dh);
	SetCaretLocation(GetTextLength() + 1);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFileNameDisplay::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JTextEditor::kTextSet))
		{
		SetCaretLocation(GetTextLength() + 1);
		}

	JXStaticText::Receive(sender, message);
}

/******************************************************************************
 BoundsMoved (virtual protected)

	Show the full text in the hint if part of it is not visible in the widget.

 ******************************************************************************/

void
JXFileNameDisplay::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXStaticText::BoundsMoved(dx, dy);

	const JRect ap = GetAperture();
	if (ap.left != 0)
		{
		SetHint(GetText());
		}
	else
		{
		ClearHint();
		}
}
