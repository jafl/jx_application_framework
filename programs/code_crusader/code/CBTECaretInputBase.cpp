/******************************************************************************
 CBTECaretInputBase.cpp

	Base class for input field controlling caret position.

	Derived classes must override:

		Act
			Position the caret in the given text editor.

		GetValue
			Extract the value from either the text editor or the message.

	BASE CLASS = JXIntegerInput

	Copyright © 1999-2017 by John Lindal.

 ******************************************************************************/

#include "CBTECaretInputBase.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXMenu.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTECaretInputBase::CBTECaretInputBase
	(
	JXStaticText*		label,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXIntegerInput(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsTE                 = nullptr;
	itsOrigValue          = 0;
	itsShouldActFlag      = false;
	itsOptimizeUpdateFlag = false;
	itsLabel              = label;

	SetLowerLimit(1);

	SetBorderWidth(0);
	itsLabel->SetBorderWidth(0);

	ShouldAllowUnboundedScrolling(true);
	Center();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTECaretInputBase::~CBTECaretInputBase()
{
}

/******************************************************************************
 SetTE

	We are created before the text editor so we can't get the pointer
	in our constructor.

 ******************************************************************************/

void
CBTECaretInputBase::SetTE
	(
	JXTEBase* te
	)
{
	itsTE = te;
	ListenTo(itsTE);

	// this re-orders the list so text editor gets focus

	WantInput(false);
	WantInput(true, false, true);
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

	Remember the original value.

 ******************************************************************************/

void
CBTECaretInputBase::HandleFocusEvent()
{
	JXIntegerInput::HandleFocusEvent();

	JXIntegerInput::GetValue(&itsOrigValue);
	itsLabel->SetBackColor(GetCurrBackColor());
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
CBTECaretInputBase::HandleUnfocusEvent()
{
	JXIntegerInput::HandleUnfocusEvent();

	if (itsShouldActFlag)
		{
		JInteger value;
		if (JXIntegerInput::GetValue(&value) && value != itsOrigValue)
			{
			Act(itsTE, value);
			}
		SetValue(GetValue(itsTE));
		}
	else
		{
		SetValue(itsOrigValue);
		}

	itsLabel->SetBackColor(GetCurrBackColor());
}

/******************************************************************************
 OKToUnfocus (virtual protected)

 ******************************************************************************/

bool
CBTECaretInputBase::OKToUnfocus()
{
	if (!itsShouldActFlag)
		{
		return true;
		}
	else
		{
		return JXIntegerInput::OKToUnfocus();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBTECaretInputBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTE && message.Is(JTextEditor::kCaretLocationChanged))
		{
		const auto* info =
			dynamic_cast<const JTextEditor::CaretLocationChanged*>(&message);
		assert( info != nullptr );
		if (itsOptimizeUpdateFlag)
			{
			GetWindow()->Update();	// avoid redrawing everything in between
			}
		SetValue(GetValue(*info));
		if (itsOptimizeUpdateFlag)
			{
			GetWindow()->Update();
			}
		}

	else
		{
		JXIntegerInput::Receive(sender, message);
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

	Escape cancels the changes.
	Return and tab switch focus to the text editor.
	Ctrl-tab is handled by the text editor.

 ******************************************************************************/

void
CBTECaretInputBase::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJEscapeKey)
		{
		itsTE->Focus();
		}
	else if (c == '\t' &&
			 !modifiers.GetState(kJXMetaKeyIndex)   &&
			 modifiers.GetState(kJXControlKeyIndex) &&
			 !modifiers.shift())
		{
		itsTE->Focus();
		itsTE->HandleKeyPress(c, keySym, modifiers);
		}
	else if ((c == '\r' || c == '\n') &&
			 !modifiers.meta() && !modifiers.control() && !modifiers.shift())
		{
		itsShouldActFlag = true;
		itsTE->Focus();				// trigger HandleUnfocusEvent()
		itsShouldActFlag = false;
		}
	else
		{
		JXIntegerInput::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
CBTECaretInputBase::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXIntegerInput::BoundsResized(dw,dh);
	Center();
}

/******************************************************************************
 Center (private)

 ******************************************************************************/

void
CBTECaretInputBase::Center()
{
	JCoordinate y = 0;

	const JCoordinate f = GetFrameHeight();
	const JCoordinate b = GetMinBoundsHeight();
	if (f > b)
		{
		y = - (f - b)/2;
		}

	ScrollTo(0, y);
}

/******************************************************************************
 GetFTCMinContentSize (virtual protected)

 ******************************************************************************/

JCoordinate
CBTECaretInputBase::GetFTCMinContentSize
	(
	const bool horizontal
	)
	const
{
	return (horizontal ?
			TEGetLeftMarginWidth() +
				GetText().GetDefaultFont().GetStringWidth(
					GetFontManager(), JString("00000", JString::kNoCopy)) :
			JXIntegerInput::GetFTCMinContentSize(horizontal));
}
