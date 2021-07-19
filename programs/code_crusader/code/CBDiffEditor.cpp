/******************************************************************************
 CBDiffEditor.cpp

	BASE CLASS = CBTextEditor

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#include "CBDiffEditor.h"
#include "CBDiffDocument.h"
#include <JXDisplay.h>
#include <JStringIterator.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDiffEditor::CBDiffEditor
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
	CBTextEditor(document, fileName, menuBar, lineInput, colInput, false,
				 scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsDiffDoc = (CBDiffDocument*) document;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDiffEditor::~CBDiffEditor()
{
}

/******************************************************************************
 ReadDiff

 ******************************************************************************/

void
CBDiffEditor::ReadDiff
	(
	std::istream&		input,
	const JFontStyle&	removeStyle,
	const JFontStyle&	insertStyle
	)
{
	itsRemoveStyle = removeStyle;
	itsInsertStyle = insertStyle;

	JSize lineOffset = 0;
	while (true)
		{
		JIndexRange origRange, newRange;
		const JUtf8Byte cmd = ReadCmd(input, &origRange, &newRange);

		if (input.eof() || input.fail())
			{
			break;
			}

		origRange.first = CRLineIndexToVisualLineIndex(origRange.first) + lineOffset;
		origRange.last  = CRLineIndexToVisualLineIndex(origRange.last)  + lineOffset;

		IgnoreOrigText(input, cmd);

		JSize newLineCount;
		const JString newText = ReadNewText(input, cmd, &newLineCount);

		JStyledText* st = GetText();

		const JStyledText::TextIndex pasteIndex =
			GetText()->AdjustTextIndex(GetLineEnd(origRange.last), +1);

		if (cmd != 'a' && st->GetText().CharacterIndexValid(origRange.last))
			{
			st->SetFontStyle(JStyledText::TextRange(
								GetLineStart(origRange.first), pasteIndex),
							 itsRemoveStyle, true);
			}

		if (newLineCount > 0)
			{
			if (origRange.last+1 > GetLineCount() &&
				!st->EndsWithNewline())
				{
				SetCaretLocation(st->GetText().GetCharacterCount()+1);
				Paste(JString::newline);
				}

			SetCaretLocation(pasteIndex);
			Paste(newText);
			GetText()->SetFontStyle(
				JStyledText::TextRange(
					pasteIndex,
					GetText()->AdjustTextIndex(GetLineEnd(origRange.last + newLineCount), +1)),
				insertStyle, true);
			}

		lineOffset += newLineCount;
		}
}

/******************************************************************************
 ReadCmd (private)

	LaR
		Add the lines in range R of the second file after line L of the
		first file.  For example, `8a12,15' means append lines 12-15 of
		file 2 after line 8 of file 1.

	FcT
		Replace the lines in range F of the first file with lines in range
		T of the second file.  This is like a combined add and delete, but
		more compact.  For example, `5,7c8,10' means change lines 5-7 of
		file 1 to read as lines 8-10 of file 2.

	RdL
		Delete the lines in range R from the first file; line L is where
		they would have appeared in the second file had they not been
		deleted.  For example, `5,7d3' means delete lines 5-7 of file 1.

 ******************************************************************************/

JUtf8Byte
CBDiffEditor::ReadCmd
	(
	std::istream&	input,
	JIndexRange*	origRange,
	JIndexRange*	newRange
	)
	const
{
	*origRange          = ReadRange(input);
	const JUtf8Byte cmd = input.get();
	*newRange           = ReadRange(input);
	return cmd;
}

/******************************************************************************
 ReadRange (private)

	Format:  x[,y]

 ******************************************************************************/

JIndexRange
CBDiffEditor::ReadRange
	(
	std::istream& input
	)
	const
{
	JIndexRange r;
	input >> r.first;
	if (input.peek() == ',')
		{
		input.ignore();
		input >> r.last;
		}
	else
		{
		r.last = r.first;
		}
	return r;
}

/******************************************************************************
 IgnoreOrigText (private)

	< FROM-FILE-LINE
	< FROM-FILE-LINE...
	---

 ******************************************************************************/

void
CBDiffEditor::IgnoreOrigText
	(
	std::istream&	input,
	const JUtf8Byte	cmd
	)
	const
{
	if (cmd == 'a')
		{
		return;
		}

	input >> std::ws;
	while (input.peek() == '<' || input.peek() == '\\')
		{
		JIgnoreLine(input);
		}

	if (cmd == 'c' && input.peek() == '-')
		{
		JIgnoreLine(input);
		}
}

