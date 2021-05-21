/******************************************************************************
 testjxGlobals.cpp

	Access to testjx global objects and factories.

	Written by John Lindal.

 ******************************************************************************/

#include "testjxGlobals.h"
#include "TestDockManager.h"
#include <JXMenu.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

static TestApp*			theApplication = nullptr;		// owns itself
static TestMDIServer*	theMDIServer   = nullptr;		// owned by JX, can be nullptr

static const JUtf8Byte* kDockSetupFileName = "testjx_dock_data";

/******************************************************************************
 TestjxCreateGlobals

	server can be nullptr

 ******************************************************************************/

void
TestjxCreateGlobals
	(
	TestApp*		app,
	const JBoolean	wantMDI
	)
{
	theApplication = app;

	JXMenu::SetDefaultStyle(JXMenu::kWindowsStyle);

	JXCreateDefaultDocumentManager();

	TestDockManager* theDockManager =
		jnew TestDockManager(JXGetApplication()->GetCurrentDisplay());
	assert( theDockManager != nullptr );

	if (JFileExists(JString(kDockSetupFileName, kJFalse)))
		{
		std::ifstream input(kDockSetupFileName);
		theDockManager->ReadSetup(input);
		}

	app->OpenDocuments();

	if (wantMDI)
		{
		theMDIServer = jnew TestMDIServer;
		assert( theMDIServer != nullptr );
		}

	JXInitHelp();
}

/******************************************************************************
 TestjxDeleteGlobals

 ******************************************************************************/

void
TestjxDeleteGlobals()
{
	std::ofstream output(kDockSetupFileName);
	JXGetDockManager()->WriteSetup(output);

	theApplication = nullptr;
	theMDIServer   = nullptr;
}

/******************************************************************************
 TestjxGetApplication

 ******************************************************************************/

TestApp*
TestjxGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 TestjxGetMDIServer

 ******************************************************************************/

JBoolean
TestjxGetMDIServer
	(
	TestMDIServer** server
	)
{
	*server = theMDIServer;
	return JBoolean( theMDIServer != nullptr );
}
