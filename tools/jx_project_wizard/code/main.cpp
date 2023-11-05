/******************************************************************************
 main.cpp

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "App.h"
#include "MainDialog.h"
#include "globals.h"
#include <jx-af/jcore/jCommandLine.h>
#include <jx-af/jcore/jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);
void PrintCommandLineHelp();
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

	bool displayLicense;
	auto* app = jnew App(&argc, argv, &displayLicense);

	auto* dlog = jnew MainDialog(app, argc, argv);
	dlog->Activate();

	if (displayLicense)
	{
		JScheduleTask([]()
		{
			if (!JGetUserNotification()->AcceptLicense())
			{
				ForgetPrefsManager();
			}
		});
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
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
PrintCommandLineHelp()
{
	const JUtf8Byte* map[] =
	{
		"vers", GetVersionNumberStr()
	};
	const JString s = JGetString("CommandLineHelp::main", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
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
