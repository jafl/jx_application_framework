/******************************************************************************
 main.cpp

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "App.h"
#include "StatsDirector.h"
#include "globals.h"
#include <jx-af/jcore/jCommandLine.h>
#include <jx-af/jcore/jAssert.h>

// Prototypes

static void ParseTextOptions(const int argc, char* argv[]);
static void PrintVersion();
static void PrintCommandLineHelp();

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

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew App(&argc, argv, &displayAbout, &prevVersStr);

	auto* dir = jnew StatsDirector(app);
	dir->Activate();

	if (displayAbout)
	{
		app->DisplayAbout(true, prevVersStr);
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
			App::InitStrings();
			PrintVersion();
			exit(0);
		}
		else if (JIsHelpRequest(argv[index]))
		{
			App::InitStrings();
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
	std::cout << GetVersionStr() << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
PrintCommandLineHelp()
{
	const JUtf8Byte* map[] =
	{
		"vers",      GetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	const JString s = JGetString("CommandLineHelp::globals", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
