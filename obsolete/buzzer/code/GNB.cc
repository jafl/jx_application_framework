/******************************************************************************
 GNB.cc
 
	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include <GNBApp.h>
#include <GNBMDIServer.h>
#include <GNBGlobals.h>

#include <JString.h>

#include <GNBGlobals.h>

#include <iostream.h>
#include <jAssert.h>

extern JCharacter* kAppSignature;

const JCharacter* kNotebookVersionNumberStr = "2.0";

const JCharacter* kNotebookVersionStr =

	"Buzzer Electronic Notebook 2.0\n"
	"\n"
	"An electronic notebook and to-do list program.\n"
	"\n"
	"Copyright (C) 2000-2003 by Glenn Bach.\n"
	"\n"
	"http://www.newplanetsoftware.com/notebook/\n";

// Prototypes

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
	
	if (!GNBMDIServer::WillBeMDIServer(kAppSignature, argc, argv))
		{
		return 0;
		}

	JBoolean displayAbout;
	JString prevVersStr;
	GNBApp* app =
		new GNBApp(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	if (displayAbout &&
		!(JGetUserNotification())->AcceptLicense())
		{
		return 0;
		}

	GNBGetMDIServer()->HandleCmdLineOptions(argc, argv);

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
		if (strcmp(argv[index], "-h") == 0)
			{
			PrintHelp();
			exit(0);
			}
		else if (strcmp(argv[index], "-v") == 0)
			{
			PrintVersion();
			exit(0);
			}
		index++;
		}
}

/******************************************************************************
 PrintHelp

	ruler:	 01234567890123456789012345678901234567890123456789012345678901234567890123456789

 ******************************************************************************/

void
PrintHelp()
{
	std::cout << std::endl;
	std::cout << "Notebook " << kNotebookVersionNumberStr << std::endl;
	std::cout << std::endl;
	std::cout << "This X application provides an electronic notebook." << std::endl;
	std::cout << std::endl;
	std::cout << "Because Notebook fully supports the Multiple Document concept," << std::endl;
	std::cout << "it can be safely invoked from the command line or via a file manager" << std::endl;
	std::cout << "as often as you like." << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "notebook" << std::endl;
	std::cout << std::endl;
	std::cout << "-h        prints this help" << std::endl;
	std::cout << "-v        prints version information" << std::endl;
	std::cout << std::endl;
	std::cout << "http://www.newplanetsoftware.com/buzzer/" << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << kNotebookVersionStr << std::endl;
	std::cout << std::endl;
}
