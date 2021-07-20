/******************************************************************************
 CBDiffDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBDiffDocument.h"
#include "CBDiffEditor.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXScrollbarSet.h>
#include <JStringIterator.h>
#include <JProcess.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <fcntl.h>
#include <stdio.h>
#include <jAssert.h>

const JSize kMenuButtonWidth = 70;

static const JUtf8Byte* kGitCmdSeparator = "\1";

// Diff menu

static const JUtf8Byte* kDiffMenuStr =
	"    First difference    %k Ctrl-1"
	"%l| Previous difference %k Meta-minus"		// and Meta-_
	"  | Next difference     %k Meta-plus";

enum
{
	kFirstDiffCmd = 1,
	kPrevDiffCmd, kNextDiffCmd
};

// JError data

const JUtf8Byte* kDiffFailed = "DiffFailed::CBDiffDocument";

/******************************************************************************
 Constructor function (static)

	plain diff

 ******************************************************************************/

JError
CBDiffDocument::CreatePlain
	(
	const JString&		fullName,
	const JString&		cmd,
	const JFontStyle&	defStyle,
	const JString&		name1,
	const JFontStyle&	removeStyle,
	const JString&		name2,
	const JFontStyle&	insertStyle,
	const bool		silent,
	CBDiffDocument*		origDoc
	)
{
	JXGetApplication()->DisplayBusyCursor();

	// create temp file for diff to avoid pipe filling up

	JString tempFileName, errFileName;
	int tempFileFD, errFileFD;
	JError err = CreateOutputFiles(&tempFileName, &tempFileFD, &errFileName, &errFileFD);
	if (!err.OK())
		{
		return DiffFailed(err);
		}

	// start diff

	JProcess* p;
	err = JProcess::Create(&p, cmd,
						   kJIgnoreConnection, nullptr,
						   kJAttachToFD, &tempFileFD,
						   kJAttachToFD, &errFileFD);
	if (err.OK())
		{
		// let diff chug along while we load the contents of file 1

		CBDiffDocument* doc = origDoc;
		if (doc == nullptr)
			{
			doc = jnew CBDiffDocument(kPlainType, fullName, JString::empty, cmd, defStyle,
									 name1, removeStyle, name2, insertStyle);
			assert( doc != nullptr );
			}
		doc->Init(fullName);

		// wait for diff to finish

		int result;
		p->WaitUntilFinished();
		if (p->SuccessfulFinish() ||
			(p->GetReturnValue(&result) && result == 1))
			{
			// read the output of diff

			close(tempFileFD);

			JSize length;
			const JError err = JGetFileLength(tempFileName, &length);
			assert_ok( err );

			std::ifstream input(tempFileName.GetBytes());
			if (length > 0 && !isdigit(input.peek()))
				{
				if (origDoc == nullptr)
					{
					doc->Close();
					}
				JString msg;
				JReadAll(input, &msg);
				if (!silent)
					{
					JGetUserNotification()->DisplayMessage(msg);
					}
				}
			else if (length > 0)
				{
				doc->ReadDiff(input);
				input.close();

				doc->Activate();
				}
			else
				{
				if (origDoc == nullptr)
					{
					doc->Close();
					}
				if (!silent)
					{
					JGetUserNotification()->DisplayMessage(JGetString("NoDiff::CBDiffDocument"));
					}
				}
			}
		else
			{
			if (origDoc == nullptr)
				{
				doc->Close();
				}
			close(errFileFD);
			err = DiffFailed(errFileName, true);
			}

		jdelete p;
		}
	else
		{
		err = DiffFailed(err);
		}

	// clean up

	close(tempFileFD);
	remove(tempFileName.GetBytes());

	close(errFileFD);
	remove(errFileName.GetBytes());

	return err;
}

/******************************************************************************
 Constructor function (static)

	cvs diff

 ******************************************************************************/

