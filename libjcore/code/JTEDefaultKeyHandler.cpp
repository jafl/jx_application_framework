/******************************************************************************
 JTEDefaultKeyHandler.cpp

	Default keybindings.

	BASE CLASS = JTEKeyHandler

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include <JTEDefaultKeyHandler.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

typedef JStyledText::TextIndex TextIndex;
typedef JStyledText::TextCount TextCount;
typedef JStyledText::TextRange TextRange;
typedef JTextEditor::CaretLocation CaretLocation;

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
	JStyledText* st             = te->GetText();
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
			restoreCaretX         = kJFalse;
			te->SetCaretLocation(te->itsSelection.GetAfter());
			}
		else
			{
			te->itsSelectionPivot = te->itsCaretLoc.location;
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

	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByWord)
		{
		te->SetCaretLocation(st->GetWordStart(
			hasSelection ? te->itsSelection.GetFirst() :
			st->AdjustTextIndex(te->itsCaretLoc.location, -1)));
		}

	else if (key == kJLeftArrow && motion == JTextEditor::kMoveByPartialWord)
		{
		te->SetCaretLocation(st->GetPartialWordStart(
			hasSelection ? te->itsSelection.GetFirst() :
			st->AdjustTextIndex(te->itsCaretLoc.location, -1)));
		}

	else if (key == kJLeftArrow)
		{
		te->SetCaretLocation(
			hasSelection ? te->itsSelection.GetFirst() :
			st->AdjustTextIndex(te->itsCaretLoc.location, -1));
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
				hasSelection ? te->itsSelection.GetLast(*st) : te->itsCaretLoc.location),
			+1));
		}

	else if (key == kJRightArrow && motion == JTextEditor::kMoveByPartialWord)
		{
		te->SetCaretLocation(st->AdjustTextIndex(
			st->GetPartialWordEnd(
				hasSelection ? te->itsSelection.GetLast(*st) : te->itsCaretLoc.location),
			+1));
		}

	else if (key == kJRightArrow && hasSelection)
		{
		te->SetCaretLocation(te->itsSelection.GetAfter());
		}
	else if (key == kJRightArrow)
		{
		te->SetCaretLocation(st->AdjustTextIndex(te->itsCaretLoc.location, +1));
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
				hasSelection ? te->itsSelection.GetFirst() : te->itsCaretLoc.location,
				-1)));
		}

	else if (key == kJUpArrow && hasSelection)
		{
		te->SetCaretLocation(te->itsSelection.GetFirst());
		}
	else if (key == kJUpArrow &&
			 te->itsCaretLoc.location.charIndex == st->GetText().GetCharacterCount()+1 &&
			 st->EndsWithNewline())
		{
		te->SetCaretLocation(te->GetLineStart(te->itsCaretLoc.lineIndex));
		}
	else if (key == kJUpArrow && te->itsCaretLoc.lineIndex > 1)
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
				hasSelection ? te->itsSelection.GetAfter() : te->itsCaretLoc.location),
			+1));
		}

	else if (key == kJDownArrow && hasSelection)
		{
		te->SetCaretLocation(te->itsSelection.GetAfter());
		}
	else if (key == kJDownArrow && te->itsCaretLoc.lineIndex < te->GetLineCount())
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
		te->InsertKeyPress(key);
		}

	else
		{
		processed = kJFalse;
		}

	// finish selection process

	if (willSelectText)
		{
		const CaretLocation savedCaretLoc = te->itsCaretLoc;
		if (te->itsCaretLoc.location.charIndex < te->itsSelectionPivot.charIndex)
			{
			te->SetSelection(
				TextRange(te->itsCaretLoc.location, te->itsSelectionPivot),
				kJFalse);
			}
		else if (te->itsCaretLoc.location.charIndex > te->itsSelectionPivot.charIndex)
			{
			te->SetSelection(
				TextRange(te->itsSelectionPivot, te->itsCaretLoc.location),
				kJFalse);
			}

		te->itsPrevDragType = JTextEditor::kSelectDrag;

		// show moving end of selection

		te->BroadcastCaretMessages(savedCaretLoc, kJTrue);
		}

	return processed;
}
