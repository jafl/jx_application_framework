/******************************************************************************
 JVIKeyHandler.cpp

	Implements vi keybindings.

	TODO
	dw   delete to end of word
	db   delete to beginning of word
	yw   yank to end of word
	yb   yank to beginning of word

	cc	 change line -- delete line and start insert mode
	s	 change character -- delete character and start insert mode

	md d'd  mark starting line, cut to ending line
	my y'y  mark starting line, copy to ending line

	BASE CLASS = JTEKeyHandler

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "JVIKeyHandler.h"
#include "JTEDefaultKeyHandler.h"
#include "JRegex.h"
#include "jASCIIConstants.h"
#include "jAssert.h"

static const JRegex numberPattern("^[0-9]+$");
static const JRegex numberPrefixPattern("^[0-9]+");
static const JRegex cutbufPattern("^[0-9]*(?<cutbuf>\"[a-zA-Z0-9])$");
static const JRegex prevCharPattern("^[0-9]*(?<cutbuf>\"[a-zA-Z0-9])?[^\"]+$");
static const JRegex yankDeletePattern("^[0-9]*(?<cutbuf>\"[a-zA-Z0-9])?[yd]?$");

JVIKeyHandler::CutBuffer JVIKeyHandler::theCutBuffer;
JVIKeyHandler::CutBuffer JVIKeyHandler::theNamedCutBuffer[ JVIKeyHandler::kNamedCutBufferCount ];

/******************************************************************************
 Constructor

 ******************************************************************************/

JVIKeyHandler::JVIKeyHandler()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVIKeyHandler::~JVIKeyHandler()
{
	jdelete itsDefKeyHandler;

	GetTE()->SetCaretMode(JTextEditor::kLineCaret);
}

/******************************************************************************
 Initialize (virtual)

 ******************************************************************************/

void
JVIKeyHandler::Initialize
	(
	JTextEditor* te
	)
{
	JTEKeyHandler::Initialize(te);

	itsDefKeyHandler = jnew JTEDefaultKeyHandler;
	itsDefKeyHandler->Initialize(te);

	SetMode(kCommandMode);
}

/******************************************************************************
 SetMode (protected)

 ******************************************************************************/

void
JVIKeyHandler::SetMode
	(
	const Mode mode
	)
{
	itsMode = mode;
	ClearKeyBuffers();

	JTextEditor* te = GetTE();
	if (mode == kTextEntryMode)
	{
		te->SetCaretMode(JTextEditor::kLineCaret);
	}
	else
	{
		te->SetCaretMode(JTextEditor::kBlockCaret);
	}
}

/******************************************************************************
 PrehandleKeyPress (protected)

 ******************************************************************************/

