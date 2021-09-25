/******************************************************************************
 <pre>Globals.cpp

	Access to global objects and factories.

	Copyright (C) <Year> by <Company>.

 ******************************************************************************/

#include "<pre>Globals.h"
#include "<PRE>App.h"
#include "<PRE>PrefsManager.h"
#include "<PRE>MDIServer.h"
#include <jAssert.h>

static <PRE>App*			theApplication  = nullptr;		// owns itself
static <PRE>PrefsManager*	thePrefsManager = nullptr;
static <PRE>MDIServer*		theMDIServer    = nullptr;

/******************************************************************************
 <PRE>CreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

bool
<PRE>CreateGlobals
	(
	<PRE>App* app
	)
{
	theApplication = app;

	bool isNew;
	thePrefsManager	= jnew <PRE>PrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theMDIServer = jnew <PRE>MDIServer;
	assert( theMDIServer != nullptr );

	return isNew;
}

/******************************************************************************
 <PRE>DeleteGlobals

 ******************************************************************************/

void
<PRE>DeleteGlobals()
{
	theApplication = nullptr;
	theMDIServer   = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
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
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 <PRE>GetPrefsManager

 ******************************************************************************/

<PRE>PrefsManager*
<PRE>GetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 <PRE>GetMDIServer

 ******************************************************************************/

<PRE>MDIServer*
<PRE>GetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 <PRE>GetVersionNumberStr

 ******************************************************************************/

const JString&
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
	const JUtf8Byte* map[] =
	{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	return JGetString("Description::<pre>Globals", map, sizeof(map));
}

/******************************************************************************
 <PRE>GetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
<PRE>GetWMClassInstance()
{
	return "<Binary>";
}

const JUtf8Byte*
<PRE>GetMainWindowClass()
{
	return "<Binary>_Main_Window";
}
