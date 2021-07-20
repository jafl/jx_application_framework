/******************************************************************************
 mdGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "mdGlobals.h"
#include "MDApp.h"
#include "MDPrefsManager.h"
#include <JXWDManager.h>
#include <JXLibVersion.h>
#include <jAssert.h>

static MDApp*			theApplication  = nullptr;		// owns itself
static MDPrefsManager*	thePrefsManager = nullptr;

/******************************************************************************
 MDCreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

bool
MDCreateGlobals
	(
	MDApp* app
	)
{
	theApplication = app;

	bool isNew;
	thePrefsManager	= jnew MDPrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	auto* wdMgr = jnew JXWDManager(app->GetCurrentDisplay(), true);
	assert( wdMgr != nullptr );
	// registers itself

	return isNew;
}

/******************************************************************************
 MDDeleteGlobals

 ******************************************************************************/

void
MDDeleteGlobals()
{
	theApplication = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 MDCleanUpBeforeSuddenDeath

	This must be the last one called by MDApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
MDCleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
/*
	if (reason != JXDocumentManager::kAssertFired)
		{
		}
*/
	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 MDGetApplication

 ******************************************************************************/

MDApp*
MDGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 MDGetPrefsManager

 ******************************************************************************/

MDPrefsManager*
MDGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 MDGetVersionNumberStr

 ******************************************************************************/

const JUtf8Byte*
MDGetVersionNumberStr()
{
	return kCurrentJXLibVersionStr;
}

/******************************************************************************
 MDGetVersionStr

 ******************************************************************************/

JString
MDGetVersionStr()
{
	const JUtf8Byte* map[] =
		{
		"version",   MDGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::mdGlobals", map, sizeof(map));
}

/******************************************************************************
 MDGetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
MDGetWMClassInstance()
{
	return "jx_memory_debugger";
}

const JUtf8Byte*
MDGetMainWindowClass()
{
	return "jx_memory_debugger_Main_Window";
}
