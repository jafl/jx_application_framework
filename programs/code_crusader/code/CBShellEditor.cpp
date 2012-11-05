/******************************************************************************
 CBShellEditor.cpp

	BASE CLASS = CBTextEditor

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBShellEditor.h"
#include "CBShellDocument.h"
#include <JFontManager.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBShellEditor::CBShellEditor
	(
	CBTextDocument*		document,
	const JCharacter*	fileName,
	JXMenuBar*			menuBar,
	CBTELineIndexInput*	lineInput,
	CBTEColIndexInput*	colInput,
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
	CBTextEditor(document, fileName, menuBar, lineInput, colInput,
				 scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsShellDoc    = (CBShellDocument*) document;
	itsInsertIndex = 1;

	SetDefaultFontStyle(JFontStyle(kJTrue, kJFalse, 0, kJFalse));

	itsInsertFont            = GetDefaultFont();
	itsInsertFont.style.bold = kJFalse;
	itsInsertFont.id         = (GetFontManager())->UpdateFontID(itsInsertFont.id, itsInsertFont.size, itsInsertFont.style);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBShellEditor::~CBShellEditor()
{
}

/******************************************************************************
 InsertText

 ******************************************************************************/

void
CBShellEditor::InsertText
	(
	const JString& text
	)
{
	JIndex index;
	const JBoolean hadCaret = GetCaretLocation(&index);

	SetCaretLocation(itsInsertIndex);
	SetCurrentFont(itsInsertFont);
	const JSize count = PasteUNIXTerminalOutput(text);
	SetCurrentFont(GetDefaultFont());
	ClearUndo();

	itsInsertIndex += count;
	if (hadCaret)
		{
		index += count;
		SetCaretLocation(index);
		}

	const JIndex i = itsInsertIndex-1;
	if (GetFontStyle(i).bold)
		{
		SetFontBold(i, i, kJFalse, kJTrue);
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBShellEditor::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean controlOn = modifiers.control();
	const JBoolean metaOn    = modifiers.meta();
	const JBoolean shiftOn   = modifiers.shift();
	if ((key == kJLeftArrow && metaOn && !controlOn && !shiftOn) ||
		(key == JXCtrl('A') && controlOn && !metaOn && !shiftOn))
		{
		const JIndex index            = GetInsertionIndex();
		const JRunArray<Font>& styles = GetStyles();
		if (index > 1 && styles.GetElement(index-1) == GetDefaultFont())
			{
			JIndex runIndex, firstIndexInRun;
			const JBoolean ok = styles.FindRun(index-1, &runIndex, &firstIndexInRun);
			SetCaretLocation(firstIndexInRun);
			return;
			}
		}

	if (key == kJReturnKey)
		{
		SetCurrentFont(itsInsertFont);
		}
	else
		{
		SetCurrentFont(GetDefaultFont());
		}

	JBoolean sentCmd = kJFalse;
	if (key == kJReturnKey && !modifiers.shift() && !HasSelection())
		{
		JIndex index;
		JBoolean ok = GetCaretLocation(&index);
		assert( ok );

		JString cmd;
		const JRunArray<Font>& styles = GetStyles();
		if (index > 1 && styles.GetElement(index-1) == GetDefaultFont())
			{
			JIndex runIndex, firstIndexInRun;
			ok = styles.FindRun(index-1, &runIndex, &firstIndexInRun);

			const JIndex endIndex = firstIndexInRun + styles.GetRunLength(runIndex);
			cmd = (GetText()).GetSubstring(firstIndexInRun, endIndex - 1);
			SetCaretLocation(endIndex);

			if (cmd.BeginsWith("man "))
				{
				cmd.ReplaceSubstring(1, 4, "jcc --man ");
				}
			else if (cmd.BeginsWith("apropos "))
				{
				cmd.ReplaceSubstring(1, 8, "jcc --apropos ");
				}
			else if (cmd.BeginsWith("vi "))
				{
				cmd.ReplaceSubstring(1, 3, "jcc ");
				}
			else if (cmd.BeginsWith("less ") || cmd.BeginsWith("more "))
				{
				cmd.ReplaceSubstring(1, 5, "jcc ");
				}
			else if (cmd == "more" || cmd == "less" || cmd == "vi")
				{
				cmd = "jcc";
				}
			}

		cmd += "\n";
		itsShellDoc->SendCommand(cmd);

		sentCmd = kJTrue;
		}

	CBTextEditor::HandleKeyPress(key, modifiers);

	if (sentCmd)
		{
		itsInsertIndex = GetInsertionIndex();
		}
}
