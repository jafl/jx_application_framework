/******************************************************************************
 processmgr.cc

	Copyright © 2001 by New Planet Software. All rights reserved.

 ******************************************************************************/

#include "GPMApp.h"
#include "GPMMDIServer.h"
#include "GPMMainDirector.h"
#include "gpmGlobals.h"
#include <JThisProcess.h>
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

	if (!GPMMDIServer::WillBeMDIServer(GPMApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	JBoolean displayAbout;
	JString prevVersStr;
	GPMApp* app = new GPMApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	JCheckForNewerVersion(GPMGetPrefsManager(), kGPMVersionCheckID);

	(GPMGetMDIServer())->HandleCmdLineOptions(argc, argv);

	GPMMainDirector* dir = new GPMMainDirector(app);
	assert( dir != NULL );
	dir->Activate();
	(GPMGetMDIServer())->SetMainDirector(dir);

	if (displayAbout)
		{
		app->DisplayAbout(prevVersStr);
		}

	JThisProcess::Instance()->SetPriority(19);

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
		if (strcmp(argv[index], "-h") == 0 ||
			strcmp(argv[index], "--help") == 0)
			{
			GPMApp::InitStrings();
			GPMMDIServer::PrintCommandLineHelp();
			exit(0);
			}
		else if (strcmp(argv[index], "-v") == 0 ||
				 strcmp(argv[index], "--version") == 0)
			{
			GPMApp::InitStrings();
			PrintVersion();
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
	cout << GPMGetVersionStr() << endl;
	cout << endl;
}
