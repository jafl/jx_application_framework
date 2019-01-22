/******************************************************************************
 CBDiffDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBDiffDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JProcess.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <strstream>
#include <fcntl.h>
#include <stdio.h>
#include <jAssert.h>

const JSize kMenuButtonWidth = 70;

const JCharacter* CBDiffDocument::theLocalSVNGetMarker = "LOCAL";
static const JCharacter* kGitCmdSeparator              = "\1";

// Diff menu

static const JCharacter* kDiffMenuTitleStr = "Differences";
static const JCharacter* kDiffMenuStr =
	"    First difference    %k Ctrl-1"
	"%l| Previous difference %k Meta-minus"		// and Meta-_
	"  | Next difference     %k Meta-plus";

enum
{
	kFirstDiffCmd = 1,
	kPrevDiffCmd, kNextDiffCmd
};

// JError data

const JCharacter* kDiffFailed = "DiffFailed::CBDiffDocument";

/******************************************************************************
 Constructor function (static)

	plain diff

 ******************************************************************************/

JError
CBDiffDocument::CreatePlain
	(
	const JCharacter*	fullName,
	const JCharacter*	cmd,
	const JFontStyle&	defStyle,
	const JCharacter*	name1,
	const JFontStyle&	removeStyle,
	const JCharacter*	name2,
	const JFontStyle&	insertStyle,
	const JBoolean		silent,
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
			doc = jnew CBDiffDocument(kPlainType, fullName, "", cmd, defStyle,
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

			std::ifstream input(tempFileName);
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
					JGetUserNotification()->DisplayMessage("The files are identical.");
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
			err = DiffFailed(errFileName, kJTrue);
			}

		jdelete p;
		}
	else
		{
		err = DiffFailed(err);
		}

	// clean up

	close(tempFileFD);
	remove(tempFileName);

	close(errFileFD);
	remove(errFileName);

	return err;
}

/******************************************************************************
 Constructor function (static)

	cvs diff

 ******************************************************************************/

