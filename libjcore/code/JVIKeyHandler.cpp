/******************************************************************************
 JVIKeyHandler.cpp

	Base class to implement vi keybindings.

	BASE CLASS = JTEKeyHandler

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include <JVIKeyHandler.h>
#include <JRegex.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

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
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVIKeyHandler::~JVIKeyHandler()
{
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

		JIndex index;
		if (te->GetCaretLocation(&index) && index > 1 &&
			(te->GetText()).GetCharacter(index-1) != '\n')
			{
			te->SetCaretLocation(index-1);
			}

		*result = kJTrue;
		return kJTrue;
		}

	if (itsMode == kBufferNameMode)
		{
		itsMode = kCommandMode;		// don't use SetMode()
		itsKeyBuffer.AppendCharacter(key);
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
		ClearKeyBuffers();
		return kJFalse;
		}

	JTextEditor* te = GetTE();

	JBoolean clearKeyBuffer = kJTrue;
	JArray<JIndexRange> matchList;
	JCharacter prevChar;
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
		const JIndex i = te->GetInsertionIndex();
		if (te->IndexValid(i) && (te->GetText()).GetCharacter(i) != '\n')
			{
			te->SetCaretLocation(te->GetInsertionIndex()+1);
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
		const JIndex i = te->GetInsertionIndex();
		te->SetCaretLocation(i-1);
		InsertKeyPress('\n');
		if (i == 1)
			{
			te->SetCaretLocation(1);
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
		te->SetCaretLocation(te->GetTextLength()+1);
		}

	else if (isdigit(key))	// after 0 => beginning of line
		{
		if (!numberPattern.Match(itsKeyBuffer))
			{
			ClearKeyBuffers();
			}
		itsKeyBuffer.AppendCharacter(key);
		clearKeyBuffer = kJFalse;
		}
	else if (key == '"')
		{
		itsMode = kBufferNameMode;		// don't use SetMode()
		itsKeyBuffer.AppendCharacter(key);
		clearKeyBuffer = kJFalse;
		}
	else if (key == 'X' || key == 'x')
		{
		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		buf->Set("", kJFalse);

		const JSize count = GetOperationCount();
		JString s;
		for (JIndex i=1; i<=count; i++)
			{
			if (key == 'X')
				{
				if (te->GetInsertionIndex() == 1)
					{
					break;
					}
				BackwardDelete(deleteToTabStop, &s);
				}
			else
				{
				if (te->GetInsertionIndex() >= te->GetTextLength())
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
			 yankDeletePattern.Match(itsKeyBuffer, &matchList))
		{
		if (key == 'Y' ||
			(GetPrevCharacter(&prevChar) && prevChar == key))
			{
			YankLines(matchList, JI2B(key == 'd'));
			}
		else
			{
			itsKeyBuffer.AppendCharacter(key);
			clearKeyBuffer = kJFalse;
			}
		}

	else if (key == 'P' || key == 'p')
		{
		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		if (buf->buf != NULL)
			{
			const JIndex i = te->GetInsertionIndex();
			if (buf->line)
				{
				te->GoToBeginningOfLine();
				if (key == 'p')
					{
					MoveCaretVert(1);
					}
				}
			else if (key == 'p' &&
					 (te->IndexValid(i) && (te->GetText()).GetCharacter(i) != '\n'))
				{
				te->SetCaretLocation(te->GetInsertionIndex()+1);
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
		te->Undo();
		}

	else if (key == '$')	// after d$ and y$
		{
		te->GoToEndOfLine();
		const JIndex i = te->GetInsertionIndex();
		if (i > 1 && te->IndexValid(i) &&
			(te->GetText()).GetCharacter(i) == '\n')
			{
			te->SetCaretLocation(i-1);
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
	const JArray<JIndexRange>&	matchList,
	const JBoolean				del
	)
{
	JTextEditor* te = GetTE();
	te->GoToBeginningOfLine();
	const JIndex start = te->GetInsertionIndex();

	MoveCaretVert(GetOperationCount());

	JIndexRange r(start, te->GetInsertionIndex()-1);
	if (!r.IsEmpty())
		{
		CutBuffer* buf = GetCutBuffer(yankDeletePattern, matchList);
		buf->Set((te->GetText()).GetSubstring(r), kJTrue);

		if (del)
			{
			te->SetSelection(r);
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

	JIndexRange r;
	r.first           = te->GetInsertionIndex();
	const JIndex line = te->GetLineForChar(r.first) + GetOperationCount() - 1;
	r.last            = te->GetLineEnd(line);
	if (te->IndexValid(r.last) && te->GetText().GetCharacter(r.last) == '\n')
		{
		r.last--;
		}

	if (!r.IsEmpty())
		{
		CutBuffer* buf = GetCutBuffer(cutbufPattern);
		buf->Set(te->GetText().GetSubstring(r), kJFalse);

		if (del)
			{
			te->SetSelection(r);
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
	return (numberPrefixPattern.Match(itsKeyBuffer) ? atoi(itsKeyBuffer) : 1);
}

/******************************************************************************
 GetPrevCharacter (protected)

 ******************************************************************************/

JBoolean
JVIKeyHandler::GetPrevCharacter
	(
	JCharacter* c
	)
	const
{
	if (prevCharPattern.Match(itsKeyBuffer))
		{
		*c = itsKeyBuffer.GetLastCharacter();
		return kJTrue;
		}
	else
		{
		*c = 0;
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

	JArray<JIndexRange> matchList;
	if (r.Match(itsKeyBuffer, &matchList))
		{
		buf = GetCutBuffer(r, matchList);
		}

	return buf;
}

JVIKeyHandler::CutBuffer*
JVIKeyHandler::GetCutBuffer
	(
	const JRegex&				r,
	const JArray<JIndexRange>&	matchList
	)
	const
{
	JString s;
	if (r.GetSubexpression(itsKeyBuffer, "cutbuf", matchList, &s))
		{
		return theNamedCutBuffer + s.GetLastCharacter() - kNamedCutBufferOffset;
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
	if (buf == NULL)
	{
		buf = jnew JString;
		assert( buf != NULL );
	}

	*buf = s;
	line = l;
};
