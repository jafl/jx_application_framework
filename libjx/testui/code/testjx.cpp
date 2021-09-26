/******************************************************************************
 testjx.cpp

	This provides a way to test the JX library and provides sample source
	code for everyone to study.

	Written by John Lindal.

 ******************************************************************************/

#include "TestDirector.h"
#include "TestApp.h"
#include "TestMDIServer.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXLibVersion.h>
#include <jx-af/jcore/JCoreLibVersion.h>
#include <string.h>
#include <stdlib.h>
#include <jx-af/jcore/jAssert.h>

// Prototypes

void ParseTextOptions(int* argc, char* argv[], bool* wantMDI);
void ParseXOptions(const int argc, char* argv[],
				   bool* startIconic, bool* bufferTestWidget,
				   bool* testWidgetIsImage, bool* snoopWindow);
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
	bool wantMDI;
	ParseTextOptions(&argc, argv, &wantMDI);

	if (wantMDI &&
		!TestMDIServer::WillBeMDIServer(TestApp::GetAppSignature(), argc, argv))
	{
		return 0;
	}

	TestApp* app = jnew TestApp(&argc, argv, wantMDI);
	assert( app != nullptr );

	bool startIconic, bufferTestWidget, testWidgetIsImage;
	bool snoopWindow;
	ParseXOptions(argc, argv, &startIconic, &bufferTestWidget,
				  &testWidgetIsImage, &snoopWindow);

	TestDirector* mainDir =
		jnew TestDirector(app, true, startIconic, bufferTestWidget,
						 testWidgetIsImage, snoopWindow);
	assert( mainDir != nullptr );

	app->SetMainDirector(mainDir);

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
	int*		argc,
	char*		argv[],
	bool*	wantMDI
	)
{
	*wantMDI = false;

	long index = 1;
	while (index < *argc)
	{
		if (strcmp(argv[index], "-h") == 0 ||
			strcmp(argv[index], "--help") == 0)
		{
			TestApp::InitStrings();
			TestMDIServer::PrintCommandLineHelp();
			exit(0);
		}
		else if (strcmp(argv[index], "-v") == 0 ||
				 strcmp(argv[index], "--version") == 0)
		{
			TestApp::InitStrings();
			PrintVersion();
			exit(0);
		}
		else if (strcmp(argv[index], "-MDI") == 0)
		{
			*wantMDI = true;
			JXApplication::RemoveCmdLineOption(argc, argv, index, 1);
			index--;
		}
		index++;
	}
}

/******************************************************************************
 ParseXOptions

	Modify the defaults based on the command line options.

	Since JXApplication has already removed all of its options, we can
	safely assume that whatever we don't understand is an error.

 ******************************************************************************/

void
ParseXOptions
	(
	const int	argc,
	char*		argv[],
	bool*	startIconic,
	bool*	bufferTestWidget,
	bool*	testWidgetIsImage,
	bool*	snoopWindow
	)
{
	*startIconic        = false;
	*bufferTestWidget   = true;
	*testWidgetIsImage  = false;
	*snoopWindow        = false;

	long index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-iconic") == 0)
		{
			*startIconic = true;
		}
		else if (strcmp(argv[index], "-nobuffer") == 0)
		{
			*bufferTestWidget = false;
		}
		else if (strcmp(argv[index], "-imagebuffer") == 0)
		{
			*testWidgetIsImage = true;
		}
		else if (strcmp(argv[index], "-snoopwindow") == 0)
		{
			*snoopWindow = true;
		}
		else
		{
			std::cerr << argv[0] << ": unknown option " << argv[index] << std::endl;
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
	std::cout << "JCore version " << kCurrentJCoreLibVersionStr << std::endl;
	std::cout << "JX    version " << kCurrentJXLibVersionStr << std::endl;
	std::cout << std::endl;
}