bool
JVIKeyHandler::PrehandleKeyPress
	(
	const JUtf8Character&	key,
	bool*				result
	)
{
	JTextEditor* te = GetTE();

	if (key == kJEscapeKey)
	{
		SetMode(kCommandMode);

		JTextEditor::CaretLocation caret;
		if (te->GetCaretLocation(&caret) && caret.location.charIndex > 1)
		{
			const JStyledText::TextIndex i = te->GetText()->AdjustTextIndex(caret.location, -1);
			if (te->GetCharacter(i) != '\n')
			{
				te->SetCaretLocation(JTextEditor::CaretLocation(i, caret.lineIndex));
			}
		}

		*result = true;
		return true;
	}

	if (itsMode == kBufferNameMode)
	{
		itsMode = kCommandMode;		// don't use SetMode()
		itsKeyBuffer.Append(key);
		*result = true;
		return true;
	}

	return false;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

bool
JVIKeyHandler::HandleKeyPress
	(
	const JUtf8Character&			key,
	const bool						selectText,
	const JTextEditor::CaretMotion	motion,
	const bool						deleteToTabStop
	)
{
	bool result;
	if (PrehandleKeyPress(key, &result))
	{
		return result;
	}

	if (itsMode == kTextEntryMode ||
		key == kJLeftArrow || key == kJRightArrow ||
		key == kJUpArrow || key == kJDownArrow)
	{
		ClearKeyBuffers();
		return itsDefKeyHandler->HandleKeyPress(key, selectText, motion, deleteToTabStop);
	}

	JTextEditor* te = GetTE();

	bool clearKeyBuffer = true;
	JArray<JIndexRange> matchList;
	JUtf8Character prevChar;
	if (key == 'i')			// insert at caret
	{
		SetMode(kTextEntryMode);
	}
	else if (key == 'I')	// insert at beginning of line
	{
		SetMode(kTextEntryMode);

		const bool save = te->WillMoveToFrontOfText();
		te->ShouldMoveToFrontOfText(true);
		te->GoToEndOfLine();
		te->GoToBeginningOfLine();
		te->ShouldMoveToFrontOfText(save);
	}
	else if (key == 'a')	// insert after caret
	{
		SetMode(kTextEntryMode);
		const JStyledText::TextIndex i = te->GetInsertionIndex();
		if (te->GetText()->GetText().CharacterIndexValid(i.charIndex) &&
			te->GetCharacter(i) != '\n')
		{
			te->SetCaretLocation(te->GetText()->AdjustTextIndex(i, +1));
		}
	}
	else if (key == 'A')	// insert at end of line
	{
		SetMode(kTextEntryMode);
		te->GoToEndOfLine();
	}
	else if (key == 'O')	// insert on new line above caret
	{
		SetMode(kTextEntryMode);

		te->GoToBeginningOfLine();
		const JStyledText::TextIndex i = te->GetInsertionIndex();
		te->SetCaretLocation(te->GetText()->AdjustTextIndex(i, -1));
		InsertKeyPress(JUtf8Character('\n'));
		if (i.charIndex == 1)
		{
			te->SetCaretLocation(JTextEditor::CaretLocation(JStyledText::TextIndex(1,1),1));
		}
	}
	else if (key == 'o')	// insert on new line below caret
	{
		SetMode(kTextEntryMode);

		te->GoToEndOfLine();
		InsertKeyPress(JUtf8Character('\n'));
	}

	else if ((key == '0' || key == '^') && itsKeyBuffer.IsEmpty())	// beginning of line
	{
		te->GoToBeginningOfLine();
	}
	else if (key == '\n')	// beginning of next line
	{
		MoveCaretVert(1);

		const bool save = te->WillMoveToFrontOfText();
		te->ShouldMoveToFrontOfText(true);
		te->GoToEndOfLine();
		te->GoToBeginningOfLine();
		te->ShouldMoveToFrontOfText(save);
	}
	else if (key == 'G')	// end of text
	{
		te->SetCaretLocation(te->GetText()->SelectAll().GetAfter());
	}

	else if (key == 'w')	// beginning of next word
	{
		JSize count = GetOperationCount();
		if (IsEndCharacterInWord())
		{
			count++;
		}

		for (JIndex i=1; i<=count; i++)
		{
			itsDefKeyHandler->HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByWord, false);
		}
		itsDefKeyHandler->HandleKeyPress(JUtf8Character(kJLeftArrow), false, JTextEditor::kMoveByWord, false);
	}

	else if (key == 'b')	// beginning of word
	{
		const JSize count = GetOperationCount();
		for (JIndex i=1; i<=count; i++)
		{
			itsDefKeyHandler->HandleKeyPress(JUtf8Character(kJLeftArrow), false, JTextEditor::kMoveByWord, false);
		}
	}
	else if (key == 'e')	// end of word
	{
		JSize count = GetOperationCount();
		if (IsEndCharacterInWord())
		{
			count++;
		}

		for (JIndex i=1; i<=count; i++)
		{
			itsDefKeyHandler->HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByWord, false);
		}
		itsDefKeyHandler->HandleKeyPress(JUtf8Character(kJLeftArrow), false, JTextEditor::kMoveByCharacter, false);
	}

	else if (key.IsDigit())				// operation count; must be after 0 => beginning of line
	{
		if (!numberPattern.Match(itsKeyBuffer))
		{
			ClearKeyBuffers();
		}
		itsKeyBuffer.Append(key);
		clearKeyBuffer = false;
	}
	else if (key == '"')				// buffer name
	{
		itsMode = kBufferNameMode;		// don't use SetMode()
		itsKeyBuffer.Append(key);
		clearKeyBuffer = false;
	}
	else if (key == 'X' || key == 'x')	// delete characters
	{
		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		buf->Set(JString::empty, false);

		const JSize count = GetOperationCount();
		JString s;
		for (JIndex i=1; i<=count; i++)
		{
			if (key == 'X')
			{
				if (te->GetInsertionIndex().charIndex == 1)
				{
					break;
				}
				BackwardDelete(deleteToTabStop, &s);
			}
			else if (te->GetInsertionIndex().charIndex > te->GetText()->GetText().GetCharacterCount())
			{
				BackwardDelete(false, &s);
			}
			else
			{
				ForwardDelete(deleteToTabStop, &s);
			}

			buf->buf->Append(s);
		}
	}
	else if (key == 'C' || key == 'D' ||	// delete/cut rest of line
			 (key == '$' && GetPrevCharacter(&prevChar) &&
			  (prevChar == 'd' || prevChar == 'y')))
	{
		const bool del = !GetPrevCharacter(&prevChar) || prevChar != 'y' || key != '$';
		YankToEndOfLine(del, key == 'C');
	}
	else if ((key == 'Y' || key == 'y' || key == 'd') &&	// copy/cut
			 yankDeletePattern.Match(itsKeyBuffer))
	{
		const JStringMatch match = yankDeletePattern.Match(itsKeyBuffer, JRegex::kIncludeSubmatches);
		if (key == 'Y' ||
			(GetPrevCharacter(&prevChar) && prevChar == key))
		{
			YankLines(match, key == 'd');
		}
		else
		{
			itsKeyBuffer.Append(key);
			clearKeyBuffer = false;
		}
	}

	else if (key == 'P' || key == 'p')		// paste
	{
		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		if (buf->buf != nullptr)
		{
			const JStyledText::TextIndex i = te->GetInsertionIndex();
			if (buf->line)
			{
				te->GoToBeginningOfLine();
				if (key == 'p')
				{
					MoveCaretVert(1);
				}
			}
			else if (key == 'p' &&
					 te->GetText()->GetText().CharacterIndexValid(i.charIndex) &&
					 te->GetCharacter(i) != '\n')
			{
				te->SetCaretLocation(te->GetText()->AdjustTextIndex(i, +1));
			}

			const JSize count = GetOperationCount();
			for (JIndex i=1; i<=count; i++)
			{
				te->Paste(*(buf->buf));
			}
		}
	}

	else if (key == 'u')	// undo
	{
		te->GetText()->Undo();
	}

	else if (key == '$')	// end of line; must be after d$ and y$
	{
		te->GoToEndOfLine();
		const JStyledText::TextIndex i = te->GetInsertionIndex();

		if (i.charIndex > 1 &&
			te->GetText()->GetText().CharacterIndexValid(i.charIndex) &&
			te->GetCharacter(i) == '\n')
		{
			te->SetCaretLocation(te->GetText()->AdjustTextIndex(i, -1));
		}
	}

	if (clearKeyBuffer)
	{
		ClearKeyBuffers();
	}
	return true;
}

