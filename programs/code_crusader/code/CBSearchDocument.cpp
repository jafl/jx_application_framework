/******************************************************************************
 CBSearchDocument.cpp

	BASE CLASS = CBExecOutputDocument

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBSearchDocument.h"
#include "CBTextEditor.h"
#include "CBSearchTE.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXProgressIndicator.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXColorManager.h>
#include <JThisProcess.h>
#include <JOutPipeStream.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jSysUtil.h>
#include <sstream>
#include <stdlib.h>
#include <jAssert.h>

const JCoordinate kIndicatorHeight = 10;

// Match menu

static const JCharacter* kMatchMenuTitleStr = "Matches";
static const JCharacter* kMatchMenuStr =
	"    First match             %k Ctrl-1"
	"%l| Previous match          %k Meta-minus"		// and Meta-_
	"  | Next match              %k Meta-plus"
	"%l| Open selection to match %k Meta-Shift-O";

enum
{
	kFirstMatchCmd = 1,
	kPrevMatchCmd, kNextMatchCmd,
	kOpenFileCmd
};

/******************************************************************************
 Constructor function (static)

	Search results.

 ******************************************************************************/

JError
CBSearchDocument::Create
	(
	const JPtrArray<JString>&	fileList,
	const JPtrArray<JString>&	nameList,
	const JCharacter*			searchStr,
	const JBoolean				onlyListFiles,
	const JBoolean				listFilesWithoutMatch
	)
{
	assert( !fileList.IsEmpty() );
	assert( fileList.GetElementCount() == nameList.GetElementCount() );

	int fd[2];
	JError err = JCreatePipe(fd);
	if (!err.OK())
		{
		return err;
		}

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	if (!err.OK())
		{
		return err;
		}

	// child

	else if (pid == 0)
		{
		close(fd[0]);

		// get rid of JXCreatePG, since we must not use X connection
		// (binary files may trigger it)
		JInitCore();

		CBSearchTE te;
		JOutPipeStream output(fd[1], kJTrue);
		te.SearchFiles(fileList, nameList,
					   onlyListFiles, listFilesWithoutMatch,
					   output);
		output.close();
		exit(0);
		}

	// parent

	else
		{
		close(fd[1]);

		JProcess* process = jnew JProcess(pid);
		assert( process != nullptr );

		const JCharacter* map[] =
			{
			"s", searchStr
			};
		const JString windowTitle = JGetString("SearchTitle::CBSearchDocument", map, sizeof(map));

		CBSearchDocument* doc =
			jnew CBSearchDocument(kJFalse, JI2B(onlyListFiles || listFilesWithoutMatch),
								 fileList.GetElementCount(),
								 process, fd[0], windowTitle);
		assert( doc != nullptr );
		doc->Activate();

		RecordLink* link;
		const JBoolean ok = doc->GetRecordLink(&link);
		assert( ok );
		CBSearchTE::SetProtocol(link);
		}

	return JNoError();
}

/******************************************************************************
 Constructor function (static)

	Replace All results.

 ******************************************************************************/

