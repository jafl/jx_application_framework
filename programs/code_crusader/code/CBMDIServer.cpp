/******************************************************************************
 CBMDIServer.cpp

	BASE CLASS = JXMDIServer, JPrefObject

	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#include "CBMDIServer.h"
#include "CBProjectDocument.h"
#include "CBManPageDocument.h"
#include "CBSearchTextDialog.h"
#include "CBDiffFileDialog.h"
#include "CBViewManPageDialog.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXStandAlonePG.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jWebUtil.h>
#include <jVCSUtil.h>
#include <stdlib.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::CBMDIServer";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBMDIServer::CBMDIServer()
	:
	JXMDIServer(),
	JPrefObject(CBGetPrefsManager(), kCBMDIServerID)
{
	itsCreateEditorFlag  = kJFalse;
	itsCreateProjectFlag = kJFalse;
	itsReopenLastFlag    = kJTrue;
	itsChooseFileFlag    = kJFalse;

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBMDIServer::~CBMDIServer()
{
	// CBDeleteGlobals() calls WritePrefs()
}

/******************************************************************************
 HandleMDIRequest (virtual protected)

 ******************************************************************************/

void
CBMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize argCount = argList.GetElementCount();

	const JString origDir = JGetCurrentDirectory();
	const JError err      = JChangeDirectory(dir);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	const JString& execName = *(argList.FirstElement());

	JXStandAlonePG pg;
	pg.RaiseWhenUpdate();
	if (argCount > 4)
		{
		pg.FixedLengthProcessBeginning(argCount-1, "Opening files...", kJTrue, kJFalse);
		}

	JIndexRange lineRange;
	JBoolean iconify     = kJFalse;
	JBoolean forceReload = kJFalse;
	JBoolean saveReopen  = CBProjectDocument::WillReopenTextFiles();

	JBoolean restore = IsFirstTime();
	for (JIndex i=2; i<=argCount; i++)
		{
		const JString& arg = *(argList.GetElement(i));
		if (arg.IsEmpty())
			{
			continue;
			}

		if (arg == "--man")
			{
			DisplayManPage(&i, argList);
			lineRange.SetToNothing();
			restore = kJFalse;
			}

		else if (arg == "--apropos" && i < argCount)
			{
			i++;
			CBManPageDocument::Create(NULL, *(argList.GetElement(i)), ' ', kJTrue);
			lineRange.SetToNothing();
			restore = kJFalse;
			}

		else if (arg == "--search")
			{
			AddFilesToSearch(&i, argList, kJTrue);
			lineRange.SetToNothing();
			}
		else if (arg == "--add-search")
			{
			AddFilesToSearch(&i, argList, kJFalse);
			lineRange.SetToNothing();
			}

		else if (arg == "--diff")
			{
			DisplayFileDiffs(&i, argList, kJFalse);
			lineRange.SetToNothing();
			restore = kJFalse;
			}
		else if (arg == "--diff-silent")
			{
			DisplayFileDiffs(&i, argList, kJTrue);
			lineRange.SetToNothing();
			restore = kJFalse;
			}
		else if (arg == "--cvs-diff")
			{
			DisplayVCSDiffs("cvs", &i, argList, kJFalse);
			lineRange.SetToNothing();
			restore = kJFalse;
			}
		else if (arg == "--cvs-diff-silent")
			{
			DisplayVCSDiffs("cvs", &i, argList, kJTrue);
			lineRange.SetToNothing();
			restore = kJFalse;
			}
		else if (arg == "--svn-diff")
			{
			DisplayVCSDiffs("svn", &i, argList, kJFalse);
			lineRange.SetToNothing();
			restore = kJFalse;
			}
		else if (arg == "--svn-diff-silent")
			{
			DisplayVCSDiffs("svn", &i, argList, kJTrue);
			lineRange.SetToNothing();
			restore = kJFalse;
			}
		else if (arg == "--git-diff")
			{
			DisplayVCSDiffs("git", &i, argList, kJFalse);
			lineRange.SetToNothing();
			restore = kJFalse;
			}
		else if (arg == "--git-diff-silent")
			{
			DisplayVCSDiffs("git", &i, argList, kJTrue);
			lineRange.SetToNothing();
			restore = kJFalse;
			}

		else if (arg == "--iconic")
			{
			iconify = kJTrue;
			}
		else if (arg == "--no-iconic")
			{
			iconify = kJFalse;
			}

		else if (arg == "--dock")
			{
			JXWindow::ShouldAutoDockNewWindows(kJTrue);
			}
		else if (arg == "--no-dock")
			{
			JXWindow::ShouldAutoDockNewWindows(kJFalse);
			}

		else if (arg == "--force-reload")
			{
			forceReload = kJTrue;
			}
		else if (arg == "--no-force-reload")
			{
			forceReload = kJFalse;
			}

		else if (arg == "--reload-open")
			{
			if (restore)
				{
				exit(0);
				}
			(CBGetDocumentManager())->ReloadTextDocuments(kJFalse);
			}
		else if (arg == "--force-reload-open")
			{
			if (restore)
				{
				exit(0);
				}
			(CBGetDocumentManager())->ReloadTextDocuments(kJTrue);
			}

		else if (arg == "--reopen")
			{
			CBProjectDocument::ShouldReopenTextFiles(kJTrue);
			}
		else if (arg == "--no-reopen")
			{
			CBProjectDocument::ShouldReopenTextFiles(kJFalse);
			restore = kJFalse;
			}

		else if (arg == "--turn-off-external-editor")		// in case they lock themselves out
			{
			(CBGetDocumentManager())->ShouldEditTextFilesLocally(kJTrue);
			}

		else if (arg.GetFirstCharacter() == '-')
			{
			std::cerr << execName << ": unknown option " << arg << std::endl;
			}

		else if (arg.GetFirstCharacter() == '+' &&
				 arg.GetLength() > 1)
			{
			const JString s = arg.GetSubstring(2, arg.GetLength());
			if (s.ConvertToUInt(&(lineRange.first)))
				{
				lineRange.last = lineRange.first;
				}
			else
				{
				lineRange.SetToNothing();
				std::cerr << execName << ": invalid line number " << s << std::endl;
				}
			}
		else
			{
			JString fileName;
			JIndexRange tempRange;
			JExtractFileAndLine(arg, &fileName, &(tempRange.first), &(tempRange.last));
			if (!tempRange.IsEmpty())
				{
				lineRange = tempRange;
				}

			if (JFileExists(fileName))
				{
				(CBGetDocumentManager())->
					OpenSomething(fileName, lineRange, iconify, forceReload);
				}
			else
				{
				(CBGetDocumentManager())->
					OpenTextDocument(fileName, lineRange, NULL, iconify, forceReload);
				}
			lineRange.SetToNothing();
			restore = kJFalse;
			}

		if (pg.ProcessRunning() && !pg.IncrementProgress())
			{
			break;
			}
		}

	if (pg.ProcessRunning())
		{
		pg.ProcessFinished();
		}

	JChangeDirectory(origDir);

	JXWindow::ShouldAutoDockNewWindows(kJTrue);

	// if argCount == 1, restore guaranteed to be true

	JBoolean stateRestored  = kJFalse;
	JBoolean projectCreated = kJFalse;
	if (restore && itsReopenLastFlag)
		{
		stateRestored = CBGetPrefsManager()->RestoreProgramState();
		}
	CBGetPrefsManager()->ForgetProgramState();	// toss it so we don't do it next time

	if (restore || argCount == 1)
		{
		if (itsCreateProjectFlag)
			{
			projectCreated = (CBGetDocumentManager())->NewProjectDocument();
			}
		if (itsCreateEditorFlag ||
			(!stateRestored && !projectCreated &&
			 !itsCreateEditorFlag && !itsChooseFileFlag))
			{
			(CBGetDocumentManager())->NewTextDocument();
			}
		if (itsChooseFileFlag)
			{
			(CBGetDocumentManager())->OpenSomething();
			}
		}

	CBProjectDocument::ShouldReopenTextFiles(saveReopen);
}

