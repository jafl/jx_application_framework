/******************************************************************************
 JVIKeyHandler.cpp

	Base class to implement vi keybindings.

	BASE CLASS = JTEKeyHandler

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JVIKeyHandler.h>
#include <JRegex.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

static const JRegex numberPattern("^[0-9]+$");
static const JRegex deletePattern("^[0-9]*d?$");

/******************************************************************************
 Constructor

 ******************************************************************************/

JVIKeyHandler::JVIKeyHandler
	(
	JTextEditor* te
	)
	:
	JTEKeyHandler(te),
	itsMode(kCommandMode)
{
	te->SetCaretMode(JTextEditor::kBlockCaret);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVIKeyHandler::~JVIKeyHandler()
{
}

/******************************************************************************
 PrehandleKeyPress (protected)

 ******************************************************************************/

JBoolean
JVIKeyHandler::PrehandleKeyPress
	(
	const JCharacter	key,
	JBoolean*			result
	)
{
	JTextEditor* te = GetTE();

	if (key == kJEscapeKey)
		{
		itsMode = kCommandMode;
		te->SetCaretMode(JTextEditor::kBlockCaret);

		JIndex index;
		if (te->GetCaretLocation(&index) && index > 1 &&
			(te->GetText()).GetCharacter(index-1) != '\n')
			{
			te->SetCaretLocation(index-1);
			}

		ClearKeyBuffer();
		*result = kJTrue;
		return kJTrue;
		}

	if (itsMode == kTextEntryMode)
		{
		te->SetCaretMode(JTextEditor::kLineCaret);
		ClearKeyBuffer();
		*result = kJFalse;
		return kJTrue;
		}

	return kJFalse;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

JBoolean
JVIKeyHandler::HandleKeyPress
	(
	const JCharacter				key,
	const JBoolean					selectText,
	const JTextEditor::CaretMotion	motion,
	const JBoolean					deleteToTabStop
	)
{
	JBoolean result;
	if (PrehandleKeyPress(key, &result))
		{
		return result;
		}

	if (key == kJLeftArrow || key == kJRightArrow ||
		key == kJUpArrow || key == kJDownArrow)
		{
		ClearKeyBuffer();
		return kJFalse;
		}

	JTextEditor* te = GetTE();

	JBoolean clearKeyBuffer = kJTrue;
	if (key == 'i')
		{
		itsMode = kTextEntryMode;
		te->SetCaretMode(JTextEditor::kLineCaret);
		}
	else if (key == 'I')
		{
		itsMode = kTextEntryMode;
		te->SetCaretMode(JTextEditor::kLineCaret);

		const JBoolean save = te->WillMoveToFrontOfText();
		te->ShouldMoveToFrontOfText(kJTrue);
		te->GoToEndOfLine();
		te->GoToBeginningOfLine();
		te->ShouldMoveToFrontOfText(save);
		}
	else if (key == 'a')
		{
		itsMode = kTextEntryMode;
		te->SetCaretMode(JTextEditor::kLineCaret);
		if ((te->GetText()).GetCharacter(te->GetInsertionIndex()) != '\n')
			{
			te->SetCaretLocation(te->GetInsertionIndex()+1);
			}
		}
	else if (key == 'A')
		{
		itsMode = kTextEntryMode;
		te->SetCaretMode(JTextEditor::kLineCaret);
		te->GoToEndOfLine();
		}
	else if (key == 'O')
		{
		itsMode = kTextEntryMode;
		te->SetCaretMode(JTextEditor::kLineCaret);
		te->GoToBeginningOfLine();
		te->SetCaretLocation(te->GetInsertionIndex()-1);
		InsertKeyPress('\n');
		}
	else if (key == 'o')
		{
		itsMode = kTextEntryMode;
		te->SetCaretMode(JTextEditor::kLineCaret);
		te->GoToEndOfLine();
		InsertKeyPress('\n');
		}

	else if ((key == '0' || key == '^') && itsKeyBuffer.IsEmpty())
		{
		te->GoToBeginningOfLine();
		}
	else if (key == '$')
		{
		te->GoToEndOfLine();
		JIndex i = te->GetInsertionIndex();
		if (i > 1 && te->IndexValid(i) &&
			(te->GetText()).GetCharacter(i) == '\n')
			{
			te->SetCaretLocation(i-1);
			}
		}
	else if (key == '\n')
		{
		MoveCaretVert(1);

		const JBoolean save = te->WillMoveToFrontOfText();
		te->ShouldMoveToFrontOfText(kJTrue);
		te->GoToEndOfLine();
		te->GoToBeginningOfLine();
		te->ShouldMoveToFrontOfText(save);
		}
	else if (key == 'G')
		{
		te->SetCaretLocation(te->GetTextLength()+1);
		}

	else if (isdigit(key))	// after 0 => beginning of line
		{
		if (!numberPattern.Match(itsKeyBuffer))
			{
			ClearKeyBuffer();
			}
		itsKeyBuffer.AppendCharacter(key);
		clearKeyBuffer = kJFalse;
		}
	else if (key == 'X' || key == 'x')
		{
		const JSize count = numberPattern.Match(itsKeyBuffer) ? atoi(itsKeyBuffer) : 1;
		for (JIndex i=1; i<=count; i++)
			{
			if (key == 'X')
				{
				BackwardDelete(deleteToTabStop);
				}
			else
				{
				ForwardDelete(deleteToTabStop);
				}
			}
		}
	else if (key == 'd' && deletePattern.Match(itsKeyBuffer))
		{
		if (!itsKeyBuffer.IsEmpty() && itsKeyBuffer.GetLastCharacter() == 'd')
			{
			te->GoToBeginningOfLine();
			const JIndex start = te->GetInsertionIndex();

			MoveCaretVert(isdigit(itsKeyBuffer.GetFirstCharacter()) ? atoi(itsKeyBuffer) : 1);

			te->SetSelection(start, te->GetInsertionIndex()-1);
			te->DeleteSelection();
			}
		else if (itsKeyBuffer.IsEmpty() || isdigit(itsKeyBuffer.GetLastCharacter()))
			{
			itsKeyBuffer.AppendCharacter(key);
			clearKeyBuffer = kJFalse;
			}
		}

	else if (key == 'u')
		{
		te->Undo();
		}

	if (clearKeyBuffer)
		{
		ClearKeyBuffer();
		}
	return kJTrue;
}