/******************************************************************************
 IsEndCharacterInWord (protected)

 ******************************************************************************/

bool
JVIKeyHandler::IsEndCharacterInWord()
	const
{
	JTextEditor* te = GetTE();
	JStyledText::TextRange r;

	const JStyledText::TextIndex i =
		te->GetSelection(&r) ? r.GetLast(*te->GetText()) : te->GetInsertionIndex();

	return te->GetText()->IsCharacterInWord(te->GetCharacter(i));
}

/******************************************************************************
 YankLines (protected)

 ******************************************************************************/

void
JVIKeyHandler::YankLines
	(
	const JStringMatch&	match,
	const bool		del
	)
{
	JTextEditor* te = GetTE();
	te->GoToBeginningOfLine();
	const JStyledText::TextIndex start = te->GetInsertionIndex();

	MoveCaretVert(GetOperationCount());

	const JStyledText::TextIndex end = te->GetInsertionIndex();

	const JCharacterRange cr(start.charIndex, end.charIndex-1);
	const JUtf8ByteRange  br(start.byteIndex, end.byteIndex-1);
	if (!cr.IsEmpty())
	{
		const JString s(te->GetText()->GetText().GetRawBytes(), br, JString::kNoCopy);

		CutBuffer* buf = GetCutBuffer(yankDeletePattern, match);
		buf->Set(s, true);

		if (del)
		{
			te->SetSelection(JStyledText::TextRange(cr, br));
			te->DeleteSelection();
		}
	}
}

