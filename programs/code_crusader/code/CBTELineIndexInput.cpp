/******************************************************************************
 CBTELineIndexInput.cpp

	Displays the line that the caret is on.

	BASE CLASS = CBTECaretInputBase

	Copyright © 1997 by John Lindal.

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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if ((c == '\r' || c == '\n') &&
		!modifiers.meta() && !modifiers.control() && modifiers.shift())
		{
		ShouldAct(true);
		itsColInput->Focus();	// trigger HandleUnfocusEvent()
		ShouldAct(false);
		}
	else if ((c == '\r' || c == '\n') &&
			 !modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex))    &&
			  modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXControlKeyIndex)) &&
			 !modifiers.shift())
		{
		JXTEBase* te = GetTE();
		JInteger value;
		if (JXIntegerInput::GetValue(&value))
			{
			const JString s((JUInt64) te->CRLineIndexToVisualLineIndex(value));
			GetText()->SetText(s);
			}

		ShouldAct(true);
		te->Focus();				// trigger HandleUnfocusEvent()
		ShouldAct(false);
		}
	else
		{
		CBTECaretInputBase::HandleKeyPress(c, keySym, modifiers);

		// vi emulation

		const JString& s = GetText()->GetText();
		if (s == "0")
			{
			GetText()->SetText(JString("1", JString::kNoCopy));
			GoToEndOfLine();
			}
		if (s == "$")
			{
			GetText()->SetText(JString((JUInt64) GetTE()->GetLineCount()+1));
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
	return te->GetLineForChar(te->GetInsertionCharIndex());
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