/******************************************************************************
 DisplayManPage (private)

	Displays the requested man page and updates *index.

		--man page_name
		--man section_name page_name
		--man -k search_string

 ******************************************************************************/

void
CBMDIServer::DisplayManPage
	(
	JIndex*						index,
	const JPtrArray<JString>&	argList
	)
{
	const JSize argCount = argList.GetElementCount();

	const JString* arg1 = NULL;
	if (*index < argCount)
		{
		arg1 = argList.GetElement((*index)+1);
		}
	if (arg1 == NULL || arg1->IsEmpty())
		{
		(CBGetViewManPageDialog())->Activate();
		return;
		}

	const JString* arg2 = NULL;
	if (*index < argCount-1)
		{
		arg2 = argList.GetElement((*index)+2);
		}

	if (arg2 != NULL && !arg2->IsEmpty() && arg2->GetFirstCharacter() != '-')
		{
		if (arg1->GetLength() == 1)
			{
			// display page of specified section

			CBManPageDocument::Create(NULL, *arg2, arg1->GetFirstCharacter());
			*index += 2;
			return;
			}
		else if (*arg1 == "-k")
			{
			// apropos

			CBManPageDocument::Create(NULL, *arg2, ' ', kJTrue);
			*index += 2;
			return;
			}
		}

	CBManPageDocument::Create(NULL, *arg1);
	(*index)++;
}

