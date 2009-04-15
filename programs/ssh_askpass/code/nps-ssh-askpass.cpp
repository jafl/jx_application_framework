/******************************************************************************
 nps-ssh-askpass.cc

	Copyright © 2006 by New Planet Software, Inc.. All rights reserved.

 ******************************************************************************/

#include <askStdInc.h>
#include "ASKApp.h"
#include "ASKDialog.h"
#include "askGlobals.h"
#include <jCommandLine.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);
void PrintVersion();
void PrintCommandLineHelp();

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::main";

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

	ASKApp* app = new ASKApp(&argc, argv);
	assert( app != NULL );

	// You may want to create all directors inside HandleCmdLineOptions()

	ASKDialog* dir = new ASKDialog(app);
	assert( dir != NULL );
	dir->BeginDialog();

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
			ASKApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			ASKApp::InitStrings();
			PrintCommandLineHelp();
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
	cout << ASKGetVersionStr() << endl;
	cout << endl;
}

/******************************************************************************
 PrintCommandLineHelp

 ******************************************************************************/

void
PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"version",   ASKGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}
