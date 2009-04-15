/******************************************************************************
 testjxGlobals.cpp

	Access to testjx global objects and factories.

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "testjxGlobals.h"
#include "testjxHelpText.h"
#include "TestDockManager.h"
#include <JXMenu.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

static TestApp*			theApplication = NULL;		// owns itself
static TestMDIServer*	theMDIServer   = NULL;		// owned by JX, can be NULL

static const JCharacter* kDockSetupFileName = "testjx_dock_data";

/******************************************************************************
 TestjxCreateGlobals

	server can be NULL

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
		new TestDockManager((JXGetApplication())->GetCurrentDisplay());
	assert( theDockManager != NULL );

	if (JFileExists(kDockSetupFileName))
		{
		ifstream input(kDockSetupFileName);
		theDockManager->ReadSetup(input);
		}

	if (wantMDI)
		{
		theMDIServer = new TestMDIServer;
		assert( theMDIServer != NULL );
		}

	JXInitHelp(NULL, kHelpSectionCount, kHelpSectionName);
}

/******************************************************************************
 TestjxDeleteGlobals

 ******************************************************************************/

void
TestjxDeleteGlobals()
{
	ofstream output(kDockSetupFileName);
	(JXGetDockManager())->WriteSetup(output);

	theApplication = NULL;
	theMDIServer   = NULL;
}

/******************************************************************************
 TestjxGetApplication

 ******************************************************************************/

TestApp*
TestjxGetApplication()
{
	assert( theApplication != NULL );
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
	return JBoolean( theMDIServer != NULL );
}
