/******************************************************************************
 CBApp.cpp

	BASE CLASS = JXApplication, JPrefObject

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBApp.h"
#include "CBAboutDialog.h"
#include "CBProjectDocument.h"
#include "CBEditMiscPrefsDialog.h"
#include "CBViewManPageDialog.h"
#include "cbGlobals.h"
#include "cbStringData.h"
#include "cbmUtil.h"
#include "cbHelpText.h"
#include <JXHelpManager.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <JXTipOfTheDayDialog.h>
#include <JXAskInitDockAll.h>
#include <jXActionDefs.h>
#include <JLatentPG.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

#ifdef _J_UNIX
static const JCharacter* kSysIncludeDir[] =
{
	"/usr/include/",
	"/usr/local/include/"
};
#endif

// Application signature (MDI, prefs)

static const JCharacter* kAppSignature = "jxcb";

// setup information

const JFileVersion kCurrentSetupVersion = 0;

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About" 
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | Tutorial"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Tip of the Day"
	"%l| Changes"
	"  | Credits";

enum
{
	kHelpAboutCmd = 1,
	kHelpTOCCmd, kHelpOverviewCmd, kHelpTutorialCmd, kHelpWindowCmd,
	kTipCmd,
	kHelpChangeLogCmd, kHelpCreditsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBApp::CBApp
	(
	int*			argc,
	char*			argv[],
	const JBoolean	useMDI,
	JBoolean*		displayAbout,
	JString*		prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kCBDefaultStringData),
	JPrefObject(NULL, kCBAppID)
{
	itsWarnBeforeQuitFlag = kJFalse;

	itsSystemIncludeDirs = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSystemIncludeDirs != NULL );

	*displayAbout = CBCreateGlobals(this, useMDI);
	SetPrefInfo(CBGetPrefsManager(), kCBAppID);
	JPrefObject::ReadPrefs();

	if (!*displayAbout)
		{
		*prevVersStr = CBGetPrefsManager()->GetJCCVersionStr();
		if (*prevVersStr == CBGetVersionNumberStr())
			{
			prevVersStr->Clear();
			}
		else
			{
			*displayAbout = kJTrue;
			}
		}
	else
		{
		prevVersStr->Clear();
		}

	GetSystemIncludeDirectories();

	// Write shared prefs, if they don't exist.
	// (must be done after everything created)

	CBMWriteSharedPrefs(kJFalse);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBApp::~CBApp()
{
	jdelete itsSystemIncludeDirs;

	JPrefObject::WritePrefs();
	CBDeleteGlobals();
}

/******************************************************************************
 Close (virtual protected)

 ******************************************************************************/

JBoolean
CBApp::Close()
{
	// --man with no args must leave window open

	if ((CBGetViewManPageDialog())->IsActive())
		{
		return kJFalse;
		}

	CBGetPrefsManager()->SaveProgramState();

	// close these first so they remember all open text documents

	if (!(CBGetDocumentManager())->CloseProjectDocuments())
		{
		return kJFalse;
		}

	// close everything else

	const JBoolean success = JXApplication::Close();	// deletes us if successful
	if (!success)
		{
		CBGetPrefsManager()->ForgetProgramState();
		}

	return success;
}

/******************************************************************************
 Quit (virtual)

 ******************************************************************************/

void
CBApp::Quit()
{
	if (!itsWarnBeforeQuitFlag ||
		(JGetUserNotification())->AskUserNo("Are you sure you want to quit?"))
		{
		JXApplication::Quit();
		}
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be NULL.

 ******************************************************************************/

void
CBApp::DisplayAbout
	(
	const JCharacter*	prevVersStr,
	const JBoolean		init
	)
{
	CBAboutDialog* dlog = jnew CBAboutDialog(this, prevVersStr);
	assert( dlog != NULL );
	dlog->BeginDialog();

	if (init && JStringEmpty(prevVersStr))
		{
		JXAskInitDockAll* task = jnew JXAskInitDockAll(dlog);
		assert( task != NULL );
		task->Start();
		}
}

/******************************************************************************
 EditMiscPrefs

 ******************************************************************************/

void
CBApp::EditMiscPrefs()
{
	CBEditMiscPrefsDialog* dlog = jnew CBEditMiscPrefsDialog();
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 CreateHelpMenu

 ******************************************************************************/

#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

JXTextMenu*
CBApp::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JCharacter*	idNamespace
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	menu->SetMenuItems(kHelpMenuStr, idNamespace);
	menu->SetUpdateAction(JXMenu::kDisableNone);

	menu->SetItemImage(kHelpTOCCmd,    jx_help_toc);
	menu->SetItemImage(kHelpWindowCmd, jx_help_specific);

	return menu;
}

/******************************************************************************
 AppendHelpMenuToToolBar

 ******************************************************************************/

void
CBApp::AppendHelpMenuToToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu* menu
	)
{
	toolBar->NewGroup();
	toolBar->AppendButton(menu, kHelpTOCCmd);
	toolBar->AppendButton(menu, kHelpWindowCmd);
}

