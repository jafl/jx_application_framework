/******************************************************************************
 svnGlobals.cc

	Access to global objects and factories.

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "svnGlobals.h"
#include "SVNApp.h"
#include "SVNPrefsManager.h"
#include "SVNWDManager.h"
#include "SVNMDIServer.h"
#include "svnHelpText.h"
#include <JXFSBindingManager.h>
#include <jAssert.h>

static SVNApp*			theApplication  = NULL;		// owns itself
static SVNPrefsManager*	thePrefsManager = NULL;
static SVNWDManager*	theWDManager    = NULL;		// owned by JX
static SVNMDIServer*	theMDIServer    = NULL;

// string ID's

static const JCharacter* kDescriptionID = "Description::svnGlobals";

/******************************************************************************
 SVNCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
SVNCreateGlobals
	(
	SVNApp* app
	)
{
	theApplication = app;

	JXDisplay* display = app->GetCurrentDisplay();

	JBoolean isNew;
	thePrefsManager	= jnew SVNPrefsManager(&isNew);
	assert( thePrefsManager != NULL );

	JXInitHelp(kSVNTOCHelpName, kSVNHelpSectionCount, kSVNHelpSectionName);

	theWDManager = jnew SVNWDManager(display, kJTrue);
	assert( theWDManager != NULL );

	theMDIServer = jnew SVNMDIServer;
	assert( theMDIServer != NULL );

	return isNew;
}

/******************************************************************************
 SVNDeleteGlobals

 ******************************************************************************/

void
SVNDeleteGlobals()
{
	theApplication = NULL;
	theMDIServer   = NULL;
	theWDManager   = NULL;

	JXFSBindingManager::Destroy();

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = NULL;
}

/******************************************************************************
 SVNCleanUpBeforeSuddenDeath

	This must be the last one called by SVNApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
SVNCleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
//		theMDIServer->JPrefObject::WritePrefs();
		}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 SVNGetApplication

 ******************************************************************************/

SVNApp*
SVNGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 SVNGetPrefsManager

 ******************************************************************************/

SVNPrefsManager*
SVNGetPrefsManager()
{
	assert( thePrefsManager != NULL );
	return thePrefsManager;
}

/******************************************************************************
 SVNGetWDManager

 ******************************************************************************/

SVNWDManager*
SVNGetWDManager()
{
	assert( theWDManager != NULL );
	return theWDManager;
}

/******************************************************************************
 SVNGetMDIServer

 ******************************************************************************/

SVNMDIServer*
SVNGetMDIServer()
{
	assert( theMDIServer != NULL );
	return theMDIServer;
}

/******************************************************************************
 SVNGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
SVNGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 SVNGetVersionStr

 ******************************************************************************/

JString
SVNGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}

/******************************************************************************
 SVNGetWMClassInstance

 ******************************************************************************/

const JCharacter*
SVNGetWMClassInstance()
{
	return "NPS SVN Client";
}

const JCharacter*
SVNGetMainWindowClass()
{
	return "NPS_SVN_Client_Main_Window";
}
