/******************************************************************************
 JVIKeyHandler.cpp

	Implements vi keybindings.

	BASE CLASS = JTEKeyHandler

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "JVIKeyHandler.h"
#include "JTEDefaultKeyHandler.h"
#include "JRegex.h"
#include "JStringIterator.h"
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

JVIKeyHandler::JVIKeyHandler
	(
	JTextEditor* te
	)
	:
	JTEKeyHandler(te)
{
	itsDefKeyHandler = jnew JTEDefaultKeyHandler(te);
	assert( itsDefKeyHandler != nullptr );
	itsDefKeyHandler->Initialize();
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
 Initialize (virtual protected)

 ******************************************************************************/

void
JVIKeyHandler::Initialize()
{
	JTEKeyHandler::Initialize();
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

JBoolean
JVIKeyHandler::PrehandleKeyPress
	(
	const JUtf8Character&	key,
	JBoolean*				result
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

		*result = kJTrue;
		return kJTrue;
		}

	if (itsMode == kBufferNameMode)
		{
		itsMode = kCommandMode;		// don't use SetMode()
		itsKeyBuffer.Append(key);
		*result = kJTrue;
		return kJTrue;
		}

	if (itsMode == kTextEntryMode)
		{
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
	const JUtf8Character&			key,
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
		ClearKeyBuffers();
		return itsDefKeyHandler->HandleKeyPress(key, selectText, motion, deleteToTabStop);
		}

	JTextEditor* te = GetTE();

	JBoolean clearKeyBuffer = kJTrue;
	JArray<JIndexRange> matchList;
	JUtf8Character prevChar;
	if (key == 'i')
		{
		SetMode(kTextEntryMode);
		}
	else if (key == 'I')
		{
		SetMode(kTextEntryMode);

		const JBoolean save = te->WillMoveToFrontOfText();
		te->ShouldMoveToFrontOfText(kJTrue);
		te->GoToEndOfLine();
		te->GoToBeginningOfLine();
		te->ShouldMoveToFrontOfText(save);
		}
	else if (key == 'a')
		{
		SetMode(kTextEntryMode);
		const JStyledText::TextIndex i = te->GetInsertionIndex();
		if (te->GetText()->GetText().CharacterIndexValid(i.charIndex) &&
			te->GetCharacter(i) != '\n')
			{
			te->SetCaretLocation(te->GetText()->AdjustTextIndex(i, +1));
			}
		}
	else if (key == 'A')
		{
		SetMode(kTextEntryMode);
		te->GoToEndOfLine();
		}
	else if (key == 'O')
		{
		SetMode(kTextEntryMode);

		te->GoToBeginningOfLine();
		const JStyledText::TextIndex i = te->GetInsertionIndex();
		te->SetCaretLocation(te->GetText()->AdjustTextIndex(i, -1));
		InsertKeyPress('\n');
		if (i.charIndex == 1)
			{
			te->SetCaretLocation(JTextEditor::CaretLocation(JStyledText::TextIndex(1,1),1));
			}
		}
	else if (key == 'o')
		{
		SetMode(kTextEntryMode);

		te->GoToEndOfLine();
		InsertKeyPress('\n');
		}

	else if ((key == '0' || key == '^') && itsKeyBuffer.IsEmpty())
		{
		te->GoToBeginningOfLine();
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
		te->SetCaretLocation(te->GetText()->SelectAll().GetAfter());
		}

	else if (key.IsDigit())	// after 0 => beginning of line
		{
		if (!numberPattern.Match(itsKeyBuffer))
			{
			ClearKeyBuffers();
			}
		itsKeyBuffer.Append(key);
		clearKeyBuffer = kJFalse;
		}
	else if (key == '"')
		{
		itsMode = kBufferNameMode;		// don't use SetMode()
		itsKeyBuffer.Append(key);
		clearKeyBuffer = kJFalse;
		}
	else if (key == 'X' || key == 'x')
		{
		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		buf->Set(JString::empty, kJFalse);

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
			else
				{
				if (te->GetInsertionIndex().charIndex >= te->GetText()->GetText().GetCharacterCount())
					{
					break;
					}
				ForwardDelete(deleteToTabStop, &s);
				}

			buf->buf->Append(s);
			}
		}
	else if (key == 'C' || key == 'D' ||
			 (key == '$' && GetPrevCharacter(&prevChar) &&
			  (prevChar == 'd' || prevChar == 'y')))
		{
		const JBoolean del = JNegate(GetPrevCharacter(&prevChar) && prevChar == 'y' && key == '$');
		YankToEndOfLine(del, JI2B(key == 'C'));
		}
	else if ((key == 'Y' || key == 'y' || key == 'd') &&
			 yankDeletePattern.Match(itsKeyBuffer))
		{
		const JStringMatch match = yankDeletePattern.Match(itsKeyBuffer, kJTrue);
		if (key == 'Y' ||
			(GetPrevCharacter(&prevChar) && prevChar == key))
			{
			YankLines(match, JI2B(key == 'd'));
			}
		else
			{
			itsKeyBuffer.Append(key);
			clearKeyBuffer = kJFalse;
			}
		}

	else if (key == 'P' || key == 'p')
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

	else if (key == 'u')
		{
		te->GetText()->Undo();
		}

	else if (key == '$')	// after d$ and y$
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
	return kJTrue;
}

/******************************************************************************
 YankLines (protected)

 ******************************************************************************/

void
JVIKeyHandler::YankLines
	(
	const JStringMatch&	match,
	const JBoolean		del
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
		const JString s(te->GetText()->GetText().GetRawBytes(), br, kJFalse);

		CutBuffer* buf = GetCutBuffer(yankDeletePattern, match);
		buf->Set(s, kJTrue);

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
	const JBoolean del,
	const JBoolean ins
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
		const JString s(te->GetText()->GetText().GetRawBytes(), br, kJFalse);

		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		buf->Set(s, kJFalse);

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

JBoolean
JVIKeyHandler::GetPrevCharacter
	(
	JUtf8Character* c
	)
	const
{
	if (prevCharPattern.Match(itsKeyBuffer))
		{
		c->Set(itsKeyBuffer.GetLastCharacter());
		return kJTrue;
		}
	else
		{
		c->Set('\0');
		return kJFalse;
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

	const JStringMatch match = r.Match(itsKeyBuffer, kJTrue);
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
	const JBoolean l
	)
{
	if (buf == nullptr)
	{
		buf = jnew JString;
		assert( buf != nullptr );
	}

	buf->Set(s);
	line = l;
};
