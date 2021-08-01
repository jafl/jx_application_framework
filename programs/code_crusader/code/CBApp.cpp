/******************************************************************************
 CBApp.cpp

	BASE CLASS = JXApplication, JPrefObject

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "CBApp.h"
#include "CBAboutDialog.h"
#include "CBProjectDocument.h"
#include "CBEditMiscPrefsDialog.h"
#include "CBViewManPageDialog.h"
#include "cbGlobals.h"
#include "cbStringData.h"
#include "cbmUtil.h"
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

static const JUtf8Byte* kDefaultSysIncludeDir[] =
{
#ifdef _J_UNIX
	"/usr/include/",
	"/usr/local/include/",
#endif
};

static const JUtf8Byte* kExtraSysIncludeDir[] =
{
#ifdef _J_OSX
	"/usr/X11/include/"
#endif
};

// Application signature (MDI, prefs)

static const JUtf8Byte* kAppSignature = "jxcb";

// setup information

const JFileVersion kCurrentSetupVersion = 0;

// Help menu

static const JUtf8Byte* kHelpMenuStr =
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
	const bool	useMDI,
	bool*		displayAbout,
	JString*		prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kCBDefaultStringData),
	JPrefObject(nullptr, kCBAppID)
{
	itsWarnBeforeQuitFlag = false;

	itsSystemIncludeDirs = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSystemIncludeDirs != nullptr );

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
			*displayAbout = true;
			}
		}
	else
		{
		prevVersStr->Clear();
		}

	GetSystemIncludeDirectories();

	// Write shared prefs, if they don't exist.
	// (must be done after everything created)

	CBMWriteSharedPrefs(false);
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

bool
CBApp::Close()
{
	// --man with no args must leave window open

	if ((CBGetViewManPageDialog())->IsActive())
		{
		return false;
		}

	CBGetPrefsManager()->SaveProgramState();

	// close these first so they remember all open text documents

	if (!CBGetDocumentManager()->CloseProjectDocuments())
		{
		return false;
		}

	// close everything else

	const bool success = JXApplication::Close();	// deletes us if successful
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
		JGetUserNotification()->AskUserNo(JGetString("AskQuit::CBApp")))
		{
		JXApplication::Quit();
		}
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
CBApp::DisplayAbout
	(
	const JString&	prevVersStr,
	const bool	init
	)
{
	auto* dlog = jnew CBAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();

	if (init && prevVersStr.IsEmpty())
		{
		auto* task = jnew JXAskInitDockAll(dlog);
		assert( task != nullptr );
		task->Start();
		}
}

/******************************************************************************
 EditMiscPrefs

 ******************************************************************************/

void
CBApp::EditMiscPrefs()
{
	auto* dlog = jnew CBEditMiscPrefsDialog();
	assert( dlog != nullptr );
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
	const JUtf8Byte*	idNamespace
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
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
	const JUtf8Byte*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
		{
		DisplayAbout();
		}

	else if (index == kHelpTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kHelpOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection("CBOverviewHelp");
		}
	else if (index == kHelpTutorialCmd)
		{
		(JXGetHelpManager())->ShowSection("CBProjectTutorialHelp");
		}
	else if (index == kHelpWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(windowSectionName);
		}

	else if (index == kTipCmd)
		{
		auto* dlog = jnew JXTipOfTheDayDialog;
		assert( dlog != nullptr );
		dlog->BeginDialog();
		}

	else if (index == kHelpChangeLogCmd)
		{
		(JXGetHelpManager())->ShowChangeLog();
		}
	else if (index == kHelpCreditsCmd)
		{
		(JXGetHelpManager())->ShowCredits();
		}
}

/******************************************************************************
 FindFile

	Search for the specified file.  Notifies the user if it can't find it.

 ******************************************************************************/

