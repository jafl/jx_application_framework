/******************************************************************************
 forge.cpp

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#include "GFGApp.h"
#include "GFGMDIServer.h"
#include "GFGMainDirector.h"
#include "gfgGlobals.h"
#include <jCommandLine.h>
#include <jWebUtil.h>
#include <jAssert.h>

// Prototypes

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

#ifdef USE_MDI

	if (!GFGMDIServer::WillBeMDIServer(GFGApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

#endif

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew GFGApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != nullptr );

	if (displayAbout &&
		!JGetUserNotification()->AcceptLicense())
		{
		return 0;
		}

	JCheckForNewerVersion(GFGGetPrefsManager(), kGFGVersionCheckID);

	(GFGGetMDIServer())->HandleCmdLineOptions(argc, argv);

	if (displayAbout)
		{
		app->DisplayAbout(prevVersStr);
		}

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
			GFGApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			GFGApp::InitStrings();
			GFGMDIServer::PrintCommandLineHelp();
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
	std::cout << GFGGetVersionStr() << std::endl;
	std::cout << std::endl;
}