/******************************************************************************
 AddFilesToSearch (private)

	Adds the remaining arguments to the multifile search list.
	(e.g. --search *.cc)

 ******************************************************************************/

void
CBMDIServer::AddFilesToSearch
	(
	JIndex*						index,
	const JPtrArray<JString>&	argList,
	const JBoolean				clearFileList
	)
{
	CBSearchTextDialog* dlog = CBGetSearchTextDialog();

	if (clearFileList)
		{
		dlog->ClearFileList();
		}

	const JSize count = argList.GetElementCount();
	JString fullName;
	for (JIndex i=*index+1; i<=count; i++)
		{
		const JString* fileName = argList.GetElement(i);
		if (JGetTrueName(*fileName, &fullName))
			{
			dlog->AddFileToSearch(fullName);
			}
		}

	*index = count+1;

	if (clearFileList)
		{
		dlog->ShouldSearchFiles(kJTrue);
		}
	dlog->Activate();
}

/******************************************************************************
 DisplayFileDiffs (private)

	Displays the differences between the two specified files
	and updates *index.

	If the second argument is a directory, we use the name from the first file.
	If only one file is specified, we attempt to diff against the ~ version,
	unless it starts with #, in which case we compare it against the original.

 ******************************************************************************/

void
CBMDIServer::DisplayFileDiffs
	(
	JIndex*						index,
	const JPtrArray<JString>&	argList,
	const JBoolean				silent
	)
{
	const JSize argCount = argList.GetElementCount();

	JSize count = 0;
	if (*index < argCount-1)
		{
		const JString* arg2 = argList.GetElement((*index)+2);
		const JCharacter c  = arg2->IsEmpty() ? ' ' : arg2->GetFirstCharacter();
		count               = (c == '-' || c == '+') ? 1 : 2;
		}
	else if (*index == argCount-1)
		{
		count = 1;
		}

	JString file1, file2;
	if (count == 1)
		{
		file2 = *(argList.GetElement((*index)+1));
		if (file2.BeginsWith("#"))	// allows empty argument
			{
			file1 = file2;
			while (file1.GetFirstCharacter() == '#')
				{
				file1.RemoveSubstring(1, 1);
				}
			while (file1.GetLastCharacter() == '#')
				{
				file1.RemoveSubstring(file1.GetLength(), file1.GetLength());
				}
			}
		else
			{
			file1 = file2 + "~";
			}
		}
	else if (count == 2)
		{
		file1 = *(argList.GetElement((*index)+1));
		file2 = *(argList.GetElement((*index)+2));

		if (JFileExists(file1) && JDirectoryExists(file2))
			{
			JStripTrailingDirSeparator(&file1);
			JString path, name;
			JSplitPathAndName(file1, &path, &name);
			file2 = JCombinePathAndName(file2, name);
			}
		}

	*index += count;
	JString full1, full2;
	if (file1.IsEmpty() || file2.IsEmpty())
		{
		std::cerr << *(argList.FirstElement()) << ": too few arguments to --diff" << std::endl;
		}
	else if (!JConvertToAbsolutePath(file1, NULL, &full1))
		{
		JString msg = "\"";
		msg += file1;
		msg += "\" does not exist or is not a file.";
		(JGetUserNotification())->ReportError(msg);
		}
	else if (!JConvertToAbsolutePath(file2, NULL, &full2))
		{
		JString msg = "\"";
		msg += file2;
		msg += "\" does not exist or is not a file.";
		(JGetUserNotification())->ReportError(msg);
		}
	else
		{
		(CBGetDiffFileDialog())->ViewDiffs(full1, full2, silent);
		}
}

