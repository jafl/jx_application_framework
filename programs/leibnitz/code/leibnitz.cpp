/******************************************************************************
 leibnitz.cpp

	Calculator and function plotter.

	Copyright (C) 1998-2000 by John Lindal.

 ******************************************************************************/

#include "THXMDIServer.h"
#include "THXPrefsManager.h"
#include "thxGlobals.h"
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

	if (!THXMDIServer::WillBeMDIServer(THXApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	auto* app = jnew THXApp(&argc, argv);
	assert( app != nullptr );

	JCheckForNewerVersion(THXGetPrefsManager(), kTHXVersionCheckID);

	app->Run();
	return 0;
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions() won't gag.

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
			THXApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			THXApp::InitStrings();
			THXMDIServer::PrintCommandLineHelp();
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
	std::cout << THXGetVersionStr() << std::endl;
	std::cout << std::endl;
}
