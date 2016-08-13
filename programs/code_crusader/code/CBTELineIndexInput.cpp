/******************************************************************************
 CBTELineIndexInput.cpp

	Displays the line that the caret is on.

	BASE CLASS = CBTECaretInputBase

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBTELineIndexInput.h"
#include <JXMenu.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTELineIndexInput::CBTELineIndexInput
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
	CBTECaretInputBase(label, enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTELineIndexInput::~CBTELineIndexInput()
{
}

/******************************************************************************
 SetTE

	We are created before the column input so we can't get the pointer
	in our constructor.

 ******************************************************************************/

void
CBTELineIndexInput::SetTE
	(
	JXTEBase*			te,
	CBTECaretInputBase*	colInput
	)
{
	itsColInput = colInput;
	CBTECaretInputBase::SetTE(te);
}

/******************************************************************************
 HandleKeyPress (virtual)

	Shift-Return switches focus to the column input.

	Meta-Return converts from hard to soft line breaks.

 ******************************************************************************/

void
CBTELineIndexInput::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if ((key == '\r' || key == '\n') &&
		!modifiers.meta() && !modifiers.control() && modifiers.shift())
		{
		ShouldAct(kJTrue);
		itsColInput->Focus();	// trigger HandleUnfocusEvent()
		ShouldAct(kJFalse);
		}
	else if ((key == '\r' || key == '\n') &&
			 !modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex))    &&
			  modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXControlKeyIndex)) &&
			 !modifiers.shift())
		{
		JXTEBase* te = GetTE();
		JInteger value;
		if (JXIntegerInput::GetValue(&value))
			{
			const JString s(te->CRLineIndexToVisualLineIndex(value), JString::kBase10);
			SetText(s);
			}

		ShouldAct(kJTrue);
		te->Focus();				// trigger HandleUnfocusEvent()
		ShouldAct(kJFalse);
		}
	else
		{
		CBTECaretInputBase::HandleKeyPress(key, modifiers);

		// vi emulation

		const JString& s = GetText();
		if (s == "0")
			{
			SetText("1");
			GoToEndOfLine();
			}
		if (s == "$")
			{
			SetText(JString(GetTE()->GetLineCount()+1, JString::kBase10));
			GoToEndOfLine();
			}
		}
}

/******************************************************************************
 Act (virtual protected)

 ******************************************************************************/

void
CBTELineIndexInput::Act
	(
	JXTEBase*		te,
	const JIndex	value
	)
{
	te->GoToLine(value);
}

/******************************************************************************
 GetValue (virtual protected)

 ******************************************************************************/

JIndex
CBTELineIndexInput::GetValue
	(
	JXTEBase* te
	)
	const
{
	return te->GetLineForChar(te->GetInsertionIndex());
}

JIndex
CBTELineIndexInput::GetValue
	(
	const JTextEditor::CaretLocationChanged& info
	)
	const
{
	return info.GetLineIndex();
}
