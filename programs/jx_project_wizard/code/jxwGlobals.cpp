/******************************************************************************
 jxwGlobals.cpp

	Access to global objects and factories.

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jxwStdInc.h>
#include "jxwGlobals.h"
#include "JXWApp.h"
#include "JXWPrefsManager.h"
#include "JXWMainDialog.h"
#include "jxwHelpText.h"
#include <JXLibVersion.h>
#include <jAssert.h>

static JXWApp*			theApplication  = NULL;		// owns itself
static JXWPrefsManager*	thePrefsManager = NULL;
static JXWMainDialog*	theMainDialog   = NULL;

// string ID's

static const JCharacter* kDescriptionID = "Description::jxwGlobals";

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
	thePrefsManager	= new JXWPrefsManager(&isNew);
	assert(thePrefsManager != NULL);

	JXInitHelp(kJXWTOCHelpName, kJXWHelpSectionCount, kJXWHelpSectionName);

	return isNew;
}

/******************************************************************************
 JXWDeleteGlobals

 ******************************************************************************/

void
JXWDeleteGlobals()
{
	theApplication = NULL;
	theMainDialog  = NULL;

	// this must be last so everybody else can use it to save their setup

	delete thePrefsManager;
	thePrefsManager = NULL;
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
	assert( theMainDialog == NULL );
	theMainDialog = dlog;
}

/******************************************************************************
 JXWGetApplication

 ******************************************************************************/

JXWApp*
JXWGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 JXWGetPrefsManager

 ******************************************************************************/

JXWPrefsManager*
JXWGetPrefsManager()
{
	assert(thePrefsManager != NULL);
	return thePrefsManager;
}

/******************************************************************************
 JXWGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
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
	const JCharacter* map[] =
		{
		"version",   JXWGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}
