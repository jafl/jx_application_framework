/******************************************************************************
 browser.cpp

	Main program for Code Crusader.

	Copyright © 1996-2000 by John Lindal.

 ******************************************************************************/

#include "CBMDIServer.h"
#include "CBTextDocument.h"
#include "cbGlobals.h"
#include <JXDockManager.h>
#include <JXSplashWindow.h>
#include <JXWindow.h>
#include <jCommandLine.h>
#include <jTime.h>
#include <jWebUtil.h>
#include <jSysUtil.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(const JSize argc, char* argv[], JString* commitFile);
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
	JString commitFile;
	ParseTextOptions(argc, argv, &commitFile);

	const bool useMDI = commitFile.IsEmpty();
	if (useMDI && !CBMDIServer::WillBeMDIServer(CBApp::GetAppSignature(), argc, argv))
		{
		return 0;
		}

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew CBApp(&argc, argv, useMDI, &displayAbout, &prevVersStr);
	assert( app != nullptr );

	if (displayAbout &&
		!JGetUserNotification()->AcceptLicense())
		{
		return 0;
		}

	if (useMDI)
		{
		JCheckForNewerVersion(CBGetPrefsManager(), kCBVersionCheckID);
		}

	CBMDIServer* mdi;
	if (CBGetMDIServer(&mdi))
		{
		mdi->HandleCmdLineOptions(argc, argv);
		}

	if (!commitFile.IsEmpty())
		{
		JXWindow::ShouldAutoDockNewWindows(false);

		CBDocumentManager* docMgr = CBGetDocumentManager();
		docMgr->OpenSomething(commitFile);

		JPtrArray<CBTextDocument>* docList = docMgr->GetTextDocList();
		if (!docList->IsEmpty())
			{
			(docList->GetFirstElement())->ShouldMakeBackupFile(false);
			}
		}

	if (displayAbout)
		{
		app->DisplayAbout(prevVersStr, true);
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
	const JSize	argc,
	char*		argv[],
	JString*	commitFile
	)
{
	commitFile->Clear();

	JIndex index = 1;
	while (index < argc)
		{
		if (JIsVersionRequest(argv[index]))
			{
			CBApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			CBApp::InitStrings();
			CBMDIServer::PrintCommandLineHelp();
			exit(0);
			}

		else if (strcmp(argv[index], "--vcs-commit") == 0 ||
				 strcmp(argv[index], "--cvs-commit") == 0)	// legacy
			{
			JCheckForValues(1, &index, argc, argv);
			*commitFile = argv[index];
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
	std::cout << CBGetVersionStr() << std::endl;
	std::cout << std::endl;
}