/******************************************************************************
 DisplayVCSDiffs (private)

	Displays the differences between the two specified revisions
	and updates *index.

 ******************************************************************************/

void
CBMDIServer::DisplayVCSDiffs
	(
	const JCharacter*			type,
	JIndex*						index,
	const JPtrArray<JString>&	argList,
	const JBoolean				silent
	)
{
	const JSize argCount = argList.GetElementCount();

	JSize count = 0;
	if (*index < argCount-1)
		{
		const JString* arg2 = argList.GetElement((*index)+2);
		const JCharacter c  = arg2->IsEmpty() ? ' ' : arg2->GetFirstCharacter();
		count               = (c == '-' || c == '+') ? 1 : 2;
		}
	else if (*index == argCount-1)
		{
		count = 1;
		}

	JString file, rev1, rev2;
	if (count == 1)
		{
		file = *(argList.GetElement((*index)+1));
		}
	else if (count == 2)
		{
		file = *(argList.GetElement((*index)+2));

		const JString* r = argList.GetElement((*index)+1);
		JIndex i;
		if (r->LocateSubstring(":", &i) &&
			1 < i && i < r->GetLength())
			{
			rev1 = r->GetSubstring(1, i-1);
			rev2 = r->GetSubstring(i+1, r->GetLength());
			}
		else
			{
			rev1 = *r;
			}
		}

	*index += count;
	if (file.IsEmpty())
		{
		std::cerr << *(argList.FirstElement()) << ": too few arguments to --" << type << "-diff" << std::endl;
		return;
		}

	if (JIsURL(file) && strcmp(type, "svn") == 0)
		{
		(JXGetApplication())->DisplayBusyCursor();

		JString type, error;
		if (!JGetSVNEntryType(file, &type, &error))
			{
			(JGetUserNotification())->ReportError(error);
			return;
			}
		else if (type != "file")
			{
			JString msg = "Unable to display diff because \"";
			msg += file;
			msg += "\" is not a file.";
			(JGetUserNotification())->ReportError(msg);
			return;
			}
		}

	JString full = file;
	if (!JIsURL(file) && !JConvertToAbsolutePath(file, NULL, &full))
		{
		JString msg = "\"";
		msg += file;
		msg += "\" does not exist or is not a file.";
		(JGetUserNotification())->ReportError(msg);
		return;
		}

	if (strcmp(type, "cvs") == 0)
		{
		(CBGetDiffFileDialog())->ViewCVSDiffs(full, rev1, rev2, silent);
		}
	else if (strcmp(type, "svn") == 0)
		{
		(CBGetDiffFileDialog())->ViewSVNDiffs(full, rev1, rev2, silent);
		}
	else if (strcmp(type, "git") == 0)
		{
		(CBGetDiffFileDialog())->ViewGitDiffs(full, rev1, rev2, silent);
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBMDIServer::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	input >> itsCreateEditorFlag >> itsCreateProjectFlag;
	input >> itsReopenLastFlag >> itsChooseFileFlag;
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBMDIServer::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ' << itsCreateEditorFlag;
	output << ' ' << itsCreateProjectFlag;
	output << ' ' << itsReopenLastFlag;
	output << ' ' << itsChooseFileFlag;
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
CBMDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"version",   CBGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