JError
CBDiffDocument::CreateCVS
	(
	const JString&		fullName,
	const JString&		getCmd,
	const JString&		diffCmd,
	const JFontStyle&	defStyle,
	const JString&		name1,
	const JFontStyle&	removeStyle,
	const JString&		name2,
	const JFontStyle&	insertStyle,
	const bool		silent,
	CBDiffDocument*		origDoc
	)
{
	JXGetApplication()->DisplayBusyCursor();

	// get rev1 file contents

	JString diskPath, fileName;
	JSplitPathAndName(fullName, &diskPath, &fileName);

	// create temp file for diff to avoid pipe filling up

	JString tempFileName, errFileName;
	int tempFileFD, errFileFD;
	JError err = CreateOutputFiles(&tempFileName, &tempFileFD, &errFileName, &errFileFD);
	if (!err.OK())
		{
		return DiffFailed(err);
		}

	// start diff

	JProcess* p;
	err = JProcess::Create(&p, diskPath, diffCmd,
						   kJIgnoreConnection, nullptr,
						   kJAttachToFD, &tempFileFD,
						   kJAttachToFD, &errFileFD);
	if (err.OK())
		{
		// let diff chug along while we load the contents of file 1

		JString s = JCombinePathAndName(diskPath, JString("CVS", JString::kNoCopy));
		s         = JCombinePathAndName(s, JString("Repository", JString::kNoCopy));

		JString cvsPath;
		JReadFile(s, &cvsPath);
		cvsPath.TrimWhitespace();
		cvsPath = JCombinePathAndName(cvsPath, fileName);

		JString tempPath;
		JCreateTempDirectory(&tempPath);	// will contain CVS directory, too
		JStripTrailingDirSeparator(&tempPath);

		JString tempRoot, tempDir;
		JSplitPathAndName(tempPath, &tempRoot, &tempDir);

		// CVS 1.11.17 does not accept an absolute path for -d

		JString cmd1 = getCmd;	// cvs get
		cmd1        += " -d ";
		cmd1        += JPrepArgForExec(tempDir);
		cmd1        += " ";
		cmd1        += JPrepArgForExec(cvsPath);

		JProcess* p1;
		int errFD;
		err = JProcess::Create(&p1, tempRoot, cmd1,
							   kJIgnoreConnection, nullptr,
							   kJIgnoreConnection, nullptr,
							   kJCreatePipe, &errFD);
		if (err.OK())
			{
			JString errOutput;
			JReadAll(errFD, &errOutput);

			p1->WaitUntilFinished();
			if (!p1->SuccessfulFinish())
				{
				err = DiffFailed(errOutput, false);
				}
			}
		jdelete p1;

		const JString startFullName = JCombinePathAndName(tempPath, fileName);

		CBDiffDocument* doc = origDoc;
		if (doc == nullptr)
			{
			doc = jnew CBDiffDocument(kCVSType, fullName, getCmd, diffCmd, defStyle,
									 name1, removeStyle, name2, insertStyle);
			assert( doc != nullptr );
			}
		doc->Init(startFullName);

		JKillDirectory(tempPath);

		// wait for diff to finish

		int result;
		p->WaitUntilFinished();
		if (err.OK() &&		// could be set by checkout process
			(p->SuccessfulFinish() ||
			 (p->GetReturnValue(&result) && result == 1)))
			{
			// read the output of diff

			close(tempFileFD);

			JSize length;
			const JError err1 = JGetFileLength(tempFileName, &length);
			assert_ok( err1 );

			if (length == 0)
				{
				if (origDoc == nullptr)
					{
					doc->Close();
					}

				if ((JGetFileLength(errFileName, &length)).OK() && length > 0)
					{
					close(errFileFD);
					err = DiffFailed(errFileName, true);
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage(JGetString("NoDiff::CBDiffDocument"));
					}
				}
			else
				{
				// skip lines until starts with digit

				std::ifstream input1(tempFileName.GetBytes());

				JString header;
				while (!input1.eof() && !isdigit(input1.peek()))
					{
					header += JReadLine(input1);
					header += "\n";
					}

				JString text;
				JReadAll(input1, &text);
				input1.close();

				if (text.GetCharacterCount() > 0 && !text.GetFirstCharacter().IsDigit())
					{
					if (origDoc == nullptr)
						{
						doc->Close();
						}
					JString msg = header + text;
					if (!silent)
						{
						JGetUserNotification()->DisplayMessage(msg);
						}
					}
				else if (text.GetCharacterCount() > 0)
					{
					icharbufstream input2(text.GetBytes(), text.GetByteCount());
					doc->ReadDiff(input2);
					doc->Activate();
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage(JGetString("NoDiff::CBDiffDocument"));
					}
				}
			}
		else
			{
			if (origDoc == nullptr)
				{
				doc->Close();
				}
			close(errFileFD);
			if (err.OK())	// could be set by checkout process
				{
				err = DiffFailed(errFileName, true);
				}
			}

		jdelete p;
		}
	else
		{
		err = DiffFailed(err);
		}

	// clean up

	close(tempFileFD);
	remove(tempFileName.GetBytes());

	close(errFileFD);
	remove(errFileName.GetBytes());

	return err;
}

