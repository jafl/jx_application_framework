/******************************************************************************
 CMCommandInput.cpp

	BASE CLASS = JXTEBase

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "CMCommandInput.h"
#include "CMTextDisplayBase.h"
#include "cbmUtil.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* CMCommandInput::kReturnKeyPressed    = "ReturnKeyPressed::CMCommandInput";
const JCharacter* CMCommandInput::kTabKeyPressed       = "TabKeyPressed::CMCommandInput";
const JCharacter* CMCommandInput::kUpArrowKeyPressed   = "UpArrowKeyPressed::CMCommandInput";
const JCharacter* CMCommandInput::kDownArrowKeyPressed = "DownArrowKeyPressed::CMCommandInput";

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
	JXTEBase(kFullEditor, kJFalse, kJFalse, NULL,
			 enclosure, hSizing, vSizing, x,y,w,h)
{
	CMTextDisplayBase::AdjustFont(this);
	WantInput(kJTrue, kJTrue);
	SetBorderWidth(0);
	SetCharacterInWordFunction(CBMIsCharacterInWord);
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
	ClearUndo();
	SelectAll();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
CMCommandInput::HandleUnfocusEvent()
{
	JXTEBase::HandleUnfocusEvent();
	ClearUndo();
}

/******************************************************************************
 HandleKeyPress (virtual)

 *****************************************************************************/

void
CMCommandInput::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean allOff    = modifiers.AllOff();
	const JBoolean controlOn = modifiers.control();

	if (controlOn && key == JXCtrl('A'))
		{
		GoToBeginningOfLine();
		}
	else if (controlOn && key == JXCtrl('E'))
		{
		GoToEndOfLine();
		}

	else if (controlOn && key == JXCtrl('B'))
		{
		JXKeyModifiers newModifiers(GetDisplay());
		JXTEBase::HandleKeyPress(kJLeftArrow, newModifiers);
		}
	else if (controlOn && key == JXCtrl('F'))
		{
		JXKeyModifiers newModifiers(GetDisplay());
		JXTEBase::HandleKeyPress(kJRightArrow, newModifiers);
		}

	else if (controlOn && key == JXCtrl('D'))
		{
		JXKeyModifiers newModifiers(GetDisplay());
		JXTEBase::HandleKeyPress(modifiers.shift() ? kJDeleteKey : kJForwardDeleteKey, newModifiers);
		}

	else if (controlOn && key == JXCtrl('U'))
		{
		SetText("");
		}

	else if (controlOn && key == JXCtrl('K'))
		{
		JIndexRange r(GetInsertionIndex(), GetTextLength());
		if (!HasSelection() && !r.IsEmpty())
			{
			SetSelection(r);
			}
		Cut();
		}

	else if (key == kJReturnKey)
		{
		Broadcast(ReturnKeyPressed());
		}
	else if (key == kJTabKey && !(GetText()).IsEmpty())
		{
		Broadcast(TabKeyPressed());
		}
	else if (allOff && key == kJUpArrow)
		{
		Broadcast(UpArrowKeyPressed());
		}
	else if (allOff && key == kJDownArrow)
		{
		Broadcast(DownArrowKeyPressed());
		}

	else
		{
		JXTEBase::HandleKeyPress(key, modifiers);
		}
}
