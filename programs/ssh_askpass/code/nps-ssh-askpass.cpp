/******************************************************************************
 nps-ssh-askpass.cc

	Copyright (C) 2006 by New Planet Software, Inc..

 ******************************************************************************/

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

	ASKApp* app = jnew ASKApp(&argc, argv);
	assert( app != NULL );

	// You may want to create all directors inside HandleCmdLineOptions()

	ASKDialog* dir = jnew ASKDialog(app);
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
	std::cout << std::endl;
	std::cout << ASKGetVersionStr() << std::endl;
	std::cout << std::endl;
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
	std::cout << std::endl << s << std::endl << std::endl;
}