/******************************************************************************
 Constructor function (static)

	svn diff

 ******************************************************************************/

JError
CBDiffDocument::CreateSVN
	(
	const JString&		fullName,
	const JString&		getCmd,
	const JString&		diffCmd,
	const JFontStyle&	defStyle,
	const JString&		name1,
	const JFontStyle&	removeStyle,
	const JString&		name2,
	const JFontStyle&	insertStyle,
	const bool		silent,
	CBDiffDocument*		origDoc
	)
{
	JXGetApplication()->DisplayBusyCursor();

	// create temp file for diff to avoid pipe filling up

	JString tempFileName, errFileName;
	int tempFileFD, errFileFD;
	JError err = CreateOutputFiles(&tempFileName, &tempFileFD, &errFileName, &errFileFD);
	if (!err.OK())
		{
		return DiffFailed(err);
		}

	// start diff

	JProcess* p;
	err = JProcess::Create(&p, diffCmd,
						   kJIgnoreConnection, nullptr,
						   kJAttachToFD, &tempFileFD,
						   kJAttachToFD, &errFileFD);
	if (err.OK())
		{
		// let diff chug along while we load the contents of file 1

		JString startFullName;
		err = FillOutputFile(JString::empty, getCmd, &startFullName);

		CBDiffDocument* doc = origDoc;
		if (doc == nullptr)
			{
			doc = jnew CBDiffDocument(kSVNType, fullName, getCmd, diffCmd, defStyle,
									  name1, removeStyle, name2, insertStyle);
			assert( doc != nullptr );
			}
		if (!startFullName.IsEmpty())
			{
			doc->Init(startFullName);
			remove(startFullName.GetBytes());
			}

		// wait for diff to finish

		int result;
		p->WaitUntilFinished();
		if (err.OK() &&		// could be set by checkout process
			(p->SuccessfulFinish() ||
			 (p->GetReturnValue(&result) && result == 1)))
			{
			// read the output of diff

			close(tempFileFD);

			JSize length;
			const JError err1 = JGetFileLength(tempFileName, &length);
			assert_ok( err1 );

			if (length == 0)
				{
				if (origDoc == nullptr)
					{
					doc->Close();
					}

				if ((JGetFileLength(errFileName, &length)).OK() && length > 0)
					{
					close(errFileFD);
					err = DiffFailed(errFileName, true);
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage(JGetString("NoDiff::CBDiffDocument"));
					}
				}
			else
				{
				// skip lines until starts with digit

				std::ifstream input1(tempFileName.GetBytes());

				JString header;
				while (!input1.eof() && !isdigit(input1.peek()))
					{
					header += JReadLine(input1);
					header += "\n";
					}

				JString text;
				JReadAll(input1, &text);
				input1.close();

				if (text.GetCharacterCount() > 0 && !text.GetFirstCharacter().IsDigit())
					{
					if (origDoc == nullptr)
						{
						doc->Close();
						}
					JString msg = header + text;
					if (!silent)
						{
						JGetUserNotification()->DisplayMessage(msg);
						}
					}
				else if (text.GetCharacterCount() > 0)
					{
					icharbufstream input2(text.GetBytes(), text.GetByteCount());
					doc->ReadDiff(input2);
					doc->Activate();
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage(JGetString("NoDiff::CBDiffDocument"));
					}
				}
			}
		else
			{
			if (origDoc == nullptr)
				{
				doc->Close();
				}
			close(errFileFD);
			if (err.OK())	// could be set by checkout process
				{
				err = DiffFailed(errFileName, true);
				}
			}

		jdelete p;
		}
	else
		{
		err = DiffFailed(err);
		}

	// clean up

	close(tempFileFD);
	remove(tempFileName.GetBytes());

	close(errFileFD);
	remove(errFileName.GetBytes());

	return err;
}