JError
CBDiffDocument::CreateCVS
	(
	const JCharacter*	fullName,
	const JCharacter*	getCmd,
	const JCharacter*	diffCmd,
	const JFontStyle&	defStyle,
	const JCharacter*	name1,
	const JFontStyle&	removeStyle,
	const JCharacter*	name2,
	const JFontStyle&	insertStyle,
	const JBoolean		silent,
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

		JString s = JCombinePathAndName(diskPath, "CVS");
		s         = JCombinePathAndName(s, "Repository");

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
				err = DiffFailed(errOutput, kJFalse);
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
					err = DiffFailed(errFileName, kJTrue);
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage("The files are identical.");
					}
				}
			else
				{
				// skip lines until starts with digit

				std::ifstream input1(tempFileName);

				JString header;
				while (!input1.eof() && !isdigit(input1.peek()))
					{
					header += JReadLine(input1);
					header += "\n";
					}

				JString text;
				JReadAll(input1, &text);
				input1.close();

				if (text.GetLength() > 0 && !isdigit(text.GetFirstCharacter()))
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
				else if (text.GetLength() > 0)
					{
					std::istrstream input2(text, text.GetLength());
					doc->ReadDiff(input2);
					doc->Activate();
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage("The files are identical.");
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
				err = DiffFailed(errFileName, kJTrue);
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
	remove(tempFileName);

	close(errFileFD);
	remove(errFileName);

	return err;
}

/******************************************************************************
 Constructor function (static)

	svn diff

 ******************************************************************************/

JError
CBDiffDocument::CreateSVN
	(
	const JCharacter*	fullName,
	const JCharacter*	getCmd,
	const JCharacter*	diffCmd,
	const JFontStyle&	defStyle,
	const JCharacter*	name1,
	const JFontStyle&	removeStyle,
	const JCharacter*	name2,
	const JFontStyle&	insertStyle,
	const JBoolean		silent,
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
		JBoolean removeAfter = kJFalse;
		if (strcmp(getCmd, theLocalSVNGetMarker) == 0)
			{
			JString diskPath, fileName;
			JSplitPathAndName(fullName, &diskPath, &fileName);

			startFullName  = JCombinePathAndName(diskPath, ".svn");
			startFullName  = JCombinePathAndName(startFullName, "text-base");
			startFullName  = JCombinePathAndName(startFullName, fileName);
			startFullName += ".svn-base";
			}
		else
			{
			err         = FillOutputFile(nullptr, getCmd, &startFullName);
			removeAfter = kJTrue;
			}

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
			if (removeAfter)
				{
				remove(startFullName);
				}
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
					err = DiffFailed(errFileName, kJTrue);
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage("The files are identical.");
					}
				}
			else
				{
				// skip lines until starts with digit

				std::ifstream input1(tempFileName);

				JString header;
				while (!input1.eof() && !isdigit(input1.peek()))
					{
					header += JReadLine(input1);
					header += "\n";
					}

				JString text;
				JReadAll(input1, &text);
				input1.close();

				if (text.GetLength() > 0 && !isdigit(text.GetFirstCharacter()))
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
				else if (text.GetLength() > 0)
					{
					std::istrstream input2(text, text.GetLength());
					doc->ReadDiff(input2);
					doc->Activate();
					}
				else if (!silent)
					{
					JGetUserNotification()->DisplayMessage("The files are identical.");
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
				err = DiffFailed(errFileName, kJTrue);
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
	remove(tempFileName);

	close(errFileFD);
	remove(errFileName);

	return err;
}

/******************************************************************************
 Constructor function (static)

	git diff (via "git show" and "diff")

 ******************************************************************************/

JError
CBDiffDocument::CreateGit
	(
	const JCharacter*	fullName,
	const JCharacter*	get1Cmd,
	const JCharacter*	get2Cmd,
	const JCharacter*	diffCmd,
	const JFontStyle&	defStyle,
	const JCharacter*	name1,
	const JFontStyle&	removeStyle,
	const JCharacter*	name2,
	const JFontStyle&	insertStyle,
	const JBoolean		silent,
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

	JBoolean removeF2 = kJFalse;
	if (strncmp(get2Cmd, "file:", 5) == 0)
		{
		f2 = get2Cmd + 5;
		}
	else
		{
		err = FillOutputFile(path, get2Cmd, &f2);
		if (!err.OK())
			{
			return err;
			}
		removeF2 = kJTrue;
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
	if (!err.OK() && origDoc == nullptr)
		{
		doc->Close();
		}

	// clean up

	remove(f1);

	if (removeF2)
		{
		remove(f2);
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
	JError(kDiffFailed, "")
{
	SetMessage(err.GetMessage());
}

CBDiffDocument::DiffFailed::DiffFailed
	(
	const JCharacter*	s,
	const JBoolean		isFileName
	)
	:
	JError(kDiffFailed, "")
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
	const JCharacter* msg
	)
{
	JString s = msg;
	s.Prepend("Unable to difference the two files because:\n\n");
	JError::SetMessage(s, kJTrue);
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
	*tempFileFD = open(*tempFileName, O_WRONLY | O_CREAT | O_TRUNC
					   #ifdef _J_UNIX
					   , S_IRUSR | S_IWUSR
					   #endif
					   );

	err = JCreateTempFile(errFileName);
	if (!err.OK())
		{
		close(*tempFileFD);
		remove(*tempFileName);
		return err;
		}
	*errFileFD = open(*errFileName, O_WRONLY | O_CREAT | O_TRUNC
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
	const JCharacter*	path,
	const JCharacter*	cmd,
	JString*			resultFullName
	)
{
	JString tempFileName, errFileName;
	int tempFileFD, errFileFD;
	JError err = CreateOutputFiles(&tempFileName, &tempFileFD, &errFileName, &errFileFD);
	if (err.OK())
		{
		JProcess* p;

		if (!JString::IsEmpty(path))
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
				err = DiffFailed(errFileName, kJTrue);
				}
			}
		jdelete p;

		close(tempFileFD);
		close(errFileFD);
		remove(errFileName);
		}

	if (err.OK())
		{
		*resultFullName = tempFileName;
		}
	else
		{
		resultFullName->Clear();
		remove(tempFileName);
		}
	return err;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBDiffDocument::CBDiffDocument
	(
	const Type			type,
	const JCharacter*	fullName,
	const JCharacter*	getCmd,
	const JCharacter*	diffCmd,
	const JFontStyle&	defStyle,
	const JCharacter*	name1,
	const JFontStyle&	removeStyle,
	const JCharacter*	name2,
	const JFontStyle&	insertStyle
	)
	:
	CBTextDocument(kCBDiffOutputFT, "CBDiffHelp"),
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
	// Diff menu

	JXMenuBar* menuBar = GetMenuBar();
	itsDiffMenu = menuBar->InsertTextMenu(menuBar->GetMenuCount(), kDiffMenuTitleStr);
	itsDiffMenu->SetMenuItems(kDiffMenuStr, "CBDiffDocument");
	itsDiffMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDiffMenu);

	// allow Meta-_ to parallel Shift key required for Meta-plus

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, kJTrue);
	GetWindow()->InstallMenuShortcut(itsDiffMenu, kPrevDiffCmd, '_', modifiers);

	// button in upper right

	JXWindow* window = GetWindow();
	const JRect rect = window->GetBounds();

	itsDiffButton =
		jnew JXTextButton("Redo diff", window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 rect.right - kMenuButtonWidth,0,
						 kMenuButtonWidth, menuBar->GetFrameHeight());
	assert( itsDiffButton != nullptr );
	ListenTo(itsDiffButton);

	menuBar->AdjustSize(-kMenuButtonWidth, 0);
}

