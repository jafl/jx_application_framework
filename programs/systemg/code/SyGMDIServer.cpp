/******************************************************************************
 SyGMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "SyGMDIServer.h"
#include "SyGGlobals.h"
#include <JXFSBindingManager.h>
#include <jDirUtil.h>
#include <jAssert.h>

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
	const JString&				dir,
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
	if (argCount == 2 && *(argList.GetLastElement()) == "--choose")
		{
		app->OpenDirectory();
		restore = kJFalse;
		}
	else if (argCount == 2 && *(argList.GetLastElement()) == "--open")
		{
		OpenFiles();
		if (IsFirstTime())
			{
			exit(0);
			}
		}
	else if (argCount == 2 && *(argList.GetLastElement()) == "--run")
		{
		JXFSBindingManager::Exec(dir);
		}
	else if (argCount > 1)
		{
		JBoolean forceNew       = kJTrue;
		JBoolean clearSelection = kJTrue;
		for (JIndex i=2; i<=argCount; i++)
			{
			const JString& arg = *(argList.GetElement(i));
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
				if (JConvertToAbsolutePath(arg, JString::empty, &path))
					{
					app->OpenDirectory(path, nullptr, nullptr, kJTrue, kJTrue, forceNew, clearSelection);
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
	if (JGetChooseSaveFile()->ChooseFiles(
			JGetString("OpenFilesPrompt::SyGEditPrefsDialog"),
			JString::empty, &fileList))
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
	const JString versStr = SyGGetVersionNumberStr();

	const JUtf8Byte* map[] =
		{
		"version",   versStr.GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	const JString s = JGetString("CommandLineHelp::SyGMDIServer", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
