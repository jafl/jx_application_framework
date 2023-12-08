/******************************************************************************
 testjxGlobals.cpp

	Access to testjx global objects and factories.

	Written by John Lindal.

 ******************************************************************************/

#include "testjxGlobals.h"
#include "TestDockManager.h"
#include <jx-af/jx/JXMenu.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

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
	const bool	wantMDI
	)
{
	theApplication = app;

	JXCreateDefaultDocumentManager();

	TestDockManager* theDockManager =
		jnew TestDockManager(JXGetApplication()->GetCurrentDisplay());
	assert( theDockManager != nullptr );

	if (JFileExists(JString(kDockSetupFileName, JString::kNoCopy)))
	{
		std::ifstream input(kDockSetupFileName);
		theDockManager->ReadSetup(input);
	}

	app->OpenDocuments();

	if (wantMDI)
	{
		theMDIServer = jnew TestMDIServer;
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

bool
TestjxGetMDIServer
	(
	TestMDIServer** server
	)
{
	*server = theMDIServer;
	return bool( theMDIServer != nullptr );
}
