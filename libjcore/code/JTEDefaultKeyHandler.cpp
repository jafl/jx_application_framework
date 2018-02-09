/******************************************************************************
 JTEDefaultKeyHandler.cpp

	Default keybindings.

	BASE CLASS = JTEKeyHandler

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include <JTEDefaultKeyHandler.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEDefaultKeyHandler::JTEDefaultKeyHandler
	(
	JTextEditor* te
	)
	:
	JTEKeyHandler(te)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEDefaultKeyHandler::~JTEDefaultKeyHandler()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

JBoolean
JTEDefaultKeyHandler::HandleKeyPress
	(
	const JUtf8Character&			key,
	const JBoolean					selectText,
	const JTextEditor::CaretMotion	motion,
	const JBoolean					deleteToTabStop
	)
{
	JTextEditor* te             = GetTE();
	const JBoolean hasSelection = te->HasSelection();

	// We select text by selecting to where the caret ends up.

	const JBoolean isArrowKey = JI2B(
		key == kJLeftArrow || key == kJRightArrow ||
		key == kJUpArrow   || key == kJDownArrow);
	const JBoolean willSelectText = JI2B( selectText && isArrowKey );

	if (willSelectText)
		{
		JBoolean restoreCaretX        = kJTrue;
		const JCoordinate savedCaretX = te->itsCaretX;

		if (hasSelection && te->itsSelectionPivot.charIndex == te->itsCharSelection.last+1)
			{
			te->SetCaretLocation(JTextEditor::TextIndex(
				te->itsCharSelection.first, te->itsByteSelection.first));
			}
		else if (hasSelection && te->itsSelectionPivot.charIndex == te->itsCharSelection.first)
			{
			te->SetCaretLocation(te->AdjustTextIndex(
				JTextEditor::TextIndex(te->itsCharSelection.last, te->itsByteSelection.last),
				+1));
			}
		else if (hasSelection)	// SetSelection() was called by outsider
			{
			te->itsSelectionPivot.Set(te->itsCharSelection.first, te->itsByteSelection.first);
			restoreCaretX         = kJFalse;
			te->SetCaretLocation(te->AdjustTextIndex(
				JTextEditor::TextIndex(te->itsCharSelection.last, te->itsByteSelection.last),
				+1));
			}
		else
			{
			te->itsSelectionPivot = te->itsCaretLoc;
			}

		if (restoreCaretX && (key == kJUpArrow || key == kJDownArrow))
			{
			te->itsCaretX = savedCaretX;
			}
		}
	else if (te->GetType() == JTextEditor::kSelectableText && !isArrowKey)
		{
		return kJFalse;
		}

	JBoolean processed = kJTrue;

	// left arrow

	if (key == kJLeftArrow && motion == JTextEditor::kMoveByLine)
		{
		te->GoToBeginningOfLine();
		}

	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByWord && hasSelection)
		{
		te->SetCaretLocation(te->GetWordStart(				// works for zero
			JTextEditor::TextIndex(te->itsCharSelection.first, te->itsByteSelection.first)));
		}
	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByWord)
		{
		te->SetCaretLocation(te->GetWordStart(				// works for zero
			te->AdjustTextIndex(te->itsCaretLoc, -1)));
		}

	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByPartialWord && hasSelection)
		{
		te->SetCaretLocation(te->GetPartialWordStart(		// works for zero
			JTextEditor::TextIndex(te->itsCharSelection.first, te->itsByteSelection.first)));
		}
	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByPartialWord)
		{
		te->SetCaretLocation(te->GetPartialWordStart(		// works for zero
			te->AdjustTextIndex(te->itsCaretLoc, -1)));
		}

	else if (key == kJLeftArrow && hasSelection)
		{
		te->SetCaretLocation(
			JTextEditor::TextIndex(te->itsCharSelection.first, te->itsByteSelection.first));
		}
	else if (key == kJLeftArrow)
		{
		if (te->itsCaretLoc.charIndex > 1)
			{
			te->SetCaretLocation(te->AdjustTextIndex(te->itsCaretLoc, -1));
			}
		else
			{
			te->SetCaretLocation(JTextEditor::CaretLocation(1,1,1));	// scroll to it
			}
		}

	// right arrow

	else if (key == kJRightArrow && motion == JTextEditor::kMoveByLine)
		{
		te->GoToEndOfLine();
		}

	else if (key == kJRightArrow && motion == JTextEditor::kMoveByWord && hasSelection)
		{
		te->SetCaretLocation(te->AdjustTextIndex(
			te->GetWordEnd(JTextEditor::TextIndex(te->itsCharSelection.last, te->itsByteSelection.last)),
			+1));
		}
	else if (key == kJRightArrow && motion == JTextEditor::kMoveByWord)
		{
		te->SetCaretLocation(te->AdjustTextIndex(
			te->GetWordEnd(te->itsCaretLoc),
			+1));
		}

	else if (key == kJRightArrow && motion == JTextEditor::kMoveByPartialWord && hasSelection)
		{
		te->SetCaretLocation(te->AdjustTextIndex(
			te->GetPartialWordEnd(JTextEditor::TextIndex(te->itsCharSelection.last, te->itsByteSelection.last)),
			+1));
		}
	else if (key == kJRightArrow && motion == JTextEditor::kMoveByPartialWord)
		{
		te->SetCaretLocation(te->AdjustTextIndex(
			te->GetPartialWordEnd(te->itsCaretLoc),
			+1));
		}

	else if (key == kJRightArrow && hasSelection)
		{
		te->SetCaretLocation(te->AdjustTextIndex(
			JTextEditor::TextIndex(te->itsCharSelection.last, te->itsByteSelection.last),
			+1));
		}
	else if (key == kJRightArrow)
		{
		const JSize charCount = te->GetText().GetCharacterCount();
		if (te->itsCaretLoc.charIndex <= charCount)
			{
			te->SetCaretLocation(te->AdjustTextIndex(te->itsCaretLoc, +1));
			}
		else
			{
			te->SetCaretLocation(charCount+1);	// scroll to it
			}
		}

	// up arrow

	else if (key == kJUpArrow && motion == JTextEditor::kMoveByLine)
		{
		te->SetCaretLocation(JTextEditor::CaretLocation(1,1,1));
		}

	else if (key == kJUpArrow && motion == JTextEditor::kMoveByWord && hasSelection)
		{
		te->SetCaretLocation(te->GetParagraphStart(te->AdjustTextIndex(
			JTextEditor::TextIndex(te->itsCharSelection.first, te->itsByteSelection.first),
			-1)));
		}
	else if (key == kJUpArrow && motion == JTextEditor::kMoveByWord)
		{
		te->SetCaretLocation(te->GetParagraphStart(te->AdjustTextIndex(te->itsCaretLoc, -1)));
		}

	else if (key == kJUpArrow && hasSelection)
		{
		te->SetCaretLocation(JTextEditor::TextIndex(te->itsCharSelection.first, te->itsByteSelection.first));
		}
	else if (key == kJUpArrow && te->itsCaretLoc.charIndex == te->GetText().GetCharacterCount()+1 &&
			 te->EndsWithNewline())
		{
		te->SetCaretLocation(te->GetLineStart(te->itsCaretLoc.lineIndex));
		}
	else if (key == kJUpArrow && te->itsCaretLoc.lineIndex > 1)
		{
		te->MoveCaretVert(-1);
		}
	else if (key == kJUpArrow)
		{
		te->SetCaretLocation(JTextEditor::CaretLocation(1,1,1));
		}

	// down arrow

	else if (key == kJDownArrow && motion == JTextEditor::kMoveByLine)
		{
		te->SetCaretLocation(te->GetText().GetCharacterCount()+1);
		}

	else if (key == kJDownArrow && motion == JTextEditor::kMoveByWord && hasSelection)
		{
		te->SetCaretLocation(te->AdjustTextIndex(
			te->GetParagraphEnd(te->AdjustTextIndex(
				JTextEditor::TextIndex(te->itsCharSelection.last, te->itsByteSelection.last),
				+1)),
			+1));
		}
	else if (key == kJDownArrow && motion == JTextEditor::kMoveByWord)
		{
		te->SetCaretLocation(te->AdjustTextIndex(te->GetParagraphEnd(te->itsCaretLoc), +1));
		}

	else if (key == kJDownArrow && hasSelection)
		{
		te->SetCaretLocation(te->AdjustTextIndex(
			JTextEditor::TextIndex(te->itsCharSelection.last, te->itsByteSelection.last),
			+1));
		}
	else if (key == kJDownArrow && te->itsCaretLoc.lineIndex < te->GetLineCount())
		{
		te->MoveCaretVert(+1);
		}
	else if (key == kJDownArrow)
		{
		te->SetCaretLocation(te->GetText().GetCharacterCount()+1);
		}

	// delete

	else if (key == kJDeleteKey && hasSelection)
		{
		te->DeleteSelection();
		}
	else if (key == kJDeleteKey && te->itsCaretLoc.charIndex > 1)
		{
		te->BackwardDelete(deleteToTabStop);
		}

	// forward delete

	else if (key == kJForwardDeleteKey && hasSelection)
		{
		te->DeleteSelection();
		}
	else if (key == kJForwardDeleteKey && te->itsCaretLoc.charIndex <= te->GetText().GetCharacterCount())
		{
		te->ForwardDelete(deleteToTabStop);
		}

	// insert character

	else if (te->itsTabToSpacesFlag && key == '\t')
		{
		te->InsertSpacesForTab();
		}

	else if (key.IsPrint() || key == '\n' || key == '\t')
		{
		te->InsertKeyPress(key);
		}

	else
		{
		processed = kJFalse;
		}

	// finish selection process

	if (willSelectText)
		{
		const JTextEditor::CaretLocation savedCaretLoc = te->itsCaretLoc;
		if (te->itsCaretLoc.charIndex < te->itsSelectionPivot.charIndex)
			{
			const JTextEditor::TextIndex i = te->AdjustTextIndex(te->itsSelectionPivot, -1);
			te->SetSelection(
				JCharacterRange(te->itsCaretLoc.charIndex, i.charIndex),
				JUtf8ByteRange(te->itsCaretLoc.byteIndex, i.byteIndex),
				kJFalse);
			}
		else if (te->itsCaretLoc.charIndex > te->itsSelectionPivot.charIndex)
			{
			const JTextEditor::TextIndex i = te->AdjustTextIndex(te->itsCaretLoc, -1);
			te->SetSelection(
				JCharacterRange(te->itsSelectionPivot.charIndex, i.charIndex),
				JUtf8ByteRange(te->itsSelectionPivot.byteIndex, i.byteIndex),
				kJFalse);
			}

		te->itsPrevDragType = JTextEditor::kSelectDrag;

		// show moving end of selection

		te->BroadcastCaretMessages(savedCaretLoc, kJTrue);
		}

	return processed;
}