/******************************************************************************
 Init (private)

	We do as much work as possible here so diff has time to finish.

 ******************************************************************************/

void
CBDiffDocument::Init
	(
	const JCharacter* fullName
	)
{
	ReadFile(fullName, kJFalse);

	// set window title and claim that it is not on disk
	// (must be before reading text to avoid styling based on content)

	JString windowTitle = "Diff:  ";
	windowTitle += itsName1;
	windowTitle += " -> ";
	windowTitle += itsName2;
	FileChanged(windowTitle, kJFalse);

	// word wrap messes up the line numbers

	CBTextEditor* te = GetTextEditor();
	te->SetBreakCROnly(kJTrue);

	// start with default style

	if (!te->IsEmpty())
		{
		te->SetFontStyle(1, te->GetTextLength(), itsDefaultStyle, kJTrue);
		}
	te->SetDefaultFontStyle(itsDefaultStyle);
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
	CBTextEditor* te = GetTextEditor();

	JSize lineOffset = 0;
	while (1)
		{
		JIndexRange origRange, newRange;
		const JCharacter cmd = ReadCmd(input, &origRange, &newRange);

		if (input.eof() || input.fail())
			{
			break;
			}

		origRange.first = te->CRLineIndexToVisualLineIndex(origRange.first);
		origRange.last  = te->CRLineIndexToVisualLineIndex(origRange.last);
		newRange.first  = te->CRLineIndexToVisualLineIndex(newRange.first);
		newRange.last   = te->CRLineIndexToVisualLineIndex(newRange.last);

		IgnoreOrigText(input, cmd);

		JSize newLineCount;
		const JString newText = ReadNewText(input, cmd, &newLineCount);

		origRange += lineOffset;
		if (cmd != 'a' && te->IndexValid(origRange.last))
			{
			te->SetFontStyle(te->GetLineStart(origRange.first),
							 te->GetLineEnd(origRange.last),
							 itsRemoveStyle, kJTrue);
			}

		if (newLineCount > 0)
			{
			if (origRange.last+1 > te->GetLineCount() &&
				!te->EndsWithNewline())
				{
				te->SetCaretLocation(te->GetTextLength()+1);
				te->Paste("\n");
				}

			const JIndex pasteIndex = te->GetLineEnd(origRange.last) + 1;
			te->SetCaretLocation(pasteIndex);
			te->Paste(newText);
			te->SetFontStyle(pasteIndex,
							 te->GetLineEnd(origRange.last + newLineCount),
							 itsInsertStyle, kJTrue);
			}

		lineOffset += newLineCount;
		}

	DataReverted();
	UpdateFileType();		// reset word wrap

	JXFileDocument* doc;
	if (CBGetDocumentManager()->FileDocumentIsOpen(itsFullName, &doc))
		{
		CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(doc);
		if (textDoc != nullptr)
			{
			te->SetBreakCROnly(textDoc->GetTextEditor()->WillBreakCROnly());
			}
		}

	ShowFirstDiff();
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

JCharacter
CBDiffDocument::ReadCmd
	(
	std::istream&		input,
	JIndexRange*	origRange,
	JIndexRange*	newRange
	)
	const
{
	*origRange           = ReadRange(input);
	const JCharacter cmd = input.get();
	*newRange            = ReadRange(input);
	return cmd;
}

/******************************************************************************
 ReadRange (private)

	Format:  x[,y]

 ******************************************************************************/

JIndexRange
CBDiffDocument::ReadRange
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
CBDiffDocument::IgnoreOrigText
	(
	std::istream&			input,
	const JCharacter	cmd
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
CBDiffDocument::ReadNewText
	(
	std::istream&			input,
	const JCharacter	cmd,
	JSize*				lineCount
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
			text.RemoveSubstring(text.GetLength(), text.GetLength());
			}
		}

	return text;
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
		const JXMenu::ItemSelected* selection =
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
							  kJFalse, this);
			}
		else if (itsType == kCVSType)
			{
			err = CreateCVS(itsFullName, itsGetCmd, itsDiffCmd, itsDefaultStyle, 
							itsName1, itsRemoveStyle, itsName2, itsInsertStyle,
							kJFalse, this);
			}
		else if (itsType == kSVNType)
			{
			err = CreateSVN(itsFullName, itsGetCmd, itsDiffCmd, itsDefaultStyle, 
							itsName1, itsRemoveStyle, itsName2, itsInsertStyle,
							kJFalse, this);
			}
		else
			{
			assert( itsType == kGitType );

			JIndex i;
			const JBoolean found = itsGetCmd.LocateSubstring(kGitCmdSeparator, &i);
			assert( found && 1 < i && i < itsGetCmd.GetLength() );
			const JString get1Cmd = itsGetCmd.GetSubstring(1, i-1);
			const JString get2Cmd = itsGetCmd.GetSubstring(i+1, itsGetCmd.GetLength());

			err = CreateGit(itsFullName, get1Cmd, get2Cmd, itsDiffCmd, itsDefaultStyle, 
							itsName1, itsRemoveStyle, itsName2, itsInsertStyle,
							kJFalse, this);
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
		ShowFirstDiff();
		}

	else if (index == kPrevDiffCmd)
		{
		ShowPrevDiff();
		}
	else if (index == kNextDiffCmd)
		{
		ShowNextDiff();
		}
}

