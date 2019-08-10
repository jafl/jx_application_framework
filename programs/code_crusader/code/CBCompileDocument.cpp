/******************************************************************************
 CBCompileDocument.cpp

	BASE CLASS = CBExecOutputDocument

	Copyright (C) 1997 by John Lindal.

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
#include <JString.h>
#include <JStack.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

static const JCharacter* kDirMarkerStr    = "Entering directory `";
static const JCharacter* kDirMarkerEndStr = "`";
static const JRegex dirMarkerPattern      = "((Entering)|(Leaving)) directory `";

// static data

JBoolean CBCompileDocument::theDoubleSpaceFlag = kJTrue;

// Error menu

static const JCharacter* kErrorMenuTitleStr = "Errors";
static const JCharacter* kErrorMenuStr =
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
	JXMenuBar* menuBar = GetMenuBar();
	itsErrorMenu = InsertTextMenu(kErrorMenuTitleStr);
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
	JProcess*			p,
	const int			inFD,
	const int			outFD,
	const JCharacter*	windowTitle,
	const JCharacter*	dontCloseMsg,
	const JCharacter*	execDir,
	const JCharacter*	execCmd,
	const JBoolean		showPID
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

	if (execCmd != nullptr)
		{
		CBTextEditor* te = GetTextEditor();
		te->Paste(kDirMarkerStr);
		te->Paste(execDir);
		te->Paste(kDirMarkerEndStr);
		te->Paste("\n\n");
		te->ClearUndo();
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

static const JRegex gccErrorRegex    = ".:[0-9]+(:[0-9]+)?: (?:(?:fatal )?error|warning):";
static const JRegex flexErrorRegex   = "..\", line [0-9]+: ";
static const JRegex bisonErrorRegex  = "...\", line [0-9]+\\) error: ";
static const JRegex makeErrorRegex   = ".(\\[[0-9]+\\])?: \\*\\*\\*";
static const JRegex absoftErrorRegex = " error on line [0-9]+ of ([^:]+): ";
static const JRegex javacOutputRegex = "^\\s+\\[.+?\\]\\s+";
static const JRegex javacErrorRegex  = "^\\s+\\[.+?\\]\\s+(.+?):[0-9]+: ";
static const JRegex maven2ErrorRegex = "^(?:\\[.+?\\]\\s+)?(.+?):\\[[0-9]+,[0-9]+\\] ";
static const JRegex maven3ErrorRegex = "^(?:\\[.+?\\]\\s+)?(.+?):[0-9]+:[0-9]+::";

static const JCharacter* makeIgnoreErrorStr = "(ignored)";
static const JCharacter* gccMultilinePrefix = "   ";
const JSize kGCCMultilinePrefixLength       = strlen(gccMultilinePrefix);

const JCharacter kMultibyteMarker = '\xE2';

void
CBCompileDocument::AppendText
	(
	const JString& origText
	)
{
	const JString* text = &origText;
	JBoolean deleteText = kJFalse;
	if (strchr(*text, kMultibyteMarker) != nullptr)
		{
		JString* s = jnew JString(origText);
		assert( s != nullptr );
		text       = s;
		deleteText = kJTrue;

		JSize length = s->GetLength();
		for (JIndex i=1; i<=length; i++)
			{
			if (s->GetCharacter(i) == kMultibyteMarker && i <= length-2)
				{
				const unsigned char c1 = s->GetCharacter(i+1);
				const unsigned char c2 = s->GetCharacter(i+2);
				const JIndex u = (((unsigned int) (unsigned char) c1) << 8) |
								 ((unsigned int) (unsigned char) c2);

				if (u == 32920 || u == 32921)
					{
					s->ReplaceSubstring(i, i+2, "'");
					}
				else
					{
					std::cout << "jcc: AppendText: unicode: " << u << std::endl;
					s->ReplaceSubstring(i, i+2, "\x80");
					}

				length -= 2;
				}
			}
		}

	const JBoolean isJavacError = javacOutputRegex.Match(*text);

	JIndexRange gccPrevLineRange, gccRange;
	const JBoolean isGCCError = JI2B(!isJavacError && gccErrorRegex.Match(*text, &gccRange));

	JIndexRange flexRange;
	const JBoolean isFlexError = flexErrorRegex.Match(*text, &flexRange);

	JIndexRange bisonRange;
	const JBoolean isBisonError = bisonErrorRegex.Match(*text, &bisonRange);

	JIndexRange makeRange;
	const JBoolean isMakeError = JI2B(
		makeErrorRegex.Match(*text, &makeRange) && !text->EndsWith(makeIgnoreErrorStr) );

	JArray<JIndexRange> absoftRangeList;
	const JBoolean isAbsoftError = absoftErrorRegex.Match(*text, &absoftRangeList);

	JArray<JIndexRange> maven2RangeList;
	const JBoolean isMaven2Error = maven2ErrorRegex.Match(*text, &maven2RangeList);

	JArray<JIndexRange> maven3RangeList;
	const JBoolean isMaven3Error = maven3ErrorRegex.Match(*text, &maven3RangeList);

	if (isGCCError &&
		gccErrorRegex.Match(itsPrevLine, &gccPrevLineRange) &&
		gccPrevLineRange == gccRange &&
		JCompareMaxN(itsPrevLine, *text, gccRange.last, kJTrue))
		{
		JString s = *text;
		s.RemoveSubstring(1, gccRange.last - 1);
		s.Prepend(" /");

		// in front of 1 or 2 trailing newlines

		CBTextEditor* te = GetTextEditor();
		te->SetCaretLocation(te->GetTextLength() - (theDoubleSpaceFlag ? 1 : 0));
		te->Paste(s);
		}
	else if (!isJavacError && !isGCCError &&
			 gccErrorRegex.Match(itsPrevLine, &gccPrevLineRange) &&
			 text->BeginsWith(gccMultilinePrefix) &&
			 text->GetLength() > kGCCMultilinePrefixLength &&
			 !isspace(text->GetCharacter(kGCCMultilinePrefixLength+1)))
		{
		JString s = *text;
		s.RemoveSubstring(1, strlen(gccMultilinePrefix));

		CBTextEditor* te = GetTextEditor();
		te->SetCaretLocation(te->GetTextLength() - (theDoubleSpaceFlag ? 1 : 0));
		te->Paste(s);
		}
	else
		{
		CBTextEditor* te        = GetTextEditor();
		const JIndex startIndex = te->GetTextLength() + 1;

		CBExecOutputDocument::AppendText(*text);
		if (theDoubleSpaceFlag)
			{
			te->Paste("\n");
			}

		itsPrevLine = *text;

		// display file name in bold and activate Errors menu

		JIndexRange boldRange;
		if (isJavacError)
			{
			JArray<JIndexRange> javacMatchList;
			if (javacErrorRegex.Match(*text, &javacMatchList))
				{
				const JIndexRange r = javacMatchList.GetElement(2);
				boldRange.Set(startIndex + r.first-1, startIndex + r.last-1);
				}
			}
		else if (isGCCError)
			{
			boldRange.Set(startIndex, startIndex + gccRange.first - 1);
			}
		else if (isFlexError)
			{
			boldRange.Set(startIndex+1, startIndex + flexRange.first);
			}
		else if (isBisonError)
			{
			boldRange.Set(startIndex+2, startIndex + bisonRange.first + 1);
			}
		else if (isMakeError)
			{
			boldRange.SetFirstAndLength(startIndex, text->GetLength());
			}
		else if (isAbsoftError)
			{
			boldRange  = absoftRangeList.GetElement(2);
			boldRange += startIndex-1;
			}
		else if (isMaven2Error)
			{
			boldRange  = maven2RangeList.GetElement(2);
			boldRange += startIndex-1;
			}
		else if (isMaven3Error)
			{
			boldRange  = maven3RangeList.GetElement(2);
			boldRange += startIndex-1;
			}

		if (!boldRange.IsEmpty())
			{
			te->JTextEditor::SetFont(boldRange.first, boldRange.last, GetErrorFont(), kJTrue);

			if (!itsHasErrorsFlag)
				{
				itsHasErrorsFlag = kJTrue;
				itsErrorMenu->Activate();

				JXWindow* window    = GetWindow();
				JString windowTitle = window->GetTitle();
				windowTitle.SetCharacter(1, '!');
				windowTitle.SetCharacter(2, '!');
				windowTitle.SetCharacter(3, '!');
				window->SetTitle(windowTitle);
				}
			}
		}

	if (deleteText)
		{
		jdelete text;
		}
}

/******************************************************************************
 OpenPrevListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CBCompileDocument::OpenPrevListItem()
{
	CBTextEditor* te    = GetTextEditor();
	const JString& text = te->GetText();
	JString s;
	JIndexRange r;
	while (ShowPrevError())
		{
		const JBoolean ok = te->GetSelection(&r);
		assert( ok );
		s = text.GetSubstring(r);
		if (makeErrorRegex.Match(s))
			{
			continue;
			}

		GetTextEditor()->OpenSelection();
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
	CBTextEditor* te    = GetTextEditor();
	const JString& text = te->GetText();
	JString s;
	JIndexRange r;
	while (ShowNextError())
		{
		const JBoolean ok = te->GetSelection(&r);
		assert( ok );
		s = text.GetSubstring(r);
		if (makeErrorRegex.Match(s))
			{
			continue;
			}

		GetTextEditor()->OpenSelection();
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
	const JString& text    = te->GetText();
	const JIndex caretChar = te->GetInsertionIndex();

	JArray<JIndexRange> matchList;
	JStack<JIndex, JArray<JIndex> > dirStack;

	JIndex i=1;
	while (dirMarkerPattern.MatchFrom(text, i, &matchList))
		{
		i = (matchList.GetElement(1)).last + 1;
		if (i >= caretChar)
			{
			break;
			}

		if (!(matchList.GetElement(3)).IsEmpty())					// Entering
			{
			dirStack.Push(i);
			}
		else														// Leaving
			{
			assert( !(matchList.GetElement(4)).IsEmpty() );
			JIndex j;
			dirStack.Pop(&j);
			}
		}

	JIndex startChar;
	if (dirStack.Peek(&startChar))
		{
		JIndex endChar = startChar;
		if (text.GetCharacter(endChar) != '\'' &&
			text.LocateNextSubstring("\'", &endChar))
			{
			endChar--;
			JString testName = text.GetSubstring(startChar, endChar);
			testName         = JCombinePathAndName(testName, *fileName);
			if (JFileExists(testName))
				{
				*fileName = testName;
				return;
				}
			}
		}

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

/******************************************************************************
 GetErrorFont (private)

 ******************************************************************************/

JFont
CBCompileDocument::GetErrorFont()
	const
{
	JFont font = GetTextEditor()->GetDefaultFont();
	font.SetBold(kJTrue);
	return font;
}
