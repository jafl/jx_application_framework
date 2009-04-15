/******************************************************************************
 <Binary>.cc

	Copyright © <Year> by <Company>. All rights reserved.

 ******************************************************************************/

#include <<pre>StdInc.h>
#include "<PRE>App.h"
#include "<PRE>MDIServer.h"
#include "<PRE>MainDirector.h"
#include "<pre>Globals.h"
#include <jCommandLine.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);
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

#ifdef USE_MDI

	if (!<PRE>MDIServer::WillBeMDIServer(<PRE>App::GetAppSignature(), argc, argv))
		{
		return 0;
		}

#endif

	JBoolean displayAbout;
	JString prevVersStr;
	<PRE>App* app = new <PRE>App(&argc, argv, &displayAbout, &prevVersStr);
	assert( app != NULL );

	if (displayAbout &&
		!(JGetUserNotification())->AcceptLicense())
		{
		return 0;
		}

	(<PRE>GetMDIServer())->HandleCmdLineOptions(argc, argv);

	// You may want to create all directors inside HandleCmdLineOptions()

	<PRE>MainDirector* dir = new <PRE>MainDirector(app);
	assert( dir != NULL );
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
			<PRE>App::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			<PRE>App::InitStrings();
			<PRE>MDIServer::PrintCommandLineHelp();
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
	cout << <PRE>GetVersionStr() << endl;
	cout << endl;
}
