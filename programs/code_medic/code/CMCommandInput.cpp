/******************************************************************************
 CMCommandInput.cpp

	BASE CLASS = JXTEBase

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMCommandInput.h"
#include "CMTextDisplayBase.h"
#include "cbmUtil.h"
#include <JXStyledText.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CMCommandInput::kReturnKeyPressed    = "ReturnKeyPressed::CMCommandInput";
const JUtf8Byte* CMCommandInput::kTabKeyPressed       = "TabKeyPressed::CMCommandInput";
const JUtf8Byte* CMCommandInput::kUpArrowKeyPressed   = "UpArrowKeyPressed::CMCommandInput";
const JUtf8Byte* CMCommandInput::kDownArrowKeyPressed = "DownArrowKeyPressed::CMCommandInput";

/******************************************************************************
 Constructor

 *****************************************************************************/

CMCommandInput::CMCommandInput
	(
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor,
			 jnew JXStyledText(false, false, enclosure->GetFontManager()),
			 true, true, nullptr,
			 enclosure, hSizing, vSizing, x,y,w,h)
{
	CMTextDisplayBase::AdjustFont(this);
	WantInput(true, true);
	SetBorderWidth(0);
	GetText()->SetCharacterInWordFunction(CBMIsCharacterInWord);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMCommandInput::~CMCommandInput()
{
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
CMCommandInput::HandleFocusEvent()
{
	JXTEBase::HandleFocusEvent();
	GetText()->ClearUndo();
	SelectAll();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
CMCommandInput::HandleUnfocusEvent()
{
	JXTEBase::HandleUnfocusEvent();
	GetText()->ClearUndo();
}

/******************************************************************************
 HandleKeyPress (virtual)

 *****************************************************************************/

void
CMCommandInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	const bool allOff    = modifiers.AllOff();
	const bool controlOn = modifiers.control();

	if (controlOn && c == JXCtrl('A'))
		{
		GoToBeginningOfLine();
		}
	else if (controlOn && c == JXCtrl('E'))
		{
		GoToEndOfLine();
		}

	else if (controlOn && c == JXCtrl('B'))
		{
		JXKeyModifiers newModifiers(GetDisplay());
		JXTEBase::HandleKeyPress(JUtf8Character(kJLeftArrow), 0, newModifiers);
		}
	else if (controlOn && c == JXCtrl('F'))
		{
		JXKeyModifiers newModifiers(GetDisplay());
		JXTEBase::HandleKeyPress(JUtf8Character(kJRightArrow), 0, newModifiers);
		}

	else if (controlOn && c == JXCtrl('D'))
		{
		JXKeyModifiers newModifiers(GetDisplay());
		JXTEBase::HandleKeyPress(JUtf8Character(modifiers.shift() ? kJDeleteKey : kJForwardDeleteKey), 0, newModifiers);
		}

	else if (controlOn && c == JXCtrl('U'))
		{
		GetText()->SetText(JString::empty);
		}

	else if (controlOn && c == JXCtrl('K'))
		{
		const JCharacterRange r(GetInsertionIndex().charIndex, GetText()->GetText().GetCharacterCount());
		if (!HasSelection() && !r.IsEmpty())
			{
			SetSelection(r);
			}
		Cut();
		}

	else if (c == kJReturnKey)
		{
		Broadcast(ReturnKeyPressed());
		}
	else if (c == kJTabKey && !GetText()->GetText().IsEmpty())
		{
		Broadcast(TabKeyPressed());
		}
	else if (allOff && c == kJUpArrow)
		{
		Broadcast(UpArrowKeyPressed());
		}
	else if (allOff && c == kJDownArrow)
		{
		Broadcast(DownArrowKeyPressed());
		}

	else
		{
		JXTEBase::HandleKeyPress(c, keySym, modifiers);
		}
}