/******************************************************************************
 ReadNewText (private)

	> TO-FILE-LINE
	> TO-FILE-LINE...

 ******************************************************************************/

JString
CBDiffEditor::ReadNewText
	(
	std::istream&	input,
	const JUtf8Byte	cmd,
	JSize*			lineCount
	)
	const
{
	JString text;
	*lineCount = 0;

	if (cmd != 'd')
		{
		input >> std::ws;
		while (input.peek() == '>')
			{
			input.ignore(2);
			text += JReadUntil(input, '\n');
			text += "\n";
			(*lineCount)++;
			}

		if (input.peek() == '\\')
			{
			JIgnoreLine(input);

			JStringIterator iter(&text, kJIteratorStartAtEnd);
			iter.RemovePrev();
			}
		}

	return text;
}

/******************************************************************************
 ShowFirstDiff

 ******************************************************************************/

void
CBDiffEditor::ShowFirstDiff()
{
	SetCaretLocation(1);
	ShowNextDiff();
}

/******************************************************************************
 ShowPrevDiff

 ******************************************************************************/

void
CBDiffEditor::ShowPrevDiff()
{
	Focus();

	JStyledText::TextRange origRange;
	const JStyledText::TextIndex origIndex = GetInsertionIndex();
	const bool hadSelection            = GetSelection(&origRange);

	JCharacterRange removeRange, insertRange;
	bool wrapped;
	if (JTextEditor::SearchBackward([this] (const JFont& f)
		{
		return f.GetStyle() == this->itsRemoveStyle;
		},
		false, &wrapped))
		{
		const bool ok = GetSelection(&removeRange);
		assert( ok );
		}

	SetCaretLocation(origIndex);
	if (JTextEditor::SearchBackward([this] (const JFont& f)
		{
		return f.GetStyle() == this->itsInsertStyle;
		},
		false, &wrapped))
		{
		const bool ok = GetSelection(&insertRange);
		assert( ok );
		}

	SelectDiff(removeRange, insertRange, removeRange.first >= insertRange.first,
			   hadSelection, origIndex, origRange);
}

/******************************************************************************
 ShowNextDiff

 ******************************************************************************/

void
CBDiffEditor::ShowNextDiff()
{
	Focus();

	JStyledText::TextRange origRange;
	const JStyledText::TextIndex origIndex = GetInsertionIndex();
	const bool hadSelection            = GetSelection(&origRange);

	JCharacterRange removeRange, insertRange;
	bool wrapped;
	if (JTextEditor::SearchForward([this] (const JFont& f)
		{
		return f.GetStyle() == this->itsRemoveStyle;
		},
		false, &wrapped))
		{
		const bool ok = GetSelection(&removeRange);
		assert( ok );
		}

	SetCaretLocation(hadSelection ? origRange.GetAfter() : origIndex);
	if (JTextEditor::SearchForward([this] (const JFont& f)
		{
		return f.GetStyle() == this->itsInsertStyle;
		},
		false, &wrapped))
		{
		const bool ok = GetSelection(&insertRange);
		assert( ok );
		}

	SelectDiff(removeRange, insertRange, removeRange.first <= insertRange.first,
			   hadSelection, origIndex, origRange);
}

/******************************************************************************
 SelectDiff (private)

 ******************************************************************************/

void
CBDiffEditor::SelectDiff
	(
	const JCharacterRange&			removeRange,
	const JCharacterRange&			insertRange,
	const bool					preferRemove,
	const bool					hadSelection,
	const JStyledText::TextIndex&	origIndex,
	const JStyledText::TextRange&	origRange
	)
{
	const bool foundRemove = !removeRange.IsNothing();
	const bool foundInsert = !insertRange.IsNothing();
	if (foundRemove && foundInsert && preferRemove)
		{
		SetSelection(removeRange);
		}
	else if (foundRemove && foundInsert)
		{
		SetSelection(insertRange);
		}
	else if (foundRemove)
		{
		SetSelection(removeRange);
		}
	else if (foundInsert)
		{
		SetSelection(insertRange);
		}
	else if (hadSelection)
		{
		SetSelection(origRange);
		GetDisplay()->Beep();
		}
	else
		{
		SetCaretLocation(origIndex);
		}

	if (HasSelection())
		{
		TEScrollToSelection(true);
		}
	else
		{
		GetDisplay()->Beep();
		}
}
