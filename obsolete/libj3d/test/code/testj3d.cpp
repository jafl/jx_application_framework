/******************************************************************************
 testj3d.cpp

	This provides a way to test the J3D library and provides sample source
	code for everyone to study.

	Written by John Lindal.

 ******************************************************************************/

#include "Test3DDirector.h"
#include "testj3dStringData.h"
#include <JXApplication.h>
#include <J3DLibVersion.h>
#include <string.h>
#include <stdlib.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(int* argc, char* argv[]);
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
	ParseTextOptions(&argc, argv);

	JXApplication* app = new JXApplication(&argc, argv, "testj3d", kDefaultStringData);
	assert( app != NULL );

	Test3DDirector* mainDir = new Test3DDirector(app);
	assert( mainDir != NULL );

	mainDir->Activate();
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
	int*	argc,
	char*	argv[]
	)
{
	long index = 1;
	while (index < *argc)
		{
		if (strcmp(argv[index], "-h") == 0 ||
			strcmp(argv[index], "--help") == 0)
			{
			PrintHelp();
			exit(0);
			}
		else if (strcmp(argv[index], "-v") == 0 ||
				 strcmp(argv[index], "--version") == 0)
			{
			PrintVersion();
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
	std::cout << std::endl;
	std::cout << "This X application demonstrates the capabilities of the J3D" << std::endl;
	std::cout << "class library." << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:  <options>" << std::endl;
	std::cout << std::endl;
	std::cout << "-h  prints help" << std::endl;
	std::cout << "-v  prints version information" << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << "J3D version " << kCurrentJ3DLibVersionStr << std::endl;
	std::cout << std::endl;
}
