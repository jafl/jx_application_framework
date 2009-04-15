/******************************************************************************
 <pre>Globals.cc

	Access to global objects and factories.

	Copyright © <Year> by <Company>. All rights reserved.

 ******************************************************************************/

#include <<pre>StdInc.h>
#include "<pre>Globals.h"
#include "<PRE>App.h"
#include "<PRE>PrefsManager.h"
#include "<PRE>MDIServer.h"
#include "<pre>HelpText.h"
#include <jAssert.h>

static <PRE>App*			theApplication  = NULL;		// owns itself
static <PRE>PrefsManager*	thePrefsManager = NULL;
static <PRE>MDIServer*		theMDIServer    = NULL;

// string ID's

static const JCharacter* kDescriptionID = "Description::<pre>Globals";

/******************************************************************************
 <PRE>CreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
<PRE>CreateGlobals
	(
	<PRE>App* app
	)
{
	theApplication = app;

	JBoolean isNew;
	thePrefsManager	= new <PRE>PrefsManager(&isNew);
	assert( thePrefsManager != NULL );

	JXInitHelp(k<PRE>TOCHelpName, k<PRE>HelpSectionCount, k<PRE>HelpSectionName);

	theMDIServer = new <PRE>MDIServer;
	assert( theMDIServer != NULL );

	return isNew;
}

/******************************************************************************
 <PRE>DeleteGlobals

 ******************************************************************************/

void
<PRE>DeleteGlobals()
{
	theApplication = NULL;
	theMDIServer   = NULL;

	// this must be last so everybody else can use it to save their setup

	delete thePrefsManager;
	thePrefsManager = NULL;
}

/******************************************************************************
 <PRE>CleanUpBeforeSuddenDeath

	This must be the last one called by <PRE>App so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
<PRE>CleanUpBeforeSuddenDeath
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
 <PRE>GetApplication

 ******************************************************************************/

<PRE>App*
<PRE>GetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 <PRE>GetPrefsManager

 ******************************************************************************/

<PRE>PrefsManager*
<PRE>GetPrefsManager()
{
	assert( thePrefsManager != NULL );
	return thePrefsManager;
}

/******************************************************************************
 <PRE>GetMDIServer

 ******************************************************************************/

<PRE>MDIServer*
<PRE>GetMDIServer()
{
	assert( theMDIServer != NULL );
	return theMDIServer;
}

/******************************************************************************
 <PRE>GetVersionNumberStr

 ******************************************************************************/

const JCharacter*
<PRE>GetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 <PRE>GetVersionStr

 ******************************************************************************/

JString
<PRE>GetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}
