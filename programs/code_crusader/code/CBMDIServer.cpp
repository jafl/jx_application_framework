/******************************************************************************
 CBMDIServer.cpp

	BASE CLASS = JXMDIServer, JPrefObject

	Copyright © 1997-98 by John Lindal.

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
#include <JStringIterator.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jWebUtil.h>
#include <jVCSUtil.h>
#include <stdlib.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

CBMDIServer::CBMDIServer()
	:
	JXMDIServer(),
	JPrefObject(CBGetPrefsManager(), kCBMDIServerID)
{
	itsCreateEditorFlag  = false;
	itsCreateProjectFlag = false;
	itsReopenLastFlag    = true;
	itsChooseFileFlag    = false;

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
	const JString&				dir,
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

	const JString& execName = *argList.GetFirstElement();

	JXStandAlonePG pg;
	pg.RaiseWhenUpdate();
	if (argCount > 4)
		{
		pg.FixedLengthProcessBeginning(argCount-1,
			JGetString("OpeningFiles::CBMDIServer"), true, false);
		}

	JIndexRange lineRange;
	bool iconify     = false;
	bool forceReload = false;
	bool saveReopen  = CBProjectDocument::WillReopenTextFiles();

	bool restore = IsFirstTime();
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
			restore = false;
			}

		else if (arg == "--apropos" && i < argCount)
			{
			i++;
			CBManPageDocument::Create(nullptr, *(argList.GetElement(i)),
									  JString::empty, true);
			lineRange.SetToNothing();
			restore = false;
			}

		else if (arg == "--search")
			{
			AddFilesToSearch(&i, argList, true);
			lineRange.SetToNothing();
			}
		else if (arg == "--add-search")
			{
			AddFilesToSearch(&i, argList, false);
			lineRange.SetToNothing();
			}

		else if (arg == "--diff")
			{
			DisplayFileDiffs(&i, argList, false);
			lineRange.SetToNothing();
			restore = false;
			}
		else if (arg == "--diff-silent")
			{
			DisplayFileDiffs(&i, argList, true);
			lineRange.SetToNothing();
			restore = false;
			}
		else if (arg == "--cvs-diff")
			{
			DisplayVCSDiffs("cvs", &i, argList, false);
			lineRange.SetToNothing();
			restore = false;
			}
		else if (arg == "--cvs-diff-silent")
			{
			DisplayVCSDiffs("cvs", &i, argList, true);
			lineRange.SetToNothing();
			restore = false;
			}
		else if (arg == "--svn-diff")
			{
			DisplayVCSDiffs("svn", &i, argList, false);
			lineRange.SetToNothing();
			restore = false;
			}
		else if (arg == "--svn-diff-silent")
			{
			DisplayVCSDiffs("svn", &i, argList, true);
			lineRange.SetToNothing();
			restore = false;
			}
		else if (arg == "--git-diff")
			{
			DisplayVCSDiffs("git", &i, argList, false);
			lineRange.SetToNothing();
			restore = false;
			}
		else if (arg == "--git-diff-silent")
			{
			DisplayVCSDiffs("git", &i, argList, true);
			lineRange.SetToNothing();
			restore = false;
			}

		else if (arg == "--iconic")
			{
			iconify = true;
			}
		else if (arg == "--no-iconic")
			{
			iconify = false;
			}

		else if (arg == "--dock")
			{
			JXWindow::ShouldAutoDockNewWindows(true);
			}
		else if (arg == "--no-dock")
			{
			JXWindow::ShouldAutoDockNewWindows(false);
			}

		else if (arg == "--force-reload")
			{
			forceReload = true;
			}
		else if (arg == "--no-force-reload")
			{
			forceReload = false;
			}

		else if (arg == "--reload-open")
			{
			if (restore)
				{
				exit(0);
				}
			CBGetDocumentManager()->ReloadTextDocuments(false);
			}
		else if (arg == "--force-reload-open")
			{
			if (restore)
				{
				exit(0);
				}
			CBGetDocumentManager()->ReloadTextDocuments(true);
			}

		else if (arg == "--reopen")
			{
			CBProjectDocument::ShouldReopenTextFiles(true);
			}
		else if (arg == "--no-reopen")
			{
			CBProjectDocument::ShouldReopenTextFiles(false);
			restore = false;
			}

		else if (arg == "--turn-off-external-editor")		// in case they lock themselves out
			{
			CBGetDocumentManager()->ShouldEditTextFilesLocally(true);
			}

		else if (arg.GetFirstCharacter() == '-')
			{
			std::cerr << execName << ": unknown option " << arg << std::endl;
			}

		else if (arg.GetFirstCharacter() == '+' &&
				 arg.GetCharacterCount() > 1)
			{
			if (JString::ConvertToUInt(arg.GetBytes()+1, &lineRange.first))
				{
				lineRange.last = lineRange.first;
				}
			else
				{
				lineRange.SetToNothing();
				std::cerr << execName << ": invalid line number " << arg << std::endl;
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
				CBGetDocumentManager()->
					OpenSomething(fileName, lineRange, iconify, forceReload);
				}
			else
				{
				CBGetDocumentManager()->
					OpenTextDocument(fileName, lineRange, nullptr, iconify, forceReload);
				}
			lineRange.SetToNothing();
			restore = false;
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

	JXWindow::ShouldAutoDockNewWindows(true);

	// if argCount == 1, restore guaranteed to be true

	bool stateRestored  = false;
	bool projectCreated = false;
	if (restore && itsReopenLastFlag)
		{
		stateRestored = CBGetPrefsManager()->RestoreProgramState();
		}
	CBGetPrefsManager()->ForgetProgramState();	// toss it so we don't do it next time

	if (restore || argCount == 1)
		{
		if (itsCreateProjectFlag)
			{
			projectCreated = CBGetDocumentManager()->NewProjectDocument();
			}
		if (itsCreateEditorFlag ||
			(!stateRestored && !projectCreated &&
			 !itsCreateEditorFlag && !itsChooseFileFlag))
			{
			CBGetDocumentManager()->NewTextDocument();
			}
		if (itsChooseFileFlag)
			{
			CBGetDocumentManager()->OpenSomething();
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

	const JString* arg1 = nullptr;
	if (*index < argCount)
		{
		arg1 = argList.GetElement((*index)+1);
		}
	if (arg1 == nullptr || arg1->IsEmpty())
		{
		(CBGetViewManPageDialog())->Activate();
		return;
		}

	const JString* arg2 = nullptr;
	if (*index < argCount-1)
		{
		arg2 = argList.GetElement((*index)+2);
		}

	if (arg2 != nullptr && !arg2->IsEmpty() && arg2->GetFirstCharacter() != '-')
		{
		if (*arg1 == "-k")
			{
			// apropos

			CBManPageDocument::Create(nullptr, *arg2, JString::empty, true);
			*index += 2;
			return;
			}
		else
			{
			// display page of specified section

			CBManPageDocument::Create(nullptr, *arg2, *arg1);
			*index += 2;
			return;
			}
		}

	CBManPageDocument::Create(nullptr, *arg1);
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
	const bool					clearFileList
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
		dlog->ShouldSearchFiles(true);
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
	const bool					silent
	)
{
	const JSize argCount = argList.GetElementCount();

	JSize count = 0;
	if (*index < argCount-1)
		{
		const JString* arg2    = argList.GetElement(*index+2);
		const JUtf8Character c = arg2->IsEmpty() ? JUtf8Character(' ') : arg2->GetFirstCharacter();
		count                  = (c == '-' || c == '+') ? 1 : 2;
		}
	else if (*index == argCount-1)
		{
		count = 1;
		}

	JString file1, file2;
	if (count == 1)
		{
		file2 = *argList.GetElement(*index+1);
		if (file2.BeginsWith("#"))	// allows empty argument
			{
			file1 = file2;

			JStringIterator iter(&file1);
			JUtf8Character c;
			while (iter.Next(&c) && c == '#')
				{
				iter.RemovePrev();
				}

			iter.MoveTo(kJIteratorStartAtEnd, 0);
			while (iter.Prev(&c) && c == '#')
				{
				iter.RemoveNext();
				}
			}
		else
			{
			file1 = file2 + "~";
			}
		}
	else if (count == 2)
		{
		file1 = *argList.GetElement(*index+1);
		file2 = *argList.GetElement(*index+2);

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
		std::cerr << *argList.GetFirstElement() << ": too few arguments to --diff" << std::endl;
		}
	else if (!JConvertToAbsolutePath(file1, JString::empty, &full1))
		{
		const JUtf8Byte* map[] =
		{
			"name", file1.GetBytes()
		};
		const JString msg = JGetString("NotAFile::CBMDIServer", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		}
	else if (!JConvertToAbsolutePath(file2, JString::empty, &full2))
		{
		const JUtf8Byte* map[] =
		{
			"name", file2.GetBytes()
		};
		const JString msg = JGetString("NotAFile::CBMDIServer", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
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
	const JUtf8Byte*			type,
	JIndex*						index,
	const JPtrArray<JString>&	argList,
	const bool					silent
	)
{
	const JSize argCount = argList.GetElementCount();

	JSize count = 0;
	if (*index < argCount-1)
		{
		const JString* arg2    = argList.GetElement(*index+2);
		const JUtf8Character c = arg2->IsEmpty() ? JUtf8Character(' ') : arg2->GetFirstCharacter();
		count                  = (c == '-' || c == '+') ? 1 : 2;
		}
	else if (*index == argCount-1)
		{
		count = 1;
		}

	JString file, rev1, rev2;
	if (count == 1)
		{
		file = *argList.GetElement(*index+1);
		}
	else if (count == 2)
		{
		file = *argList.GetElement(*index+2);

		const JString* r = argList.GetElement(*index+1);

		JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
		r->Split(":", &s, 2);
		if (s.GetElementCount() == 2)
			{
			rev1 = *s.GetElement(1);
			rev2 = *s.GetElement(2);
			}
		else
			{
			rev1 = *r;
			}
		}

	*index += count;
	if (file.IsEmpty())
		{
		std::cerr << *argList.GetFirstElement() << ": too few arguments to --" << type << "-diff" << std::endl;
		return;
		}

	if (JIsURL(file) && strcmp(type, "svn") == 0)
		{
		JXGetApplication()->DisplayBusyCursor();

		JString type1, error;
		if (!JGetSVNEntryType(file, &type1, &error))
			{
			JGetUserNotification()->ReportError(error);
			return;
			}
		else if (type1 != "file")
			{
			const JUtf8Byte* map[] =
			{
				"name", file.GetBytes()
			};
			const JString msg = JGetString("DiffFailed::CBMDIServer", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			return;
			}
		}

	JString full = file;
	if (!JIsURL(file) && !JConvertToAbsolutePath(file, JString::empty, &full))
		{
		const JUtf8Byte* map[] =
		{
			"name", file.GetBytes()
		};
		const JString msg = JGetString("NotAFile::CBMDIServer", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
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

	input >> JBoolFromString(itsCreateEditorFlag)
		  >> JBoolFromString(itsCreateProjectFlag)
		  >> JBoolFromString(itsReopenLastFlag)
		  >> JBoolFromString(itsChooseFileFlag);
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

	output << ' ' << JBoolToString(itsCreateEditorFlag)
				  << JBoolToString(itsCreateProjectFlag)
				  << JBoolToString(itsReopenLastFlag)
				  << JBoolToString(itsChooseFileFlag);
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
CBMDIServer::PrintCommandLineHelp()
{
	const JUtf8Byte* map[] =
		{
		"version",   CBGetVersionNumberStr().GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	const JString s = JGetString("CommandLineHelp::CBMDIServer", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
