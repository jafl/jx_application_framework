/******************************************************************************
 JTEDefaultKeyHandler.cpp

	Default keybindings.

	BASE CLASS = JTEKeyHandler

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JTEDefaultKeyHandler.h"
#include "jx-af/jcore/jASCIIConstants.h"
#include "jx-af/jcore/jAssert.h"

using TextIndex     = JStyledText::TextIndex;
using TextCount     = JStyledText::TextCount;
using TextRange     = JStyledText::TextRange;
using CaretLocation = JTextEditor::CaretLocation;

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEDefaultKeyHandler::JTEDefaultKeyHandler()
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

bool
JTEDefaultKeyHandler::HandleKeyPress
	(
	const JUtf8Character&			key,
	const bool						selectText,
	const JTextEditor::CaretMotion	motion,
	const bool						deleteToTabStop
	)
{
	JTextEditor* te   = GetTE();
	JStyledText* st   = te->GetText();
	bool hasSelection = te->HasSelection();

	// We select text by selecting to where the caret ends up.

	const bool isArrowKey = (key == kJLeftArrow || key == kJRightArrow ||
							 key == kJUpArrow   || key == kJDownArrow);
	const bool willSelectText = selectText && isArrowKey;

	if (willSelectText)
	{
		bool restoreCaretX            = true;
		const JCoordinate savedCaretX = te->itsCaretX;

		if (hasSelection && te->itsSelectionPivot.charIndex == te->itsSelection.charRange.last+1)
		{
			te->SetCaretLocation(te->itsSelection.GetFirst());
		}
		else if (hasSelection && te->itsSelectionPivot.charIndex == te->itsSelection.charRange.first)
		{
			te->SetCaretLocation(te->itsSelection.GetAfter());
		}
		else if (hasSelection)	// SetSelection() was called by outsider
		{
			te->itsSelectionPivot = te->itsSelection.GetFirst();
			restoreCaretX         = false;
			te->SetCaretLocation(te->itsSelection.GetAfter());
		}
		else
		{
			te->itsSelectionPivot = te->itsCaret.location;
		}

		if (restoreCaretX && (key == kJUpArrow || key == kJDownArrow))
		{
			te->itsCaretX = savedCaretX;
		}
	}
	else if (te->GetType() == JTextEditor::kSelectableText && !isArrowKey)
	{
		return false;
	}

	hasSelection = te->HasSelection();

	bool processed = true;

	// left arrow

	if (key == kJLeftArrow && motion == JTextEditor::kMoveByLine)
	{
		te->GoToBeginningOfLine();
	}

	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByWord)
	{
		te->SetCaretLocation(st->GetWordStart(
			hasSelection ? te->itsSelection.GetFirst() :
			st->AdjustTextIndex(te->itsCaret.location, -1)));
	}

	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByPartialWord)
	{
		te->SetCaretLocation(st->GetPartialWordStart(
			hasSelection ? te->itsSelection.GetFirst() :
			st->AdjustTextIndex(te->itsCaret.location, -1)));
	}

	else if (key == kJLeftArrow)
	{
		te->SetCaretLocation(
			hasSelection ? te->itsSelection.GetFirst() :
			st->AdjustTextIndex(te->itsCaret.location, -1));
	}

	// right arrow

	else if (key == kJRightArrow && motion == JTextEditor::kMoveByLine)
	{
		te->GoToEndOfLine();
	}

	else if (key == kJRightArrow && motion == JTextEditor::kMoveByWord)
	{
		te->SetCaretLocation(st->AdjustTextIndex(
			st->GetWordEnd(
				hasSelection ? te->itsSelection.GetLast(*st) : te->itsCaret.location),
			+1));
	}

	else if (key == kJRightArrow && motion == JTextEditor::kMoveByPartialWord)
	{
		te->SetCaretLocation(st->AdjustTextIndex(
			st->GetPartialWordEnd(
				hasSelection ? te->itsSelection.GetLast(*st) : te->itsCaret.location),
			+1));
	}

	else if (key == kJRightArrow && hasSelection)
	{
		te->SetCaretLocation(te->itsSelection.GetAfter());
	}
	else if (key == kJRightArrow)
	{
		te->SetCaretLocation(st->AdjustTextIndex(te->itsCaret.location, +1));
	}

	// up arrow

	else if (key == kJUpArrow && motion == JTextEditor::kMoveByLine)
	{
		te->SetCaretLocation(CaretLocation(TextIndex(1,1),1));
	}

	else if (key == kJUpArrow && motion == JTextEditor::kMoveByWord)
	{
		te->SetCaretLocation(st->GetParagraphStart(
			st->AdjustTextIndex(
				hasSelection ? te->itsSelection.GetFirst() : te->itsCaret.location,
				-1)));
	}

	else if (key == kJUpArrow && hasSelection)
	{
		te->SetCaretLocation(te->itsSelection.GetFirst());
	}
	else if (key == kJUpArrow &&
			 te->itsCaret.location.charIndex == st->GetText().GetCharacterCount()+1 &&
			 st->EndsWithNewline())
	{
		te->SetCaretLocation(te->GetLineStart(te->itsCaret.lineIndex));
	}
	else if (key == kJUpArrow && te->itsCaret.lineIndex > 1)
	{
		te->MoveCaretVert(-1);
	}
	else if (key == kJUpArrow)
	{
		te->SetCaretLocation(CaretLocation(TextIndex(1,1),1));
	}

	// down arrow

	else if (key == kJDownArrow && motion == JTextEditor::kMoveByLine)
	{
		te->SetCaretLocation(st->GetText().GetCharacterCount()+1);
	}

	else if (key == kJDownArrow && motion == JTextEditor::kMoveByWord)
	{
		te->SetCaretLocation(st->AdjustTextIndex(
			st->GetParagraphEnd(
				hasSelection ? te->itsSelection.GetAfter() : te->itsCaret.location),
			+1));
	}

	else if (key == kJDownArrow && hasSelection)
	{
		te->SetCaretLocation(te->itsSelection.GetAfter());
	}
	else if (key == kJDownArrow && te->itsCaret.lineIndex < te->GetLineCount())
	{
		te->MoveCaretVert(+1);
	}
	else if (key == kJDownArrow)
	{
		te->SetCaretLocation(st->GetText().GetCharacterCount()+1);
	}

	// delete

	else if (key == kJDeleteKey)
	{
		BackwardDelete(deleteToTabStop);
	}

	// forward delete

	else if (key == kJForwardDeleteKey)
	{
		ForwardDelete(deleteToTabStop);
	}

	// insert character

	else if (st->TabInsertsSpaces() && key == '\t')
	{
		const TextIndex i = te->GetInsertionIndex();
		st->InsertSpacesForTab(te->GetLineStart(te->GetLineForChar(i.charIndex)), i);
	}

	else if (key.IsPrint() || key == '\n' || key == '\t')
	{
		te->InsertCharacter(key);
	}

	else
	{
		processed = false;
	}

	// finish selection process

	if (willSelectText)
	{
		const CaretLocation savedCaretLoc = te->itsCaret;
		if (te->itsCaret.location.charIndex < te->itsSelectionPivot.charIndex)
		{
			te->SetSelection(
				TextRange(te->itsCaret.location, te->itsSelectionPivot),
				false);
		}
		else if (te->itsCaret.location.charIndex > te->itsSelectionPivot.charIndex)
		{
			te->SetSelection(
				TextRange(te->itsSelectionPivot, te->itsCaret.location),
				false);
		}

		te->itsPrevDragType = JTextEditor::kSelectDrag;

		// show moving end of selection

		te->BroadcastCaretMessages(savedCaretLoc);
	}

	return processed;
}
