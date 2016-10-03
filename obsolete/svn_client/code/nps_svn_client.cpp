/******************************************************************************
 nps_svn_client.cc

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SVNApp.h"
#include "SVNMDIServer.h"
#include "SVNMainDirector.h"
#include "svnGlobals.h"
#include <jCommandLine.h>
#include <jWebUtil.h>
#include <jTime.h>
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

	if (!SVNMDIServer::WillBeMDIServer(SVNApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	JBoolean displayAbout;
	JString prevVersStr;
	SVNApp* app = new SVNApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	if (displayAbout &&
		!(JGetUserNotification())->AcceptLicense())
		{
		return 0;
		}

	JCheckForNewerVersion(SVNGetPrefsManager(), kSVNVersionCheckID);

	(SVNGetMDIServer())->HandleCmdLineOptions(argc, argv);

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
			SVNApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			SVNApp::InitStrings();
			SVNMDIServer::PrintCommandLineHelp();
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
	cout << SVNGetVersionStr() << endl;
	cout << endl;
}