/******************************************************************************
 YankToEndOfLine (protected)

 ******************************************************************************/

void
JVIKeyHandler::YankToEndOfLine
	(
	const bool del,
	const bool ins
	)
{
	JTextEditor* te = GetTE();

	const JStyledText::TextIndex start = te->GetInsertionIndex();

	const JIndex line = te->GetLineForChar(start.charIndex) + GetOperationCount() - 1;

	JStyledText::TextIndex beyond = te->GetLineEnd(line);

	if (te->GetText()->GetText().CharacterIndexValid(beyond.charIndex) &&
		te->GetCharacter(beyond) != '\n')
	{
		beyond = te->GetText()->AdjustTextIndex(beyond, +1);
	}

	const JUtf8ByteRange br(start.byteIndex, beyond.byteIndex-1);
	if (!br.IsEmpty())
	{
		const JString s(te->GetText()->GetText().GetRawBytes(), br, JString::kNoCopy);

		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		buf->Set(s, false);

		if (del)
		{
			te->SetSelection(JStyledText::TextRange(JCharacterRange(start.charIndex, beyond.charIndex-1), br));
			te->DeleteSelection();
		}

		if (ins)
		{
			SetMode(kTextEntryMode);
		}
	}
}

/******************************************************************************
 GetOperationCount (protected)

 ******************************************************************************/

JSize
JVIKeyHandler::GetOperationCount()
	const
{
	return (numberPrefixPattern.Match(itsKeyBuffer) ? atoi(itsKeyBuffer.GetBytes()) : 1);
}

/******************************************************************************
 GetPrevCharacter (protected)

 ******************************************************************************/

bool
JVIKeyHandler::GetPrevCharacter
	(
	JUtf8Character* c
	)
	const
{
	if (prevCharPattern.Match(itsKeyBuffer))
	{
		c->Set(itsKeyBuffer.GetLastCharacter());
		return true;
	}
	else
	{
		c->Set('\0');
		return false;
	}
}

/******************************************************************************
 GetCutBuffer (protected)

 ******************************************************************************/

JVIKeyHandler::CutBuffer*
JVIKeyHandler::GetCutBuffer
	(
	const JRegex& r
	)
	const
{
	CutBuffer* buf = &theCutBuffer;

	const JStringMatch match = r.Match(itsKeyBuffer, JRegex::kIncludeSubmatches);
	if (!match.IsEmpty())
	{
		buf = GetCutBuffer(r, match);
	}

	return buf;
}

JVIKeyHandler::CutBuffer*
JVIKeyHandler::GetCutBuffer
	(
	const JRegex&		r,
	const JStringMatch&	match
	)
	const
{
	JString s = match.GetSubstring("cutbuf");
	if (!s.IsEmpty())
	{
		return theNamedCutBuffer + s.GetLastCharacter().GetBytes()[0] - kNamedCutBufferOffset;
	}
	else
	{
		return &theCutBuffer;
	}
}

/******************************************************************************
 CutBuffer::Set

 ******************************************************************************/

void
JVIKeyHandler::CutBuffer::Set
	(
	const JString& s,
	const bool l
	)
{
	if (buf == nullptr)
{
		buf = jnew JString;
}

	buf->Set(s);
	line = l;
};
