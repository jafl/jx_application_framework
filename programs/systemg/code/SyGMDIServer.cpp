/******************************************************************************
 SyGMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright © 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include "SyGMDIServer.h"
#include "SyGGlobals.h"
#include <JXFSBindingManager.h>
#include <jDirUtil.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::SyGMDIServer";

/******************************************************************************
 Constructor

 *****************************************************************************/

SyGMDIServer::SyGMDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

SyGMDIServer::~SyGMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
SyGMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize argCount = argList.GetElementCount();
	SyGApplication* app  = SyGGetApplication();

	const JString cwd = JGetCurrentDirectory();
	const JError err  = JChangeDirectory(dir);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	JBoolean restore = IsFirstTime();
	if (argCount == 2 && *(argList.LastElement()) == "--choose")
		{
		app->OpenDirectory();
		restore = kJFalse;
		}
	else if (argCount == 2 && *(argList.LastElement()) == "--open")
		{
		OpenFiles();
		if (IsFirstTime())
			{
			exit(0);
			}
		}
	else if (argCount == 2 && *(argList.LastElement()) == "--run")
		{
		JXFSBindingManager::Exec(dir);
		}
	else if (argCount > 1)
		{
		JBoolean forceNew       = kJTrue;
		JBoolean clearSelection = kJTrue;
		for (JIndex i=2; i<=argCount; i++)
			{
			const JString& arg = *(argList.NthElement(i));
			if (arg == "--no-force-new")
				{
				forceNew = kJFalse;
				}
			else if (arg == "--force-new")
				{
				forceNew = kJTrue;
				}
			else
				{
				JString path;
				if (JConvertToAbsolutePath(arg, NULL, &path))
					{
					app->OpenDirectory(path, NULL, NULL, kJTrue, kJTrue, forceNew, clearSelection);
					clearSelection = kJFalse;
					}
				restore = kJFalse;
				}
			}
		}

	JChangeDirectory(cwd);

	// if argCount == 1, restore guaranteed to be true

	if (restore && !app->RestoreProgramState())
		{
		app->OpenDirectory(dir);
		}
}

/******************************************************************************
 OpenFiles

 ******************************************************************************/

void
SyGMDIServer::OpenFiles()
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	if ((JGetChooseSaveFile())->ChooseFiles("Open files", NULL, &fileList))
		{
		JXFSBindingManager::Exec(fileList);
		}
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
SyGMDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"version",   SyGGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}