JError
CBSearchDocument::Create
	(
	const JPtrArray<JString>&	fileList,
	const JPtrArray<JString>&	nameList,
	const JCharacter*			searchStr,
	const JCharacter*			replaceStr
	)
{
	assert( !fileList.IsEmpty() );
	assert( fileList.GetElementCount() == nameList.GetElementCount() );

	int fd[2];
	JError err = JCreatePipe(fd);
	if (!err.OK())
		{
		return err;
		}

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	if (!err.OK())
		{
		return err;
		}

	// child

	else if (pid == 0)
		{
		close(fd[0]);

		// get rid of JXCreatePG, since we must not use X connection
		// (binary files may trigger it)
		JInitCore();

		CBSearchTE te;
		JOutPipeStream output(fd[1], kJTrue);
		te.SearchFiles(fileList, nameList, kJTrue, kJFalse, output);
		output.close();
		exit(0);
		}

	// parent

	else
		{
		close(fd[1]);

		JProcess* process = jnew JProcess(pid);
		assert( process != nullptr );

		const JCharacter* map[] =
			{
			"s", searchStr,
			"r", replaceStr
			};
		const JString windowTitle = JGetString("ReplaceTitle::CBSearchDocument", map, sizeof(map));

		CBSearchDocument* doc =
			jnew CBSearchDocument(kJTrue, kJTrue, fileList.GetElementCount(),
								 process, fd[0], windowTitle);
		assert( doc != nullptr );

		(JXGetApplication())->Suspend();	// do this first so result window is active
		doc->Activate();

		RecordLink* link;
		const JBoolean ok = doc->GetRecordLink(&link);
		assert( ok );
		CBSearchTE::SetProtocol(link);
		}

	return JNoError();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBSearchDocument::CBSearchDocument
	(
	const JBoolean		isReplace,
	const JBoolean		onlyListFiles,
	const JSize			fileCount,
	JProcess*			p,
	const int			fd,
	const JCharacter*	windowTitle
	)
	:
	CBExecOutputDocument(kCBSearchOutputFT, "CBSearchTextHelp-Multifile", kJFalse, kJFalse),
	itsIsReplaceFlag(isReplace),
	itsOnlyListFilesFlag(onlyListFiles),
	itsReplaceTE(nullptr)
{
	itsFoundFlag       = kJFalse;
	itsPrevQuoteOffset = 0;

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	GetFileDisplayInfo(&hSizing, &vSizing);

	itsIndicator =
		jnew JXProgressIndicator(GetWindow(), hSizing, vSizing,
								 -1000, -1000, 500, kIndicatorHeight);
	assert( itsIndicator != nullptr );
	itsIndicator->SetMaxValue(fileCount);

	JXMenuBar* menuBar = GetMenuBar();
	itsMatchMenu = InsertTextMenu(kMatchMenuTitleStr);
	itsMatchMenu->SetMenuItems(kMatchMenuStr, "CBSearchDocument");
	itsMatchMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsMatchMenu->Deactivate();
	ListenTo(itsMatchMenu);

	// allow Meta-_ to parallel Shift key required for Meta-plus

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, kJTrue);
	GetWindow()->InstallMenuShortcut(itsMatchMenu, kPrevMatchCmd, '_', modifiers);

	GetWindow()->SetWMClass(CBGetWMClassInstance(), CBGetSearchOutputWindowClass());

	SetConnection(p, fd, ACE_INVALID_HANDLE,
				  windowTitle, JGetString("NoCloseWhileSearching::CBSearchDocument"),
				  "/", windowTitle, kJFalse);

	(CBGetDocumentManager())->SetActiveListDocument(this);

	if (itsIsReplaceFlag)
		{
		itsReplaceTE = jnew CBSearchTE;
		assert( itsReplaceTE != nullptr );
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchDocument::~CBSearchDocument()
{
	jdelete itsReplaceTE;
}

/******************************************************************************
 PlaceCmdLineWidgets (virtual protected)

 ******************************************************************************/

void
CBSearchDocument::PlaceCmdLineWidgets()
{
	CBExecOutputDocument::PlaceCmdLineWidgets();

	JXWindow* window = GetWindow();

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	const JRect frame = GetFileDisplayInfo(&hSizing, &vSizing);

	itsIndicator->Place(frame.left, frame.ycenter() - kIndicatorHeight/2);
	itsIndicator->SetSize(frame.width(), kIndicatorHeight);
}

/******************************************************************************
 NeedsFormattedData (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchDocument::NeedsFormattedData()
	const
{
	return kJTrue;
}

/******************************************************************************
 ProcessFinished (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchDocument::ProcessFinished
	(
	const JProcess::Finished& info
	)
{
	if (!CBExecOutputDocument::ProcessFinished(info))
		{
		return kJFalse;
		}

	if (itsIsReplaceFlag)
		{
		jdelete itsReplaceTE;
		itsReplaceTE = nullptr;

		(JXGetApplication())->Resume();
		}

	jdelete itsIndicator;
	itsIndicator = nullptr;

	SetFileDisplayVisible(kJTrue);

	if (!itsFoundFlag)
		{
		CBExecOutputDocument::AppendText("\n\nNothing matched");
		DataReverted();
		GetTextEditor()->ClearUndo();
		}
	else if (!GetTextEditor()->HasSelection())
		{
		ShowFirstMatch();
		}

	return kJTrue;
}

/******************************************************************************
 AppendText (virtual protected)

 ******************************************************************************/

void
CBSearchDocument::AppendText
	(
	const JString& text
	)
{
	if (text.GetFirstCharacter() == CBSearchTE::kIncrementProgress)
		{
		itsIndicator->IncrementValue();
		return;
		}

	CBTextEditor* te = GetTextEditor();

	itsFoundFlag = kJTrue;
	const std::string s(text.GetCString(), text.GetLength());
	std::istringstream input(s);

	if (text.GetFirstCharacter() == CBSearchTE::kError)
		{
		input.ignore();
		JString msg;
		JReadAll(input, &msg);

		const JIndex startIndex = te->GetTextLength() + 1;
		te->SetCaretLocation(startIndex);
		te->Paste(msg);
		te->Paste("\n");
		if (!itsOnlyListFilesFlag)
			{
			te->Paste("\n");
			}

		te->SetFontStyle(startIndex, startIndex + msg.GetLength()-1,
						 GetErrorStyle(), kJTrue);
		}
	else if (itsOnlyListFilesFlag)
		{
		JString fileName;
		input >> fileName;
		CBExecOutputDocument::AppendText(fileName);

		if (itsIsReplaceFlag)
			{
			ReplaceAll(fileName);
			}
		}
	else
		{
		JCharacter mode;
		input.get(mode);

		const JIndex startIndex    = te->GetTextLength() + 1;
		const JFontStyle bold      = GetFileNameStyle();
		const JFontStyle underline = GetMatchStyle();

		if (mode == CBSearchTE::kNewMatchLine)
			{
			JString fileName;
			JIndex lineIndex;
			JString text;
			JIndexRange matchRange;
			input >> fileName >> lineIndex >> text >> matchRange;

			JString fileStr = fileName;
			fileStr.AppendCharacter(':');
			fileStr += JString((JUInt64) lineIndex);

			te->SetCaretLocation(startIndex);
			te->Paste(fileStr);
			te->Paste("\n\n");
			const JIndex textOffset = te->GetTextLength();
			te->Paste(text);
			te->Paste("\n\n");

			// display file name in bold

			te->SetFontStyle(startIndex, startIndex + fileName.GetLength()-1,
							 bold, kJTrue);

			// underline match

			matchRange += textOffset;
			te->SetFontStyle(matchRange.first, matchRange.last, underline, kJTrue);

			// save text range in case of multiple matches

			itsPrevQuoteOffset = textOffset;
			}
		else
			{
			assert( mode == CBSearchTE::kRepeatMatchLine &&
					itsPrevQuoteOffset > 0 );

			JIndexRange matchRange;
			input >> matchRange;

			// underline match

			matchRange += itsPrevQuoteOffset;
			te->SetFontStyle(matchRange.first, matchRange.last, underline, kJTrue);
			}

		itsMatchMenu->Activate();
		}
}

/******************************************************************************
 ReplaceAll (private)

 ******************************************************************************/

void
CBSearchDocument::ReplaceAll
	(
	const JCharacter* fileName
	)
{
	JTextEditor::PlainTextFormat format;

	JXFileDocument* doc;
	if ((CBGetDocumentManager())->FileDocumentIsOpen(fileName, &doc))
		{
		CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(doc);
		if (textDoc != nullptr)
			{
			(textDoc->GetWindow())->Update();

			CBTextEditor* te = textDoc->GetTextEditor();
			te->SetCaretLocation(1);
			te->ReplaceAllForward();
			}
		}
	else if (JFileReadable(fileName) &&
			 itsReplaceTE->ReadPlainText(fileName, &format, kJFalse))
		{
		itsReplaceTE->SetCaretLocation(1);
		if (itsReplaceTE->ReplaceAllForward())
			{
			itsReplaceTE->WritePlainText(fileName, format);
			}
		}
}

/******************************************************************************
 OpenPrevListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CBSearchDocument::OpenPrevListItem()
{
	if (ShowPrevMatch())
		{
		GetTextEditor()->OpenSelection();
		}
}

/******************************************************************************
 OpenNextListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CBSearchDocument::OpenNextListItem()
{
	if (ShowNextMatch())
		{
		GetTextEditor()->OpenSelection();
		}
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Find the preceding "Entering directory" or "Leaving directory" statement
	printed by make.

 ******************************************************************************/

void
CBSearchDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	(CBGetDocumentManager())->SetActiveListDocument(const_cast<CBSearchDocument*>(this));

	CBExecOutputDocument::ConvertSelectionToFullPath(fileName);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBSearchDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMatchMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMatchMenu();
		}
	else if (sender == itsMatchMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleMatchMenu(selection->GetIndex());
		}

	else
		{
		CBExecOutputDocument::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateMatchMenu (private)

 ******************************************************************************/

void
CBSearchDocument::UpdateMatchMenu()
{
	itsMatchMenu->SetItemEnable(kOpenFileCmd, GetTextEditor()->HasSelection());
}

/******************************************************************************
 HandleMatchMenu (private)

 ******************************************************************************/

void
CBSearchDocument::HandleMatchMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveListDocument(this);

	if (index == kFirstMatchCmd)
		{
		ShowFirstMatch();
		}

	else if (index == kPrevMatchCmd)
		{
		ShowPrevMatch();
		}
	else if (index == kNextMatchCmd)
		{
		ShowNextMatch();
		}

	else if (index == kOpenFileCmd)
		{
		GetTextEditor()->OpenSelection();
		}
}

/******************************************************************************
 ShowFirstMatch (private)

 ******************************************************************************/

void
CBSearchDocument::ShowFirstMatch()
{
	GetTextEditor()->SetCaretLocation(1);
	ShowNextMatch();
}

/******************************************************************************
 ShowPrevMatch (private)

 ******************************************************************************/

JBoolean
CBSearchDocument::ShowPrevMatch()
{
	CBTextEditor* te = GetTextEditor();
	te->Focus();

	JBoolean wrapped;
	if (te->JTextEditor::SearchBackward(MatchFileNameStyle(), kJFalse, &wrapped))
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
 ShowNextMatch (private)

 ******************************************************************************/

JBoolean
CBSearchDocument::ShowNextMatch()
{
	CBTextEditor* te = GetTextEditor();
	te->Focus();

	JBoolean wrapped;
	if (te->JTextEditor::SearchForward(MatchFileNameStyle(), kJFalse, &wrapped))
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
 GetFileNameStyle (private)

 ******************************************************************************/

JFontStyle
CBSearchDocument::GetFileNameStyle()
	const
{
	JFont font = GetTextEditor()->GetDefaultFont();
	font.SetBold(kJTrue);
	return font.GetStyle();
}

/******************************************************************************
 GetMatchStyle (private)

 ******************************************************************************/

JFontStyle
CBSearchDocument::GetMatchStyle()
	const
{
	JFont font = GetTextEditor()->GetDefaultFont();
	font.SetUnderlineCount(1);
	return font.GetStyle();
}

/******************************************************************************
 GetErrorStyle (private)

	Including bold allows next/prev will highlight it, but this can swamp
	out the useful matches.

 ******************************************************************************/

JFontStyle
CBSearchDocument::GetErrorStyle()
	const
{
	JFont font = GetTextEditor()->GetDefaultFont();
//	font.SetBold(!itsOnlyListFilesFlag);
	font.SetColor(GetColormap()->GetDarkRedColor());
	return font.GetStyle();
}

/******************************************************************************
 MatchFileNameStyle class (private)

 ******************************************************************************/

CBSearchDocument::MatchFileNameStyle::~MatchFileNameStyle()
{
}

JBoolean
CBSearchDocument::MatchFileNameStyle::Match
	(
	const JFont& font
	)
	const
{
	return font.GetStyle().bold;
}