/******************************************************************************
 Constructor function (static)

	git diff (via "git show" and "diff")

 ******************************************************************************/

JError
CBDiffDocument::CreateGit
	(
	const JString&		fullName,
	const JString&		get1Cmd,
	const JString&		get2Cmd,
	const JString&		diffCmd,
	const JFontStyle&	defStyle,
	const JString&		name1,
	const JFontStyle&	removeStyle,
	const JString&		name2,
	const JFontStyle&	insertStyle,
	const bool		silent,
	CBDiffDocument*		origDoc
	)
{
	JXGetApplication()->DisplayBusyCursor();

	// get revisions

	JString path, n;
	JSplitPathAndName(fullName, &path, &n);

	JString f1, f2;
	JError err = FillOutputFile(path, get1Cmd, &f1);
	if (!err.OK())
		{
		return err;
		}

	bool removeF2 = false;
	if (strncmp(get2Cmd.GetBytes(), "file:", 5) == 0)
		{
		f2.Set(get2Cmd.GetBytes() + 5);
		}
	else
		{
		err = FillOutputFile(path, get2Cmd, &f2);
		if (!err.OK())
			{
			return err;
			}
		removeF2 = true;
		}

	CBDiffDocument* doc = origDoc;
	if (doc == nullptr)
		{
		JString getCmd = get1Cmd;
		getCmd        += kGitCmdSeparator;
		getCmd        += get2Cmd;

		doc = jnew CBDiffDocument(kGitType, fullName, getCmd, diffCmd, defStyle,
								 name1, removeStyle, name2, insertStyle);
		assert( doc != nullptr );
		}

	JString cmd = diffCmd;
	cmd        += JPrepArgForExec(f1);
	cmd        += " ";
	cmd        += JPrepArgForExec(f2);

	err = CBDiffDocument::CreatePlain(f1, cmd, defStyle, name1, removeStyle,
									  name2, insertStyle, silent, doc);
	if ((!err.OK() && origDoc == nullptr) || !doc->IsActive())
		{
		doc->Close();
		}

	// clean up

	remove(f1.GetBytes());

	if (removeF2)
		{
		remove(f2.GetBytes());
		}

	return err;
}

/******************************************************************************
 DiffFailed

 ******************************************************************************/

CBDiffDocument::DiffFailed::DiffFailed
	(
	const JError& err
	)
	:
	JError(kDiffFailed)
{
	SetMessage(err.GetMessage());
}

CBDiffDocument::DiffFailed::DiffFailed
	(
	const JString&	s,
	const bool	isFileName
	)
	:
	JError(kDiffFailed)
{
	JString msg;
	if (isFileName)
		{
		JReadFile(s, &msg);
		}
	else
		{
		msg = s;
		}
	SetMessage(msg);
}

void
CBDiffDocument::DiffFailed::SetMessage
	(
	const JString& msg
	)
{
	const JUtf8Byte* map[] =
		{
		"err", msg.GetBytes()
		};
	JError::SetMessage(map, sizeof(map));
}

/******************************************************************************
 CreateOutputFiles (static private)

 ******************************************************************************/

JError
CBDiffDocument::CreateOutputFiles
	(
	JString*	tempFileName,
	int*		tempFileFD,
	JString*	errFileName,
	int*		errFileFD
	)
{
	JError err = JCreateTempFile(tempFileName);
	if (!err.OK())
		{
		return err;
		}
	*tempFileFD = open(tempFileName->GetBytes(), O_WRONLY | O_CREAT | O_TRUNC
					   #ifdef _J_UNIX
					   , S_IRUSR | S_IWUSR
					   #endif
					   );

	err = JCreateTempFile(errFileName);
	if (!err.OK())
		{
		close(*tempFileFD);
		remove(tempFileName->GetBytes());
		return err;
		}
	*errFileFD = open(errFileName->GetBytes(), O_WRONLY | O_CREAT | O_TRUNC
					  #ifdef _J_UNIX
					  , S_IRUSR | S_IWUSR
					  #endif
					  );

	return JNoError();
}