/******************************************************************************
 ShowFirstDiff (private)

 ******************************************************************************/

void
CBDiffDocument::ShowFirstDiff()
{
	GetTextEditor()->SetCaretLocation(1);
	ShowNextDiff();
}

/******************************************************************************
 ShowPrevDiff (private)

 ******************************************************************************/

void
CBDiffDocument::ShowPrevDiff()
{
	CBTextEditor* te = GetTextEditor();
	te->Focus();

	JIndexRange origRange;
	const JIndex origIndex      = te->GetInsertionIndex();
	const JBoolean hadSelection = te->GetSelection(&origRange);

	JIndexRange removeRange, insertRange;
	JBoolean wrapped;
	if (te->JTextEditor::SearchBackward(MatchStyle(itsRemoveStyle), kJFalse, &wrapped))
		{
		const JBoolean ok = te->GetSelection(&removeRange);
		assert( ok );
		}

	te->SetCaretLocation(origIndex);
	if (te->JTextEditor::SearchBackward(MatchStyle(itsInsertStyle), kJFalse, &wrapped))
		{
		const JBoolean ok = te->GetSelection(&insertRange);
		assert( ok );
		}

	SelectDiff(removeRange, insertRange, JI2B(removeRange.first >= insertRange.first),
			   hadSelection, origIndex, origRange);
}

/******************************************************************************
 ShowNextDiff (private)

 ******************************************************************************/

void
CBDiffDocument::ShowNextDiff()
{
	CBTextEditor* te = GetTextEditor();
	te->Focus();

	JIndexRange origRange;
	const JIndex origIndex      = te->GetInsertionIndex();
	const JBoolean hadSelection = te->GetSelection(&origRange);

	JIndexRange removeRange, insertRange;
	JBoolean wrapped;
	if (te->JTextEditor::SearchForward(MatchStyle(itsRemoveStyle), kJFalse, &wrapped))
		{
		const JBoolean ok = te->GetSelection(&removeRange);
		assert( ok );
		}

	te->SetCaretLocation(hadSelection ? origRange.last+1 : origIndex);
	if (te->JTextEditor::SearchForward(MatchStyle(itsInsertStyle), kJFalse, &wrapped))
		{
		const JBoolean ok = te->GetSelection(&insertRange);
		assert( ok );
		}

	SelectDiff(removeRange, insertRange, JI2B(removeRange.first <= insertRange.first),
			   hadSelection, origIndex, origRange);
}

/******************************************************************************
 SelectDiff (private)

 ******************************************************************************/

void
CBDiffDocument::SelectDiff
	(
	const JIndexRange&	removeRange,
	const JIndexRange&	insertRange,
	const JBoolean		preferRemove,
	const JBoolean		hadSelection,
	const JIndex		origIndex,
	const JIndexRange&	origRange
	)
{
	CBTextEditor* te = GetTextEditor();

	const JBoolean foundRemove = !removeRange.IsNothing();
	const JBoolean foundInsert = !insertRange.IsNothing();
	if (foundRemove && foundInsert && preferRemove)
		{
		te->SetSelection(removeRange);
		}
	else if (foundRemove && foundInsert)
		{
		te->SetSelection(insertRange);
		}
	else if (foundRemove)
		{
		te->SetSelection(removeRange);
		}
	else if (foundInsert)
		{
		te->SetSelection(insertRange);
		}
	else if (hadSelection)
		{
		te->SetSelection(origRange);
		GetDisplay()->Beep();
		}
	else
		{
		te->SetCaretLocation(origIndex);
		}

	if (te->HasSelection())
		{
		te->TEScrollToSelection(kJTrue);
		}
	else
		{
		GetDisplay()->Beep();
		}
}

/******************************************************************************
 MatchStyle class (private)

 ******************************************************************************/

CBDiffDocument::MatchStyle::MatchStyle
	(
	const JFontStyle& style
	)
	:
	itsStyle(style)
{
}

CBDiffDocument::MatchStyle::~MatchStyle()
{
}

JBoolean
CBDiffDocument::MatchStyle::Match
	(
	const JFont& font
	)
	const
{
	return JI2B( font.GetStyle() == itsStyle );
}
