/******************************************************************************
 testjx.cpp

	This provides a way to test the JX library and provides sample source
	code for everyone to study.

	Written by John Lindal.

 ******************************************************************************/

#include "TestDirector.h"
#include "TestApp.h"
#include "TestMDIServer.h"
#include <JXDisplay.h>
#include <JXColormap.h>
#include <JXLibVersion.h>
#include <JCoreLibVersion.h>
#include <string.h>
#include <stdlib.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(int* argc, char* argv[], JBoolean* wantMDI);
void ParseXOptions(const int argc, char* argv[],
				   JBoolean* startIconic, JBoolean* bufferTestWidget,
				   JBoolean* testWidgetIsImage, JBoolean* snoopWindow);
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
	JBoolean wantMDI;
	ParseTextOptions(&argc, argv, &wantMDI);

	if (wantMDI &&
		!TestMDIServer::WillBeMDIServer(TestApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	TestApp* app = jnew TestApp(&argc, argv, wantMDI);
	assert( app != NULL );

	JBoolean startIconic, bufferTestWidget, testWidgetIsImage;
	JBoolean snoopWindow;
	ParseXOptions(argc, argv, &startIconic, &bufferTestWidget,
				  &testWidgetIsImage, &snoopWindow);

	TestDirector* mainDir =
		jnew TestDirector(app, kJTrue, startIconic, bufferTestWidget,
						 testWidgetIsImage, snoopWindow);
	assert( mainDir != NULL );

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
	JBoolean*	wantMDI
	)
{
	*wantMDI = kJFalse;

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
			*wantMDI = kJTrue;
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
	JBoolean*	startIconic,
	JBoolean*	bufferTestWidget,
	JBoolean*	testWidgetIsImage,
	JBoolean*	snoopWindow
	)
{
	*startIconic        = kJFalse;
	*bufferTestWidget   = kJTrue;
	*testWidgetIsImage  = kJFalse;
	*snoopWindow        = kJFalse;

	long index = 1;
	while (index < argc)
		{
		if (strcmp(argv[index], "-iconic") == 0)
			{
			*startIconic = kJTrue;
			}
		else if (strcmp(argv[index], "-nobuffer") == 0)
			{
			*bufferTestWidget = kJFalse;
			}
		else if (strcmp(argv[index], "-imagebuffer") == 0)
			{
			*testWidgetIsImage = kJTrue;
			}
		else if (strcmp(argv[index], "-snoopwindow") == 0)
			{
			*snoopWindow = kJTrue;
			}
		else
			{
			cerr << argv[0] << ": unknown option " << argv[index] << endl;
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
	cout << "JCore version " << kCurrentJCoreLibVersionStr << endl;
	cout << "JX    version " << kCurrentJXLibVersionStr << endl;
	cout << endl;
}
