/******************************************************************************
 CBCompileDocument.cpp

	BASE CLASS = CBExecOutputDocument

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "CBCompileDocument.h"
#include "CBTextEditor.h"
#include "CBCommandMenu.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStack.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JRegex dirMarkerPattern = "((Entering)|(Leaving)) directory `";

// static data

JBoolean CBCompileDocument::theDoubleSpaceFlag = kJTrue;

// Error menu

static const JUtf8Byte* kErrorMenuStr =
	"    First error             %k Ctrl-1"
	"%l| Previous error          %k Meta-minus"		// and Meta-_
	"  | Next error              %k Meta-plus"
	"%l| Open selection to error %k Meta-Shift-O";

enum
{
	kFirstErrorCmd = 1,
	kPrevErrorCmd, kNextErrorCmd,
	kOpenFileCmd
};

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBCompileDocument::CBCompileDocument
	(
	CBProjectDocument* projDoc
	)
	:
	CBExecOutputDocument(kCBExecOutputFT, "CBCompileHelp", kJFalse),
	itsProjDoc(projDoc),
	itsHasErrorsFlag(kJFalse)
{
	itsErrorMenu = InsertTextMenu(JGetString("ErrorMenuTitle::CBCompileDocument"));
	itsErrorMenu->SetMenuItems(kErrorMenuStr, "CBCompileDocument");
	itsErrorMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsErrorMenu);

	GetCommandMenu()->SetProjectDocument(projDoc);

	// allow Meta-_ to parallel Shift key required for Meta-plus

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, kJTrue);
	GetWindow()->InstallMenuShortcut(itsErrorMenu, kPrevErrorCmd, '_', modifiers);

	GetWindow()->SetWMClass(CBGetWMClassInstance(), CBGetCompileOutputWindowClass());

	Activate();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCompileDocument::~CBCompileDocument()
{
}

/******************************************************************************
 NeedsFormattedData (virtual protected)

 ******************************************************************************/

JBoolean
CBCompileDocument::NeedsFormattedData()
	const
{
	return kJTrue;
}

/******************************************************************************
 SetConnection (virtual)

 ******************************************************************************/

void
CBCompileDocument::SetConnection
	(
	JProcess*		p,
	const int		inFD,
	const int		outFD,
	const JString&	windowTitle,
	const JString&	dontCloseMsg,
	const JString&	execDir,
	const JString&	execCmd,
	const JBoolean	showPID
	)
{
	CBGetDocumentManager()->SetActiveListDocument(this);

	itsPrevLine.Clear();

	if (ShouldClearWhenStart())
		{
		itsHasErrorsFlag = kJFalse;
		}
	if (!itsHasErrorsFlag)
		{
		itsErrorMenu->Deactivate();
		}

	CBExecOutputDocument::SetConnection(p, inFD, outFD, windowTitle,
										dontCloseMsg, execDir, execCmd, kJFalse);

	if (!execCmd.IsEmpty())
		{
		CBTextEditor* te = GetTextEditor();

		const JUtf8Byte* map[] =
			{
			"d", execDir.GetBytes()
			};
		te->Paste(JGetString("ChangeDirectory::CBCompileDocument", map, sizeof(map)));
		te->Paste(JString("\n\n", kJFalse));
		te->GetText()->ClearUndo();
		}
}

/******************************************************************************
 ProcessFinished (virtual protected)

	Jump to the first error.

 ******************************************************************************/

JBoolean
CBCompileDocument::ProcessFinished
	(
	const JProcess::Finished& info
	)
{
	if (!CBExecOutputDocument::ProcessFinished(info))
		{
		return kJFalse;
		}

	if (!GetTextEditor()->HasSelection())
		{
		ShowFirstError();
		}

	return kJTrue;
}

/******************************************************************************
 AppendText (virtual protected)

	Double space all the output and display each error message on a single line.

	The dots at the front of the regexes guarantee that the file name is
	not empty.

	gcc, g++, g77:  $:#:
	cc, CC, flex:   "$", line #:
	bison:          ("$", line #) error:
	make:           make[#]: *** ... (ignored)
	Absoft F77:      error on line # of $:
	javac:           [javac] $:#:
	maven2:         $:[#,...]
	maven3:         $:#:#::

 ******************************************************************************/

