/******************************************************************************
 mondrian.cc

	Copyright © 2008 by New Planet Software. All rights reserved.

 ******************************************************************************/

#include <mdStdInc.h>
#include "MDApp.h"
#include "MDMDIServer.h"
#include "MDMainDirector.h"
#include "mdGlobals.h"
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

	if (!MDMDIServer::WillBeMDIServer(MDApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

#endif

	JBoolean displayAbout;
	JString prevVersStr;
	MDApp* app = new MDApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	if (displayAbout &&
		!(JGetUserNotification())->AcceptLicense())
		{
		return 0;
		}

	JCheckForNewerVersion(MDGetPrefsManager(), kMDVersionCheckID);

	(MDGetMDIServer())->HandleCmdLineOptions(argc, argv);

	// You may want to create all directors inside HandleCmdLineOptions()

	MDMainDirector* dir = new MDMainDirector(app);
	assert( dir != NULL );
	dir->Activate();

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
			MDApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			MDApp::InitStrings();
			MDMDIServer::PrintCommandLineHelp();
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
	cout << endl;
	cout << MDGetVersionStr() << endl;
	cout << endl;
}