/******************************************************************************
 UpdateHelpMenu

 ******************************************************************************/

void
CBApp::UpdateHelpMenu
	(
	JXTextMenu* menu
	)
{
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
CBApp::HandleHelpMenu
	(
	JXTextMenu*			menu,
	const JCharacter*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
		{
		DisplayAbout();
		}

	else if (index == kHelpTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kCBTOCHelpName);
		}
	else if (index == kHelpOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kCBOverviewHelpName);
		}
	else if (index == kHelpTutorialCmd)
		{
		(JXGetHelpManager())->ShowSection(kCBProjectTutorialHelpName);
		}
	else if (index == kHelpWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(windowSectionName);
		}

	else if (index == kTipCmd)
		{
		JXTipOfTheDayDialog* dlog = jnew JXTipOfTheDayDialog;
		assert( dlog != NULL );
		dlog->BeginDialog();
		}

	else if (index == kHelpChangeLogCmd)
		{
		(JXGetHelpManager())->ShowSection(kCBChangeLogName);
		}
	else if (index == kHelpCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kCBCreditsName);
		}
}

/******************************************************************************
 FindFile

	Search for the specified file.  Notifies the user if it can't find it.

 ******************************************************************************/

JBoolean
CBApp::FindFile
	(
	const JCharacter*	fileName,
	const JBoolean		caseSensitive,
	JString*			fullName
	)
	const
{
	JBoolean cancelled = kJFalse;

	const JBoolean relative = JIsRelativePath(fileName);
	if (!relative && JFileExists(fileName))
		{
		*fullName = fileName;
		return kJTrue;
		}
	else if (relative)
		{
		CBDirInfoList searchPaths;
		CollectSearchPaths(&searchPaths);
		const JSize dirCount = searchPaths.GetElementCount(),
					sysCount = itsSystemIncludeDirs->GetElementCount();

		JBoolean found = kJFalse;

		JLatentPG pg;
		JString msg = "Searching for \"";
		msg += fileName;
		msg += "\"...";
		pg.FixedLengthProcessBeginning(dirCount+sysCount, msg, kJTrue, kJFalse);

		JString path, newName;
		for (JIndex i=1; i<=dirCount; i++)
			{
			const CBDirInfo info = searchPaths.GetElement(i);
			if (!info.recurse)
				{
				*fullName = JCombinePathAndName(*(info.path), fileName);
				if (JFileExists(*fullName))
					{
					found = kJTrue;
					break;
					}
				}
			else if (JSearchSubdirs(*(info.path), fileName, kJTrue, caseSensitive,
									&path, &newName, NULL, &cancelled))
				{
				*fullName = JCombinePathAndName(path, newName);
				found     = kJTrue;
				break;
				}
			else if (cancelled)
				{
				break;
				}

			if (!pg.IncrementProgress())
				{
				cancelled = kJTrue;
				break;
				}
			}

		if (!found && !cancelled)
			{
			// We have to search system paths last because these are always
			// last on the compiler search path.

			for (JIndex i=1; i<=sysCount; i++)
				{
				if (JSearchSubdirs(*itsSystemIncludeDirs->GetElement(i), fileName, kJTrue, caseSensitive,
								   &path, &newName, NULL, &cancelled))
					{
					*fullName = JCombinePathAndName(path, newName);
					found     = kJTrue;
					break;
					}
				else if (cancelled)
					{
					break;
					}

				if (!pg.IncrementProgress())
					{
					cancelled = kJTrue;
					break;
					}
				}
			}

		pg.ProcessFinished();
		searchPaths.DeleteAll();
		if (found)
			{
			return kJTrue;
			}
		}

	if (!cancelled)
		{
		JString msg = "Unable to find the file \"";
		msg += fileName;
		msg += "\".";
		(JGetUserNotification())->ReportError(msg);
		}

	fullName->Clear();
	return kJFalse;
}

