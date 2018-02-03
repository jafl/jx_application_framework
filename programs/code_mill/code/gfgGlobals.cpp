/******************************************************************************
 gfgGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 2002 by New Planet Software.

 ******************************************************************************/

#include "gfgGlobals.h"
#include "GFGApp.h"
#include "GFGPrefsManager.h"
#include "GFGMDIServer.h"
#include <jAssert.h>

static GFGApp*			theApplication  = NULL;		// owns itself
static GFGPrefsManager*	thePrefsManager = NULL;
static GFGMDIServer*	theMDIServer    = NULL;

// string ID's

static const JCharacter* kDescriptionID = "Description::gfgGlobals";

/******************************************************************************
 GFGCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
GFGCreateGlobals
	(
	GFGApp* app
	)
{
	theApplication = app;

	JBoolean isNew;
	thePrefsManager	= jnew GFGPrefsManager(&isNew);
	assert( thePrefsManager != NULL );

	JXInitHelp();

	theMDIServer = jnew GFGMDIServer;
	assert( theMDIServer != NULL );

	return isNew;
}

/******************************************************************************
 GFGDeleteGlobals

 ******************************************************************************/

void
GFGDeleteGlobals()
{
	theApplication = NULL;
	theMDIServer   = NULL;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = NULL;
}

/******************************************************************************
 GFGCleanUpBeforeSuddenDeath

	This must be the last one called by GFGApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
GFGCleanUpBeforeSuddenDeath
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
 GFGGetApplication

 ******************************************************************************/

GFGApp*
GFGGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 GFGGetPrefsManager

 ******************************************************************************/

GFGPrefsManager*
GFGGetPrefsManager()
{
	assert( thePrefsManager != NULL );
	return thePrefsManager;
}

/******************************************************************************
 GFGGetMDIServer

 ******************************************************************************/

GFGMDIServer*
GFGGetMDIServer()
{
	assert( theMDIServer != NULL );
	return theMDIServer;
}

/******************************************************************************
 GFGGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
GFGGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 GFGGetVersionStr

 ******************************************************************************/

JString
GFGGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}
