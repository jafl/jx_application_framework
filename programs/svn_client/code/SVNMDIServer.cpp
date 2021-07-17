/******************************************************************************
 SVNMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "SVNMDIServer.h"
#include "SVNMainDirector.h"
#include "SVNRepoView.h"
#include "SVNRepoTree.h"
#include "svnGlobals.h"
#include <JStringIterator.h>
#include <JStringMatch.h>
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
	const JString&				dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize argCount = argList.GetElementCount();

	JString s, fullPath;
	Action action = kStatus;

	bool restore       = IsFirstTime();
	bool failedRefresh = false;
	for (JIndex i=2; i<=argCount; i++)
		{
		const JString& arg = *(argList.GetElement(i));
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
			bool isURL = JIsURL(arg);
			JString rev;
			if (isURL)
				{
				fullPath = arg;

				JStringIterator iter(&fullPath, kJIteratorStartAtEnd);
				iter.BeginMatch();
				if (iter.Prev("@"))
					{
					rev = iter.FinishMatch().GetString();
					if (rev.Contains("/"))
						{
						rev.Clear();	// found username instead
						}
					else
						{
						iter.RemoveAllNext();
						}
					}
				}
			else if (!JConvertToAbsolutePath(arg, dir, &s) ||
					 !JGetTrueName(s, &fullPath))
				{
				const JUtf8Byte* map[] =
					{
					"path", arg.GetBytes()
					};
				JGetUserNotification()->ReportError(JGetString("PathNotFound::SVNMDIServer", map, sizeof(map)));
				continue;
				}

			if (action == kRefreshRepo || action == kRefreshStatus)
				{
				SVNMainDirector* dir1;
				const bool open = (SVNGetWDManager())->GetBrowser(fullPath, &dir1);
				if (open && !dir1->OKToStartActionProcess())
					{
					JGetUserNotification()->ReportError(JGetString("WindowBusy::SVNMDIServer"));
					}
				else if (open && action == kRefreshRepo)
					{
					dir1->RefreshRepo();
					}
				else if (open && action == kRefreshStatus)
					{
					dir1->RefreshStatus();
					}
				else
					{
					failedRefresh = true;
					}
				}
			else
				{
				JString path = fullPath, name;
				if (isURL)
					{
					JXGetApplication()->DisplayBusyCursor();

					JString type, error;
					if (!JGetSVNEntryType(path, &type, &error))
						{
						JGetUserNotification()->ReportError(error);
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

				bool wasOpen;
				SVNMainDirector* dir1 = (SVNGetWDManager())->OpenDirectory(path, &wasOpen);
				if (action != kStatus && action != kInfoLog &&
					!dir1->OKToStartActionProcess())
					{
					JGetUserNotification()->ReportError(JGetString("WindowBusy::SVNMDIServer"));
					}
				else if (action == kUpdate)
					{
					dir1->UpdateWorkingCopy();
					}
				else if (action == kInfoLog)
					{
					dir1->ShowInfoLog(fullPath);
					}
				else if (action == kPropList)
					{
					dir1->ShowProperties(fullPath);
					}
				else if (isURL && !rev.IsEmpty())
					{
					dir1->BrowseRepo(rev);
					}
				else if (wasOpen)
					{
					dir1->RefreshRepo();
					dir1->RefreshStatus();

					SVNRepoView* widget;
					if (isURL && dir1->GetRepoWidget(&widget))
						{
						(widget->GetRepoTree())->SavePathToOpen(path);
						}
					}

				restore = false;
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
		SVNMainDirector* dir1;
		(SVNGetWDManager())->NewBrowser(&dir1);
		}
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
SVNMDIServer::PrintCommandLineHelp()
{
	const JUtf8Byte* map[] =
		{
		"vers",      SVNGetVersionNumberStr().GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	const JString s = JGetString("CommandLineHelp::SVNMDIServer", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
