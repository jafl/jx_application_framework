/******************************************************************************
 JXFileNameDisplay.cpp

	Widget for displaying a file name.  It always scroll to display the
	end of the string since the file name is more important than the path.

	BASE CLASS = JXStaticText

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <JXFileNameDisplay.h>
#include <JXFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFileNameDisplay::JXFileNameDisplay
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
	JXStaticText(text, kJFalse, kJFalse, nullptr,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
	SetFont(GetFontManager()->GetDefaultMonospaceFont());
	SetBorderWidth(1);
	GoToEndOfLine();
	ListenTo(GetText());
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
	GoToEndOfLine();
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
	if (sender == GetText() && message.Is(JStyledText::kTextSet))
		{
		GoToEndOfLine();
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
		SetHint(GetText()->GetText());
		}
	else
		{
		ClearHint();
		}
}

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

	We do not need to expand to show the entire text.

 ******************************************************************************/

JCoordinate
JXFileNameDisplay::GetFTCMinContentSize
	(
	const JBoolean horizontal
	)
	const
{
	return (horizontal ? JXContainer::GetFTCMinContentSize(kJTrue) :
						 JXStaticText::GetFTCMinContentSize(kJFalse));
}
