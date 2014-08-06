/******************************************************************************
 gmail.cc

 Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 ******************************************************************************/

#include "GMessageTableDir.h"
#include "GMApp.h"
#include "GMMDIServer.h"
#include "GPrefsMgr.h"
#include "GMGlobals.h"
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

	if (!GMMDIServer::WillBeMDIServer(GMApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	JBoolean displayAbout;
	JString prevVersStr;
	GMApp* app = new GMApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	JCheckForNewerVersion(GGetPrefsMgr(), kGVersionCheckID);

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
		if (strcmp(argv[index], "-h")	  == 0 ||
			strcmp(argv[index], "--help") == 0)
			{
			GMApp::InitStrings();
			GMMDIServer::PrintCommandLineHelp();
			exit(0);
			}
		else if (strcmp(argv[index], "-v") == 0 ||
				 strcmp(argv[index], "--version") == 0)
			{
			GMApp::InitStrings();
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
	cout << GMGetVersionStr() << endl;
	cout << endl;
}