/******************************************************************************
 FillOutputFile (static private)

 ******************************************************************************/

JError
CBDiffDocument::FillOutputFile
	(
	const JString&	path,
	const JString&	cmd,
	JString*		resultFullName
	)
{
	JString tempFileName, errFileName;
	int tempFileFD, errFileFD;
	JError err = CreateOutputFiles(&tempFileName, &tempFileFD, &errFileName, &errFileFD);
	if (err.OK())
		{
		JProcess* p;

		if (!path.IsEmpty())
			{
			err = JProcess::Create(&p, path, cmd,
								   kJIgnoreConnection, nullptr,
								   kJAttachToFD, &tempFileFD,
								   kJAttachToFD, &errFileFD);
			}
		else
			{
			err = JProcess::Create(&p, cmd,
								   kJIgnoreConnection, nullptr,
								   kJAttachToFD, &tempFileFD,
								   kJAttachToFD, &errFileFD);
			}

		if (err.OK())
			{
			p->WaitUntilFinished();
			if (!p->SuccessfulFinish())
				{
				err = DiffFailed(errFileName, true);
				}
			}
		jdelete p;

		close(tempFileFD);
		close(errFileFD);
		remove(errFileName.GetBytes());
		}

	if (err.OK())
		{
		*resultFullName = tempFileName;
		}
	else
		{
		resultFullName->Clear();
		remove(tempFileName.GetBytes());
		}
	return err;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBDiffDocument::CBDiffDocument
	(
	const Type			type,
	const JString&		fullName,
	const JString&		getCmd,
	const JString&		diffCmd,
	const JFontStyle&	defStyle,
	const JString&		name1,
	const JFontStyle&	removeStyle,
	const JString&		name2,
	const JFontStyle&	insertStyle
	)
	:
	CBTextDocument(kCBDiffOutputFT, "CBDiffHelp", true, ConstructDiffEditor),
	itsType(type),
	itsFullName(fullName),
	itsGetCmd(getCmd),
	itsDiffCmd(diffCmd),
	itsName1(name1),
	itsName2(name2),
	itsDefaultStyle(defStyle),
	itsRemoveStyle(removeStyle),
	itsInsertStyle(insertStyle)
{
	itsDiffEditor = (CBDiffEditor*) GetTextEditor();

	// Diff menu

	JXMenuBar* menuBar = GetMenuBar();
	itsDiffMenu = menuBar->InsertTextMenu(menuBar->GetMenuCount(), JGetString("DiffMenuTitle::CBDiffDocument"));
	itsDiffMenu->SetMenuItems(kDiffMenuStr, "CBDiffDocument");
	itsDiffMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDiffMenu);

	// allow Meta-_ to parallel Shift key required for Meta-plus

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, true);
	GetWindow()->InstallMenuShortcut(itsDiffMenu, kPrevDiffCmd, '_', modifiers);

	// button in upper right

	JXWindow* window = GetWindow();
	const JRect rect = window->GetBounds();

	itsDiffButton =
		jnew JXTextButton(JGetString("RedoDiffLabel::CBDiffDocument"), window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 rect.right - kMenuButtonWidth,0,
						 kMenuButtonWidth, menuBar->GetFrameHeight());
	assert( itsDiffButton != nullptr );
	ListenTo(itsDiffButton);

	menuBar->AdjustSize(-kMenuButtonWidth, 0);
}

// static private

