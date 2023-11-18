/******************************************************************************
 globals.cpp

	Access to global objects and factories.

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "globals.h"
#include "App.h"
#include "PrefsManager.h"
#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jcore/jAssert.h>

static App*					theApplication  = nullptr;		// owns itself
static PrefsManager*		thePrefsManager = nullptr;
static JXDocumentManager*	theDocManager   = nullptr;

/******************************************************************************
 CreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

bool
CreateGlobals
	(
	App* app
	)
{
	theApplication = app;

	bool isNew;
	thePrefsManager	= jnew PrefsManager(&isNew);

	JXInitHelp();

	theDocManager = jnew JXDocumentManager;

	return isNew;
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	theApplication = nullptr;
	theDocManager  = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 CleanUpBeforeSuddenDeath

	This must be the last one called by App so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
	{
	}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 GetApplication

 ******************************************************************************/

App*
GetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 GetPrefsManager

 ******************************************************************************/

PrefsManager*
GetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 ForgetPrefsManager

	Called when license is not accepted, to avoid writing prefs file.

 ******************************************************************************/

void
ForgetPrefsManager()
{
	thePrefsManager = nullptr;
}

/******************************************************************************
 GetVersionNumberStr

 ******************************************************************************/

const JString&
GetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 GetVersionStr

 ******************************************************************************/

JString
GetVersionStr()
{
	const JUtf8Byte* map[] =
	{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	return JGetString("Description::globals", map, sizeof(map));
}

/******************************************************************************
 GetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
GetWMClassInstance()
{
	return "jx_layout";
}

const JUtf8Byte*
GetMainDocumentClass()
{
	return "jx_layout_Document";
}

const JUtf8Byte*
GetLayoutDirectorClass()
{
	return "jx_layout_Layout";
}
