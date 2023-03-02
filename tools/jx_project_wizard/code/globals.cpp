/******************************************************************************
 globals.cpp

	Access to global objects and factories.

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "globals.h"
#include "App.h"
#include "PrefsManager.h"
#include "MainDialog.h"
#include <jx-af/jx/JXLibVersion.h>
#include <jx-af/jcore/jAssert.h>

static App*				theApplication  = nullptr;		// owns itself
static PrefsManager*	thePrefsManager = nullptr;
static MainDialog*		theMainDialog   = nullptr;

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
	assert(thePrefsManager != nullptr);

	JXInitHelp();

	return isNew;
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	theApplication = nullptr;
	theMainDialog  = nullptr;

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
		theMainDialog->JPrefObject::WritePrefs();
	}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 SetMainDialog

 ******************************************************************************/

void
SetMainDialog
	(
	MainDialog* dlog
	)
{
	assert( theMainDialog == nullptr );
	theMainDialog = dlog;
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
	assert(thePrefsManager != nullptr);
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

const JUtf8Byte*
GetVersionNumberStr()
{
	return kCurrentJXLibVersionStr;
}

/******************************************************************************
 GetVersionStr

 ******************************************************************************/

JString
GetVersionStr()
{
	const JUtf8Byte* map[] =
	{
		"version",   GetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	return JGetString("Description::globals", map, sizeof(map));
}
