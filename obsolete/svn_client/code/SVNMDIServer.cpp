/******************************************************************************
 SVNMDIServer.cc

	<Description>

	BASE CLASS = public JXMDIServer

	Copyright © 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <svnStdInc.h>
#include "SVNMDIServer.h"
#include "SVNMainDirector.h"
#include "SVNRepoView.h"
#include "SVNRepoTree.h"
#include "svnGlobals.h"
#include <jDirUtil.h>
#include <jWebUtil.h>
#include <jStreamUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

SVNMDIServer::SVNMDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

SVNMDIServer::~SVNMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

enum Action
{
	kStatus,
	kRefreshRepo,
	kRefreshStatus,
	kUpdate,
	kInfoLog,
	kPropList
};

void
SVNMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize argCount = argList.GetElementCount();

	JString s, fullPath;
	Action action = kStatus;

	JBoolean restore       = IsFirstTime();
	JBoolean failedRefresh = kJFalse;
	for (JIndex i=2; i<=argCount; i++)
		{
		const JString& arg = *(argList.NthElement(i));
		if (arg == "--status")
			{
			action = kStatus;
			}
		else if (arg == "--refresh-repo")
			{
			action = kRefreshRepo;
			}
		else if (arg == "--refresh-status")
			{
			action = kRefreshStatus;
			}
		else if (arg == "--update")
			{
			action = kUpdate;
			}
		else if (arg == "--info-log")
			{
			action = kInfoLog;
			}
		else if (arg == "--proplist")
			{
			action = kPropList;
			}
		else
			{
			JBoolean isURL = JIsURL(arg);
			JString rev;
			if (isURL)
				{
				fullPath = arg;
				JIndex j;
				if (fullPath.LocateLastSubstring("@", &j) &&
					1 < j && j < fullPath.GetLength())
					{
					rev = fullPath.GetSubstring(j+1, fullPath.GetLength());
					if (rev.Contains("/"))
						{
						rev.Clear();	// found username instead
						}
					else
						{
						fullPath.RemoveSubstring(j, fullPath.GetLength());
						}
					}
				}
			else if (!JConvertToAbsolutePath(arg, dir, &s) ||
					 !JGetTrueName(s, &fullPath))
				{
				const JCharacter* map[] =
					{
					"path", arg
					};
				(JGetUserNotification())->ReportError(JGetString("PathNotFound::SVNMDIServer", map, sizeof(map)));
				continue;
				}

			if (action == kRefreshRepo || action == kRefreshStatus)
				{
				SVNMainDirector* dir;
				const JBoolean open = (SVNGetWDManager())->GetBrowser(fullPath, &dir);
				if (open && !dir->OKToStartActionProcess())
					{
					(JGetUserNotification())->ReportError(JGetString("WindowBusy::SVNMDIServer"));
					}
				else if (open && action == kRefreshRepo)
					{
					dir->RefreshRepo();
					}
				else if (open && action == kRefreshStatus)
					{
					dir->RefreshStatus();
					}
				else
					{
					failedRefresh = kJTrue;
					}
				}
			else
				{
				JString path = fullPath, name;
				if (isURL)
					{
					(JXGetApplication())->DisplayBusyCursor();

					JString type, error;
					if (!JGetSVNEntryType(path, &type, &error))
						{
						(JGetUserNotification())->ReportError(error);
						continue;
						}
					else if (type != "dir")
						{
						JStripTrailingDirSeparator(&fullPath);
						JSplitPathAndName(fullPath, &path, &name);
						}
					}
				else if (JFileExists(path))
					{
					JStripTrailingDirSeparator(&fullPath);
					JSplitPathAndName(fullPath, &path, &name);
					}

				JBoolean wasOpen;
				SVNMainDirector* dir = (SVNGetWDManager())->OpenDirectory(path, &wasOpen);
				if (action != kStatus && action != kInfoLog &&
					!dir->OKToStartActionProcess())
					{
					(JGetUserNotification())->ReportError(JGetString("WindowBusy::SVNMDIServer"));
					}
				else if (action == kUpdate)
					{
					dir->UpdateWorkingCopy();
					}
				else if (action == kInfoLog)
					{
					dir->ShowInfoLog(fullPath);
					}
				else if (action == kPropList)
					{
					dir->ShowProperties(fullPath);
					}
				else if (isURL && !rev.IsEmpty())
					{
					dir->BrowseRepo(rev);
					}
				else if (wasOpen)
					{
					dir->RefreshRepo();
					dir->RefreshStatus();

					SVNRepoView* widget;
					if (isURL && dir->GetRepoWidget(&widget))
						{
						(widget->GetRepoTree())->SavePathToOpen(path);
						}
					}

				restore = kJFalse;
				}
			}
		}

	// if argCount == 1, restore guaranteed to be true the first time

	if (restore && failedRefresh)
		{
		exit(0);	// don't lose state
		}
	else if (restore && !(SVNGetPrefsManager())->RestoreProgramState())
		{
		SVNMainDirector* dir;
		(SVNGetWDManager())->NewBrowser(&dir);
		}
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
SVNMDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"vers",      SVNGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString("CommandLineHelp::SVNMDIServer", map, sizeof(map));
	cout << endl << s << endl << endl;
}