static const JRegex gccErrorRegex    = "(?<=.):[0-9]+(:[0-9]+)?: (?:(?:fatal )?error|warning):";
static const JRegex flexErrorRegex   = "(?<=..)\", line [0-9]+: ";
static const JRegex bisonErrorRegex  = "(?<=...)\", line [0-9]+\\) error: ";
static const JRegex makeErrorRegex   = ".(\\[[0-9]+\\])?: \\*\\*\\*";
static const JRegex absoftErrorRegex = " error on line [0-9]+ of ([^:]+): ";
static const JRegex javacOutputRegex = "^\\s+\\[.+?\\]\\s+";
static const JRegex javacErrorRegex  = "^\\s+\\[.+?\\]\\s+(.+?):[0-9]+: ";
static const JRegex maven2ErrorRegex = "^(?:\\[.+?\\]\\s+)?(.+?):\\[[0-9]+,[0-9]+\\] ";
static const JRegex maven3ErrorRegex = "^(?:\\[.+?\\]\\s+)?(.+?):[0-9]+:[0-9]+::";

static const JUtf8Byte* makeIgnoreErrorStr = "(ignored)";
static const JUtf8Byte* gccMultilinePrefix = "   ";
const JSize kGCCMultilinePrefixLength      = strlen(gccMultilinePrefix);

void
cbSetErrorRangeFromMatchStart
	(
	const JStringMatch&		m,
	JStyledText::TextRange*	r
	)
{
	r->SetCount(
		JStyledText::TextCount(
			m.GetCharacterRange().first-1,
			m.GetUtf8ByteRange().first-1));
}

JStyledText::TextRange
cbComputeErrorRangeFromFirstSubmatch
	(
	const JStyledText::TextIndex	i,
	const JStringMatch&				m
	)
{
	JCharacterRange cr = m.GetCharacterRange(1);
	JUtf8ByteRange br  = m.GetUtf8ByteRange(1);

	cr += i.charIndex-1;
	br += i.byteIndex-1;

	return JStyledText::TextRange(cr, br);
}

