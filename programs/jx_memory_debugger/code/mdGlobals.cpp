/******************************************************************************
 mdGlobals.cc

	Access to global objects and factories.

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "mdGlobals.h"
#include "MDApp.h"
#include "MDPrefsManager.h"
#include "mdHelpText.h"
#include <JXWDManager.h>
#include <jAssert.h>

static MDApp*			theApplication  = NULL;		// owns itself
static MDPrefsManager*	thePrefsManager = NULL;

// string ID's

static const JCharacter* kDescriptionID = "Description::mdGlobals";

/******************************************************************************
 MDCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
MDCreateGlobals
	(
	MDApp* app
	)
{
	theApplication = app;

	JBoolean isNew;
	thePrefsManager	= new MDPrefsManager(&isNew);
	assert( thePrefsManager != NULL );

	JXInitHelp(kMDTOCHelpName, kMDHelpSectionCount, kMDHelpSectionName);

	JXWDManager* wdMgr = new JXWDManager(app->GetCurrentDisplay(), kJTrue);
	assert( wdMgr != NULL );
	// registers itself

	return isNew;
}

/******************************************************************************
 MDDeleteGlobals

 ******************************************************************************/

void
MDDeleteGlobals()
{
	theApplication = NULL;

	// this must be last so everybody else can use it to save their setup

	delete thePrefsManager;
	thePrefsManager = NULL;
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
	if (reason != JXDocumentManager::kAssertFired)
		{
		}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 MDGetApplication

 ******************************************************************************/

MDApp*
MDGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 MDGetPrefsManager

 ******************************************************************************/

MDPrefsManager*
MDGetPrefsManager()
{
	assert( thePrefsManager != NULL );
	return thePrefsManager;
}

/******************************************************************************
 MDGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
MDGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 MDGetVersionStr

 ******************************************************************************/

JString
MDGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}

/******************************************************************************
 MDGetWMClassInstance

 ******************************************************************************/

const JCharacter*
MDGetWMClassInstance()
{
	return "jx_memory_debugger";
}

const JCharacter*
MDGetMainWindowClass()
{
	return "jx_memory_debugger_Main_Window";
}
