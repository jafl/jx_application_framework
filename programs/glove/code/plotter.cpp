/******************************************************************************
 plotter.cpp

	This provides a way to test the JX library and provides sample source
	code for everyone to study.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include <GLPlotApp.h>
#include "GLMDIServer.h"
#include "GLGlobals.h"

#include <JString.h>

#include <jCommandLine.h>
#include <jTime.h>

#include <stdlib.h>
#include <jAssert.h>

extern JCharacter* kGLSig;
extern JCharacter* kAppSignature;

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

	if (!GLMDIServer::WillBeMDIServer(kAppSignature, argc, argv))
		{
		return 0;
		}

	JBoolean displayAbout;
	JString prevVersStr;

	GLPlotApp* app =
		new GLPlotApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	GLGetMDIServer()->HandleCmdLineOptions(argc, argv);

	if (displayAbout)
		{
		app->DisplayAbout(prevVersStr);
		}

	app->Run();				// never actually returns
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
			GLPlotApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			GLPlotApp::InitStrings();
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
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString("GLCommandLineHelp", map, sizeof(map));
	cout << endl << s << endl << endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	cout << endl;
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	cout << JGetString("GLDescription", map, sizeof(map));
	cout << endl << endl;
}