void
CBCompileDocument::AppendText
	(
	const JString& text
	)
{
	const JBoolean isJavacError = javacOutputRegex.Match(text);

	const JStringMatch gccMatch         = gccErrorRegex.Match(text, kJFalse),
					   gccPrevLineMatch = gccErrorRegex.Match(itsPrevLine, kJFalse);

	const JBoolean isGCCError = JI2B(!isJavacError && !gccMatch.IsEmpty());

	const JStringMatch flexMatch = flexErrorRegex.Match(text, kJFalse);
	const JBoolean isFlexError   = !flexMatch.IsEmpty();

	const JStringMatch bisonMatch = bisonErrorRegex.Match(text, kJFalse);
	const JBoolean isBisonError   = !bisonMatch.IsEmpty();

	const JStringMatch makeMatch = makeErrorRegex.Match(text, kJFalse);
	const JBoolean isMakeError   = JI2B(
		!makeMatch.IsEmpty() && !text.EndsWith(makeIgnoreErrorStr) );

	const JStringMatch absoftMatch = absoftErrorRegex.Match(text, kJTrue);
	const JBoolean isAbsoftError   = !absoftMatch.IsEmpty();

	const JStringMatch maven2Match = maven2ErrorRegex.Match(text, kJTrue);
	const JBoolean isMaven2Error   = !maven2Match.IsEmpty();

	const JStringMatch maven3Match = maven3ErrorRegex.Match(text, kJTrue);
	const JBoolean isMaven3Error   = !maven3Match.IsEmpty();

	CBTextEditor* te = GetTextEditor();

	if (isGCCError &&
		gccPrevLineMatch.GetUtf8ByteRange() == gccMatch.GetUtf8ByteRange() &&
		JString::CompareMaxNBytes(itsPrevLine.GetBytes(), text.GetBytes(),
								  gccMatch.GetByteCount(), kJTrue) == 0)
		{
		JString s = text;
		JStringIterator iter(&s, kJIteratorStartAfterByte, gccMatch.GetUtf8ByteRange().last);
		iter.RemoveAllPrev();
		iter.Invalidate();
		s.Prepend(" /");

		// in front of 1 or 2 trailing newlines

		te->SetCaretLocation(te->GetText()->GetText().GetCharacterCount() - (theDoubleSpaceFlag ? 1 : 0));
		te->Paste(s);
		return;
		}
	else if (!isJavacError && !isGCCError && !gccPrevLineMatch.IsEmpty() &&
			 text.BeginsWith(gccMultilinePrefix) &&
			 text.GetByteCount() > kGCCMultilinePrefixLength)
		{
		JString s = text;
		JStringIterator iter(&s, kJIteratorStartAfterByte, kGCCMultilinePrefixLength);
		JUtf8Character c;
		if (iter.Next(&c, kJFalse) && !c.IsSpace())
			{
			iter.RemoveAllPrev();
			iter.Invalidate();

			te->SetCaretLocation(te->GetText()->GetText().GetCharacterCount() - (theDoubleSpaceFlag ? 1 : 0));
			te->Paste(s);
			return;
			}
		}

	const JStyledText::TextIndex startIndex = te->GetText()->GetBeyondEnd();

	CBExecOutputDocument::AppendText(text);
	if (theDoubleSpaceFlag)
		{
		te->Paste(JString("\n", kJFalse));
		}

	itsPrevLine = text;

	// display file name in bold and activate Errors menu

	JStyledText::TextRange boldRange(startIndex, JStyledText::TextCount());
	if (isJavacError)
		{
		const JStringMatch javacMatch = javacErrorRegex.Match(text, kJFalse);
		if (!javacMatch.IsEmpty())
			{
			boldRange = cbComputeErrorRangeFromFirstSubmatch(startIndex, javacMatch);
			}
		}
	else if (isGCCError)
		{
		cbSetErrorRangeFromMatchStart(gccMatch, &boldRange);
		}
	else if (isFlexError)
		{
		cbSetErrorRangeFromMatchStart(flexMatch, &boldRange);
		}
	else if (isBisonError)
		{
		cbSetErrorRangeFromMatchStart(bisonMatch, &boldRange);
		}
	else if (isMakeError)
		{
		boldRange.SetCount(
			JStyledText::TextCount(
				text.GetCharacterCount(),
				text.GetByteCount()));
		}
	else if (isAbsoftError)
		{
		boldRange = cbComputeErrorRangeFromFirstSubmatch(startIndex, absoftMatch);
		}
	else if (isMaven2Error)
		{
		boldRange = cbComputeErrorRangeFromFirstSubmatch(startIndex, maven2Match);
		}
	else if (isMaven3Error)
		{
		boldRange = cbComputeErrorRangeFromFirstSubmatch(startIndex, maven3Match);
		}

	if (!boldRange.IsEmpty())
		{
		te->GetText()->SetFontBold(boldRange, kJTrue, kJTrue);

		if (!itsHasErrorsFlag)
			{
			itsHasErrorsFlag = kJTrue;
			itsErrorMenu->Activate();

			JXWindow* window    = GetWindow();
			JString windowTitle = window->GetTitle();

			JStringIterator iter(&windowTitle);
			const JUtf8Character c('!');
			iter.SetNext(c);
			iter.SetNext(c);
			iter.SetNext(c);
			iter.Invalidate();

			window->SetTitle(windowTitle);
			}
		}
}

/******************************************************************************
 OpenPrevListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CBCompileDocument::OpenPrevListItem()
{
	CBTextEditor* te = GetTextEditor();
	JString s;
	while (ShowPrevError())
		{
		const JBoolean ok = te->GetSelection(&s);
		assert( ok );
		if (makeErrorRegex.Match(s))
			{
			continue;
			}

		te->OpenSelection();
		break;
		}
}

/******************************************************************************
 OpenNextListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CBCompileDocument::OpenNextListItem()
{
	CBTextEditor* te = GetTextEditor();
	JString s;
	while (ShowNextError())
		{
		const JBoolean ok = te->GetSelection(&s);
		assert( ok );
		if (makeErrorRegex.Match(s))
			{
			continue;
			}

		te->OpenSelection();
		break;
		}
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Find the preceding "Entering directory" or "Leaving directory" statement
	printed by make.

 ******************************************************************************/