bool
CBApp::FindFile
	(
	const JString&		fileName,
	const JString::Case	caseSensitive,
	JString*			fullName
	)
	const
{
	bool cancelled = false;

	const bool relative = JIsRelativePath(fileName);
	if (!relative && JFileExists(fileName))
		{
		*fullName = fileName;
		return true;
		}
	else if (relative)
		{
		CBDirInfoList searchPaths;
		CollectSearchPaths(&searchPaths);
		const JSize dirCount = searchPaths.GetElementCount(),
					sysCount = itsSystemIncludeDirs->GetElementCount();

		bool found = false;

		JLatentPG pg;

		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		const JString msg = JGetString("FileSearch::CBApp", map, sizeof(map));
		pg.FixedLengthProcessBeginning(dirCount+sysCount, msg, true, false);

		JString path, newName;
		for (JIndex i=1; i<=dirCount; i++)
			{
			const CBDirInfo info = searchPaths.GetElement(i);
			if (!info.recurse)
				{
				*fullName = JCombinePathAndName(*(info.path), fileName);
				if (JFileExists(*fullName))
					{
					found = true;
					break;
					}
				}
			else if (JSearchSubdirs(*(info.path), fileName, true, caseSensitive,
									&path, &newName, nullptr, &cancelled))
				{
				*fullName = JCombinePathAndName(path, newName);
				found     = true;
				break;
				}
			else if (cancelled)
				{
				break;
				}

			if (!pg.IncrementProgress())
				{
				cancelled = true;
				break;
				}
			}

		if (!found && !cancelled)
			{
			// We have to search system paths last because these are always
			// last on the compiler search path.

			for (JIndex i=1; i<=sysCount; i++)
				{
				if (JSearchSubdirs(*itsSystemIncludeDirs->GetElement(i), fileName, true, caseSensitive,
								   &path, &newName, nullptr, &cancelled))
					{
					*fullName = JCombinePathAndName(path, newName);
					found     = true;
					break;
					}
				else if (cancelled)
					{
					break;
					}

				if (!pg.IncrementProgress())
					{
					cancelled = true;
					break;
					}
				}
			}

		pg.ProcessFinished();
		searchPaths.DeleteAll();
		if (found)
			{
			return true;
			}
		}

	if (!cancelled)
		{
		const JUtf8Byte* map[] =
			{
			"name", fileName.GetBytes()
			};
		const JString msg = JGetString("FileNotFound::CBApp", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		}

	fullName->Clear();
	return false;
}

/******************************************************************************
 GetSystemIncludeDirectories (private)

 ******************************************************************************/

void
CBApp::GetSystemIncludeDirectories()
{
	for (const auto* s : kExtraSysIncludeDir)
		{
		if (JDirectoryExists(JString(s, JString::kNoCopy)))
			{
			auto* p = jnew JString(s);
			assert( p != nullptr );
			itsSystemIncludeDirs->Append(p);
			}
		}

	int pid, fd, inFD;
	const JError err = JExecute(JString("gcc -Wp,-v -x c++ -fsyntax-only -", JString::kNoCopy), &pid,
								kJCreatePipe, &inFD,
								kJCreatePipe, &fd,
								kJAttachToFromFD);
	if (!err.OK())
		{
		for (const auto* s : kDefaultSysIncludeDir)
			{
			if (JDirectoryExists(JString(s, JString::kNoCopy)))
				{
				auto* p = jnew JString(s);
				assert( p != nullptr );
				itsSystemIncludeDirs->Append(p);
				}
			}
		return;
		}

	close(inFD);	// must pass open fd to process and then explicitly close, to avoid a long wait

	JString s;
	while (true)
		{
		s = JReadUntil(fd, '\n');
		if (s.IsEmpty())
			{
			break;
			}

		if (s.GetFirstCharacter() == ' ')
			{
			s.TrimWhitespace();
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
		CBGetDocumentManager()->GetProjectDocList();

	const JSize docCount = docList->GetElementCount();
	JString truePath;
	bool recurse;
	for (JIndex j=1; j<=docCount; j++)
		{
		CBProjectDocument* doc   = docList->GetElement(j);
		const CBDirList& dirList = doc->GetDirectories();
		const JSize count        = dirList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (dirList.GetTruePath(i, &truePath, &recurse))
				{
				CBDirInfo newInfo(jnew JString(truePath), recurse);
				assert( newInfo.path != nullptr );
				newInfo.projIndex = j;

				bool found;
				const JIndex index =
					searchPaths->SearchSorted1(newInfo, JListT::kAnyMatch, &found);
				if (found)
					{
					// compute OR of recurse flags

					CBDirInfo existingInfo = searchPaths->GetElement(index);
					if (newInfo.recurse && !existingInfo.recurse)
						{
						existingInfo.recurse = true;
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

bool
CBApp::FindAndViewFile
	(
	const JString&		fileName,
	const JIndexRange	lineRange,		// not reference because of default value
	const JString::Case	caseSensitive
	)
	const
{
	JString fullName;
	if (FindFile(fileName, caseSensitive, &fullName))
		{
		CBGetDocumentManager()->OpenSomething(fullName, lineRange);
		return true;
		}
	else
		{
		return false;
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

	input >> JBoolFromString(itsWarnBeforeQuitFlag);
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

	output << ' ' << JBoolToString(itsWarnBeforeQuitFlag);
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
//		CBGetDocumentManager()->Save
		}

	CBCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

	Required for call to WillBeMDIServer() *before* app is constructed.

 ******************************************************************************/

const JUtf8Byte*
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
	JGetStringManager()->Register(kAppSignature, kCBDefaultStringData);
}
