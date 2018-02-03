/******************************************************************************
 jx_project_wizard.cc

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JXWApp.h"
#include "JXWMainDialog.h"
#include "jxwGlobals.h"
#include <jCommandLine.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);
void PrintCommandLineHelp();
void PrintVersion();

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

	JBoolean displayLicense;
	JXWApp* app = jnew JXWApp(&argc, argv, &displayLicense);
	assert( app != NULL );

	if (displayLicense &&
		!(JGetUserNotification())->AcceptLicense())
		{
		return 0;
		}

	JXWMainDialog* dlog = jnew JXWMainDialog(app, argc, argv);
	assert( dlog != NULL );
	dlog->Activate();

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
			JXWApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			JXWApp::InitStrings();
			PrintCommandLineHelp();
			exit(0);
			}
		index++;
		}
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"vers", JXWGetVersionNumberStr()
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << JXWGetVersionStr() << std::endl;
	std::cout << std::endl;
}
