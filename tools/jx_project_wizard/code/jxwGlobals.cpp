/******************************************************************************
 jxwGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "jxwGlobals.h"
#include "JXWApp.h"
#include "JXWPrefsManager.h"
#include "JXWMainDialog.h"
#include <JXLibVersion.h>
#include <jAssert.h>

static JXWApp*			theApplication  = nullptr;		// owns itself
static JXWPrefsManager*	thePrefsManager = nullptr;
static JXWMainDialog*	theMainDialog   = nullptr;

/******************************************************************************
 JXWCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
JXWCreateGlobals
	(
	JXWApp* app
	)
{
	theApplication = app;

	JBoolean isNew;
	thePrefsManager	= jnew JXWPrefsManager(&isNew);
	assert(thePrefsManager != nullptr);

	JXInitHelp();

	return isNew;
}

/******************************************************************************
 JXWDeleteGlobals

 ******************************************************************************/

void
JXWDeleteGlobals()
{
	theApplication = nullptr;
	theMainDialog  = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 JXWCleanUpBeforeSuddenDeath

	This must be the last one called by JXWApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
JXWCleanUpBeforeSuddenDeath
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
 JXWSetMainDialog

 ******************************************************************************/

void
JXWSetMainDialog
	(
	JXWMainDialog* dlog
	)
{
	assert( theMainDialog == nullptr );
	theMainDialog = dlog;
}

/******************************************************************************
 JXWGetApplication

 ******************************************************************************/

JXWApp*
JXWGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 JXWGetPrefsManager

 ******************************************************************************/

JXWPrefsManager*
JXWGetPrefsManager()
{
	assert(thePrefsManager != nullptr);
	return thePrefsManager;
}

/******************************************************************************
 JXWGetVersionNumberStr

 ******************************************************************************/

const JUtf8Byte*
JXWGetVersionNumberStr()
{
	return kCurrentJXLibVersionStr;
}

/******************************************************************************
 JXWGetVersionStr

 ******************************************************************************/

JString
JXWGetVersionStr()
{
	const JUtf8Byte* map[] =
		{
		"version",   JXWGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::jxwGlobals", map, sizeof(map));
}