void
CBCompileDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	CBGetDocumentManager()->SetActiveListDocument(const_cast<CBCompileDocument*>(this));

	if (JIsAbsolutePath(*fileName))
		{
		return;
		}

	CBTextEditor* te       = GetTextEditor();
	const JIndex caretChar = te->GetInsertionCharIndex();

	JStack<JIndex, JArray<JIndex> > dirStack;

	JStringIterator iter(te->GetText()->GetText());
	JIndex i;
	while (iter.Next(dirMarkerPattern) &&
		   iter.GetNextCharacterIndex(&i) && i < caretChar)
		{
		const JStringMatch& m = iter.GetLastMatch();
		if (!m.GetCharacterRange(2).IsEmpty())					// Entering
			{
			dirStack.Push(i);
			}
		else													// Leaving
			{
			assert( !m.GetCharacterRange(3).IsEmpty() );
			dirStack.Pop(&i);
			}
		}

	if (dirStack.Peek(&i))
		{
		iter.MoveTo(kJIteratorStartBefore, i);
		iter.BeginMatch();
		JUtf8Character c;
		if (iter.Next(&c) && c != '`' && iter.Next("`"))
			{
			const JStringMatch& m  = iter.FinishMatch();
			const JString testName = JCombinePathAndName(m.GetString(), *fileName);
			if (JFileExists(testName))
				{
				*fileName = testName;
				return;
				}
			}
		}

	iter.Invalidate();

	CBExecOutputDocument::ConvertSelectionToFullPath(fileName);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBCompileDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsErrorMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateErrorMenu();
		}
	else if (sender == itsErrorMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleErrorMenu(selection->GetIndex());
		}

	else
		{
		CBExecOutputDocument::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateErrorMenu (private)

 ******************************************************************************/

void
CBCompileDocument::UpdateErrorMenu()
{
	itsErrorMenu->SetItemEnable(kOpenFileCmd, GetTextEditor()->HasSelection());
}

/******************************************************************************
 HandleErrorMenu (private)

 ******************************************************************************/

void
CBCompileDocument::HandleErrorMenu
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveListDocument(this);

	if (index == kFirstErrorCmd)
		{
		ShowFirstError();
		}

	else if (index == kPrevErrorCmd)
		{
		ShowPrevError();
		}
	else if (index == kNextErrorCmd)
		{
		ShowNextError();
		}

	else if (index == kOpenFileCmd)
		{
		GetTextEditor()->OpenSelection();
		}
}

/******************************************************************************
 ShowFirstError (private)

 ******************************************************************************/

void
CBCompileDocument::ShowFirstError()
{
	if (itsHasErrorsFlag)
		{
		GetTextEditor()->SetCaretLocation(1);
		ShowNextError();
		}
}

/******************************************************************************
 ShowPrevError (private)

 ******************************************************************************/

JBoolean
jMatchErrorStyle
	(
	const JFont& font
	)
{
	return font.GetStyle().bold;
}

JBoolean
CBCompileDocument::ShowPrevError()
{
	CBTextEditor* te = GetTextEditor();
	te->Focus();

	JBoolean wrapped;
	if (te->JTextEditor::SearchBackward(jMatchErrorStyle, kJFalse, &wrapped))
		{
		te->TEScrollToSelection(kJTrue);
		return kJTrue;
		}
	else
		{
		GetDisplay()->Beep();
		return kJFalse;
		}
}

/******************************************************************************
 ShowNextError (private)

 ******************************************************************************/

JBoolean
CBCompileDocument::ShowNextError()
{
	CBTextEditor* te = GetTextEditor();
	te->Focus();

	JBoolean wrapped;
	if (te->JTextEditor::SearchForward(jMatchErrorStyle, kJFalse, &wrapped))
		{
		te->TEScrollToSelection(kJTrue);
		return kJTrue;
		}
	else
		{
		GetDisplay()->Beep();
		return kJFalse;
		}
}
