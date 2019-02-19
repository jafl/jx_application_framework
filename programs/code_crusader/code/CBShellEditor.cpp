/******************************************************************************
 CBShellEditor.cpp

	BASE CLASS = CBTextEditor

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "CBShellEditor.h"
#include "CBShellDocument.h"
#include <JStringIterator.h>
#include <JFontManager.h>
#include <jTextUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBShellEditor::CBShellEditor
	(
	CBTextDocument*		document,
	const JString&		fileName,
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
				 scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsInsertIndex(1,1)
{
	itsShellDoc = (CBShellDocument*) document;

	GetText()->SetDefaultFontStyle(JFontStyle(kJTrue, kJFalse, 0, kJFalse));

	itsInsertFont = GetText()->GetDefaultFont();
	itsInsertFont.SetBold(kJFalse);
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
	CaretLocation savedCaret;
	const JBoolean hadCaret = GetCaretLocation(&savedCaret);

	SetCaretLocation(itsInsertIndex);
	SetCurrentFont(itsInsertFont);
	const JStyledText::TextRange range =
		JPasteUNIXTerminalOutput(text, itsInsertIndex, GetText());
	SetCurrentFont(GetText()->GetDefaultFont());
	GetText()->ClearUndo();

	itsInsertIndex = range.GetAfter();
	if (hadCaret)
		{
		savedCaret.location.charIndex += range.charRange.GetCount();
		savedCaret.location.byteIndex += range.byteRange.GetCount();
		SetCaretLocation(savedCaret.location);
		}

	const JStyledText::TextIndex i = GetText()->AdjustTextIndex(itsInsertIndex, -1);
	if (GetText()->GetFont(i.charIndex).GetStyle().bold)
		{
		GetText()->SetFontBold(JStyledText::TextRange(i, itsInsertIndex), kJFalse, kJTrue);
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBShellEditor::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean controlOn = modifiers.control();
	const JBoolean metaOn    = modifiers.meta();
	const JBoolean shiftOn   = modifiers.shift();
	if ((c == kJLeftArrow && metaOn && !controlOn && !shiftOn) ||
		(c == JXCtrl('A') && controlOn && !metaOn && !shiftOn))
		{
		const JIndex index            = GetInsertionIndex();
		const JRunArray<JFont>& styles = GetText()->GetStyles();
		if (index > 1 && styles.GetElement(index-1) == GetDefaultFont())
			{
			JIndex runIndex, firstIndexInRun;
			const JBoolean ok = styles.FindRun(index-1, &runIndex, &firstIndexInRun);
			SetCaretLocation(firstIndexInRun);
			return;
			}
		}

	if (c == kJReturnKey)
		{
		SetCurrentFont(itsInsertFont);
		}
	else
		{
		SetCurrentFont(GetText()->GetDefaultFont());
		}

	JBoolean sentCmd = kJFalse;
	if (c == kJReturnKey && !modifiers.shift() && !HasSelection())
		{
		JIndex index;
		JBoolean ok = GetCaretLocation(&index);
		assert( ok );

		JString cmd;
		const JRunArray<JFont>& styles = GetText()->GetStyles();
		if (index > 1 && styles.GetElement(index-1) == GetText()->GetDefaultFont())
			{
			JIndex runIndex, firstIndexInRun;
			ok = styles.FindRun(index-1, &runIndex, &firstIndexInRun);

			const JIndex endIndex = firstIndexInRun + styles.GetRunLength(runIndex);
			cmd = GetText()->GetSubstring(firstIndexInRun, endIndex - 1);
			SetCaretLocation(endIndex);

			if (cmd.BeginsWith("man "))
				{
				JStringIterator iter(&cmd);
				iter.RemoveNext(3);
				cmd.Prepend("jcc --man");
				}
			else if (cmd.BeginsWith("apropos "))
				{
				JStringIterator iter(&cmd);
				iter.RemoveNext(7);
				cmd.Prepend("jcc --apropos");
				}
			else if (cmd.BeginsWith("vi "))
				{
				JStringIterator iter(&cmd);
				iter.RemoveNext(2);
				cmd.Prepend("jcc");
				}
			else if (cmd.BeginsWith("less ") || cmd.BeginsWith("more "))
				{
				JStringIterator iter(&cmd);
				iter.RemoveNext(4);
				cmd.Prepend("jcc");
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

	CBTextEditor::HandleKeyPress(c, keySym, modifiers);

	if (sentCmd)
		{
		itsInsertIndex = GetInsertionIndex();
		}
}
