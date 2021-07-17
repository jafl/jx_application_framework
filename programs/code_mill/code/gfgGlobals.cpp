/******************************************************************************
 gfgGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#include "gfgGlobals.h"
#include "GFGApp.h"
#include "GFGPrefsManager.h"
#include "GFGMDIServer.h"
#include <jAssert.h>

static GFGApp*			theApplication  = nullptr;		// owns itself
static GFGPrefsManager*	thePrefsManager = nullptr;
static GFGMDIServer*	theMDIServer    = nullptr;

/******************************************************************************
 GFGCreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

bool
GFGCreateGlobals
	(
	GFGApp* app
	)
{
	theApplication = app;

	bool isNew;
	thePrefsManager	= jnew GFGPrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theMDIServer = jnew GFGMDIServer;
	assert( theMDIServer != nullptr );

	return isNew;
}

/******************************************************************************
 GFGDeleteGlobals

 ******************************************************************************/

void
GFGDeleteGlobals()
{
	theApplication = nullptr;
	theMDIServer   = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
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
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 GFGGetPrefsManager

 ******************************************************************************/

GFGPrefsManager*
GFGGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 GFGGetMDIServer

 ******************************************************************************/

GFGMDIServer*
GFGGetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 GFGGetVersionNumberStr

 ******************************************************************************/

const JString&
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
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::gfgGlobals", map, sizeof(map));
}
