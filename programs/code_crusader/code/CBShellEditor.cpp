/******************************************************************************
 CBShellEditor.cpp

	BASE CLASS = CBTextEditor

	Copyright © 2009 by John Lindal.

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
	CBTextEditor(document, fileName, menuBar, lineInput, colInput, true,
				 scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsInsertIndex(1,1)
{
	itsShellDoc = (CBShellDocument*) document;

	GetText()->SetDefaultFontStyle(JFontStyle(true, false, 0, false));

	itsInsertFont = GetText()->GetDefaultFont();
	itsInsertFont.SetBold(false);
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
	const bool hadCaret = GetCaretLocation(&savedCaret);

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
		GetText()->SetFontBold(JStyledText::TextRange(i, itsInsertIndex), false, true);
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
	const bool controlOn = modifiers.control();
	const bool metaOn    = modifiers.meta();
	const bool shiftOn   = modifiers.shift();
	if ((c == kJLeftArrow && metaOn && !controlOn && !shiftOn) ||
		(c == JXCtrl('A') && controlOn && !metaOn && !shiftOn))
		{
		JRunArrayIterator<JFont> iter(
			GetText()->GetStyles(),
			kJIteratorStartBefore, GetInsertionIndex().charIndex);

		JFont f;
		if (iter.Prev(&f) && f == GetText()->GetDefaultFont())
			{
			SetCaretLocation(iter.GetRunStart());
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

	bool sentCmd = false;
	if (c == kJReturnKey && !modifiers.shift() && !HasSelection())
		{
		JIndex index;
		bool ok = GetCaretLocation(&index);
		assert( ok );

		JString cmd;

		JRunArrayIterator<JFont> fiter(GetText()->GetStyles(), kJIteratorStartBefore, index);
		JFont f;
		if (fiter.Prev(&f) && f == GetText()->GetDefaultFont())
			{
			const JIndex endIndex = fiter.GetRunEnd();

			JStringIterator siter(GetText()->GetText(), kJIteratorStartBefore, fiter.GetRunStart());
			siter.BeginMatch();
			siter.MoveTo(kJIteratorStartBefore, endIndex);
			cmd = siter.FinishMatch().GetString();
			SetCaretLocation(endIndex);
			siter.Invalidate();

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

		sentCmd = true;
		}

	CBTextEditor::HandleKeyPress(c, keySym, modifiers);

	if (sentCmd)
		{
		itsInsertIndex = GetInsertionIndex();
		}
}
