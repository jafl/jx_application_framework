/******************************************************************************
 medic.cpp

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CMApp.h"
#include "CMCommandDirector.h"
#include "CMMDIServer.h"
#include "cmGlobals.h"
#include <JXDockManager.h>
#include <JXSplashWindow.h>
#include <jProcessUtil.h>
#include <jCommandLine.h>
#include <jTime.h>
#include <jWebUtil.h>
#include <jSysUtil.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);

void PrintHelp();
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

	if (!CMMDIServer::WillBeMDIServer(CMApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew CMApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != nullptr );

	if (displayAbout &&
		!JGetUserNotification()->AcceptLicense())
		{
		return 0;
		}

	JCheckForNewerVersion(CMGetPrefsManager(), kVersionCheckID);

	CMCreateCommandDirector();	// so dock appears after JCheckForNewerVersion()

	JXMDIServer* mdi;
	if (JXGetMDIServer(&mdi))
		{
		mdi->HandleCmdLineOptions(argc, argv);
		}

	if (displayAbout)
		{
		app->DisplayAbout(prevVersStr, true);
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
			CMApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			CMApp::InitStrings();
			PrintHelp();
			exit(0);
			}
		index++;
		}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	const JString s = JGetString("CMCommandLineHelp", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	std::cout << JGetString("CMDescription", map, sizeof(map));
	std::cout << std::endl << std::endl;
}