/******************************************************************************
 GetSystemIncludeDirectories (private)

 ******************************************************************************/

void
CBApp::GetSystemIncludeDirectories()
{
	int pid, fd, inFD;
	const JError err = JExecute("gcc -Wp,-v -x c++ -fsyntax-only -", &pid,
								kJCreatePipe, &inFD,
								kJCreatePipe, &fd,
								kJAttachToFromFD);
	if (!err.OK())
		{
		for (const JCharacter* s : kSysIncludeDir)
			{
			itsSystemIncludeDirs->Append(s);
			}
		return;
		}

	close(inFD);

	JString s;
	while (1)
		{
		s = JReadUntil(fd, '\n');
		if (s.IsEmpty())
			{
			break;
			}

		if (s.GetFirstCharacter() == ' ')
			{
			s.RemoveSubstring(1,1);
			if (!s.Contains(" "))
				{
				itsSystemIncludeDirs->Append(s);
				}
			}
		}
}

/******************************************************************************
 CollectSearchPaths (private)

	It's a hopeless mess to listen to all CBTrees and keep the list of paths
	up to date, so we build it every time it is needed.

 ******************************************************************************/

void
CBApp::CollectSearchPaths
	(
	CBDirInfoList* searchPaths
	)
	const
{
	searchPaths->DeleteAll();
	searchPaths->SetCompareFunction(CBDirInfo::ComparePathNames);

	JPtrArray<CBProjectDocument>* docList =
		(CBGetDocumentManager())->GetProjectDocList();

	const JSize docCount = docList->GetElementCount();
	JString truePath;
	JBoolean recurse;
	for (JIndex j=1; j<=docCount; j++)
		{
		CBProjectDocument* doc   = docList->NthElement(j);
		const CBDirList& dirList = doc->GetDirectories();
		const JSize count        = dirList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (dirList.GetTruePath(i, &truePath, &recurse))
				{
				CBDirInfo newInfo(jnew JString(truePath), recurse);
				assert( newInfo.path != NULL );
				newInfo.projIndex = j;

				JBoolean found;
				const JIndex index =
					searchPaths->SearchSorted1(newInfo, JOrderedSetT::kAnyMatch, &found);
				if (found)
					{
					// compute OR of recurse flags

					CBDirInfo existingInfo = searchPaths->GetElement(index);
					if (newInfo.recurse && !existingInfo.recurse)
						{
						existingInfo.recurse = kJTrue;
						searchPaths->SetElement(index, existingInfo);
						}
					jdelete newInfo.path;
					}
				else
					{
					searchPaths->InsertElementAtIndex(index, newInfo);
					}
				}
			}
		}

	searchPaths->SetCompareFunction(CBDirInfo::CompareProjIndex);
	searchPaths->Sort();
}

/******************************************************************************
 FindAndViewFile

	If we can find the specified file, tell text editor to display it.
	If lineRange is not empty, displays the specified lines of the file.

 ******************************************************************************/

JBoolean
CBApp::FindAndViewFile
	(
	const JCharacter*	fileName,
	const JIndexRange	lineRange,		// not reference because of default value
	const JBoolean		caseSensitive
	)
	const
{
	JString fullName;
	if (FindFile(fileName, caseSensitive, &fullName))
		{
		(CBGetDocumentManager())->OpenSomething(fullName, lineRange);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBApp::ReadPrefs
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

	input >> itsWarnBeforeQuitFlag;
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBApp::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ' << itsWarnBeforeQuitFlag;
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
CBApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
		JPrefObject::WritePrefs();
		CBGetPrefsManager()->SaveProgramState();
//		(CBGetDocumentManager())->Save
		}

	CBCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

	Required for call to WillBeMDIServer() *before* app is constructed.

 ******************************************************************************/

const JCharacter*
CBApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
CBApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kCBDefaultStringData);
}
