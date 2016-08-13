/******************************************************************************
 wiz.cpp

	Graphical WizWar client.

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "WizApp.h"
#include "wwMessages.h"
#include "wizGlobals.h"
#include <jCommandLine.h>
#include <stdlib.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);

void PrintHelp();
void PrintVersion();

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::wizMain";

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

	WizApp* app = new WizApp(&argc, argv);
	assert( app != NULL );

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
			WizApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			WizApp::InitStrings();
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
	const JString protVers(kWWCurrentProtocolVersion, 0);
	const JCharacter* map[] =
		{
		"vers",          WizGetVersionNumberStr(),
		"protocol_vers", protVers
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	cout << endl;
	cout << WizGetVersionStr() << endl;
	cout << endl;
}
