/******************************************************************************
 main.cpp

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "MDApp.h"
#include "MDStatsDirector.h"
#include "mdGlobals.h"
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
	auto* app = jnew MDApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != nullptr );

	if (displayAbout &&
		!JGetUserNotification()->AcceptLicense())
	{
		return 0;
	}

	auto* dir = jnew MDStatsDirector(app);
	assert( dir != nullptr );
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
	std::cout << MDGetVersionStr() << std::endl;
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
		"vers",      MDGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	const JString s = JGetString("CommandLineHelp::mdGlobals", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}