CBTextEditor*
CBDiffDocument::ConstructDiffEditor
	(
	CBTextDocument*		document,
	const JString&		fileName,
	JXMenuBar*			menuBar,
	CBTELineIndexInput*	lineInput,
	CBTEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet
	)
{
	auto* te =
		jnew CBDiffEditor(document, fileName, menuBar, lineInput, colInput,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( te != nullptr );

	return te;
}

/******************************************************************************
 Init (private)

	We do as much work as possible here so diff has time to finish.

 ******************************************************************************/

void
CBDiffDocument::Init
	(
	const JString& fullName
	)
{
	ReadFile(fullName, false);

	// set window title and claim that it is not on disk
	// (must be before reading text to avoid styling based on content)

	const JUtf8Byte* map[] =
		{
		"name1", itsName1.GetBytes(),
		"name2", itsName2.GetBytes()
		};
	const JString windowTitle = JGetString("WindowTitle::CBDiffDocument", map, sizeof(map));
	FileChanged(windowTitle, false);

	// word wrap messes up the line numbers

	itsDiffEditor->SetBreakCROnly(true);

	// start with default style

	if (!itsDiffEditor->GetText()->IsEmpty())
		{
		itsDiffEditor->GetText()->SetFontStyle(
			JStyledText::TextRange(
				JStyledText::TextIndex(1,1),
				itsDiffEditor->GetText()->GetBeyondEnd()),
			itsDefaultStyle, true);
		}
	itsDiffEditor->GetText()->SetDefaultFontStyle(itsDefaultStyle);

	DataReverted();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDiffDocument::~CBDiffDocument()
{
}

/******************************************************************************
 ReadDiff (private)

 ******************************************************************************/

void
CBDiffDocument::ReadDiff
	(
	std::istream& input
	)
{
	itsDiffEditor->ReadDiff(input, itsRemoveStyle, itsInsertStyle);

	DataReverted();
	UpdateFileType();		// reset word wrap

	JXFileDocument* doc;
	if (CBGetDocumentManager()->FileDocumentIsOpen(itsFullName, &doc))
		{
		auto* textDoc = dynamic_cast<CBTextDocument*>(doc);
		if (textDoc != nullptr)
			{
			itsDiffEditor->SetBreakCROnly(textDoc->GetTextEditor()->WillBreakCROnly());
			}
		}

	itsDiffEditor->ShowFirstDiff();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBDiffDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDiffMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateDiffMenu();
		}
	else if (sender == itsDiffMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDiffMenu(selection->GetIndex());
		}

	else if (sender == itsDiffButton && message.Is(JXButton::kPushed))
		{
		JError err = JNoError();
		if (itsType == kPlainType)
			{
			err = CreatePlain(itsFullName, itsDiffCmd, itsDefaultStyle,
							  itsName1, itsRemoveStyle, itsName2, itsInsertStyle,
							  false, this);
			}
		else if (itsType == kCVSType)
			{
			err = CreateCVS(itsFullName, itsGetCmd, itsDiffCmd, itsDefaultStyle, 
							itsName1, itsRemoveStyle, itsName2, itsInsertStyle,
							false, this);
			}
		else if (itsType == kSVNType)
			{
			err = CreateSVN(itsFullName, itsGetCmd, itsDiffCmd, itsDefaultStyle, 
							itsName1, itsRemoveStyle, itsName2, itsInsertStyle,
							false, this);
			}
		else
			{
			assert( itsType == kGitType );

			JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
			itsGetCmd.Split(kGitCmdSeparator, &s, 2);
			assert( s.GetElementCount() == 2 );

			err = CreateGit(itsFullName, *s.GetElement(1), *s.GetElement(2),
							itsDiffCmd, itsDefaultStyle, 
							itsName1, itsRemoveStyle, itsName2, itsInsertStyle,
							false, this);
			}
		err.ReportIfError();
		}

	else
		{
		CBTextDocument::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDiffMenu (private)

 ******************************************************************************/

void
CBDiffDocument::UpdateDiffMenu()
{
}

/******************************************************************************
 HandleDiffMenu (private)

 ******************************************************************************/

void
CBDiffDocument::HandleDiffMenu
	(
	const JIndex index
	)
{
	if (index == kFirstDiffCmd)
		{
		itsDiffEditor->ShowFirstDiff();
		}

	else if (index == kPrevDiffCmd)
		{
		itsDiffEditor->ShowPrevDiff();
		}
	else if (index == kNextDiffCmd)
		{
		itsDiffEditor->ShowNextDiff();
		}
}
