/******************************************************************************
 systemg.cpp

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGApplication.h"
#include "SyGMDIServer.h"
#include "SyGGlobals.h"
#include "SyGPrefsMgr.h"
#include <JXFSBindingManager.h>
#include <JXSplashWindow.h>
#include <jCommandLine.h>
#include <jTime.h>
#include <jWebUtil.h>
#include <jSysUtil.h>
#include <jAssert.h>

void ParseTextOptions(const int argc, char* argv[]);
void PrintVersion();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	ParseTextOptions(argc, argv);

	if (!SyGMDIServer::WillBeMDIServer(SyGApplication::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew SyGApplication(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != nullptr );

	if (displayAbout &&
		!JGetUserNotification()->AcceptLicense())
		{
		return 0;
		}

	JCheckForNewerVersion(SyGGetPrefsMgr(), kSVersionCheckID);

	(SyGGetMDIServer())->HandleCmdLineOptions(argc, argv);

	if (displayAbout)
		{
		app->DisplayAbout(prevVersStr);
		}

	JXFSBindingManager::Initialize();	// notify user of any upgrades

	app->Run();
	return 0;
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions won't gag.

 ******************************************************************************/

void
ParseTextOptions
	(
	const int	argc,
	char*		argv[]
	)
{
	long index = 1;
	while (index < argc)
		{
		if (JIsVersionRequest(argv[index]))
			{
			SyGApplication::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			SyGApplication::InitStrings();
			SyGMDIServer::PrintCommandLineHelp();
			exit(0);
			}
		index++;
		}
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << SyGGetVersionStr() << std::endl;
	std::cout << std::endl;
}
