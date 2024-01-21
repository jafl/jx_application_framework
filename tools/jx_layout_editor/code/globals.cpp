/******************************************************************************
 globals.cpp

	Access to global objects and factories.

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "globals.h"
#include "App.h"
#include "PrefsManager.h"
#include "MDIServer.h"
#include "DocumentManager.h"
#include "ImageCache.h"
#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/jAssert.h>

static App*				theApplication  = nullptr;		// owns itself
static PrefsManager*	thePrefsManager = nullptr;
static MDIServer*		theMDIServer    = nullptr;
static DocumentManager*	theDocManager   = nullptr;
static ImageCache*		theImageCache   = nullptr;

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

	theMDIServer  = jnew MDIServer;
	theDocManager = jnew DocumentManager;
	theImageCache = jnew ImageCache(app->GetDisplay(1));

	// widgets hidden in permanent window

	JXWindowDirector* permDir = jnew JXWindowDirector(JXGetPersistentWindowOwner());
	JXWindow* permWindow      = jnew JXWindow(permDir, 100, 100, JString::empty);

	theDocManager->CreateFileHistoryMenu(permWindow);

	return isNew;
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	if (thePrefsManager != nullptr)
	{
		theDocManager->JPrefObject::WritePrefs();
	}

	theApplication = nullptr;
	theMDIServer   = nullptr;
	theDocManager  = nullptr;

	jdelete theImageCache;
	theImageCache = nullptr;

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
 GetDocumentManager

 ******************************************************************************/

DocumentManager*
GetDocumentManager()
{
	assert( theDocManager != nullptr );
	return theDocManager;
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
 GetMDIServer

 ******************************************************************************/

MDIServer*
GetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 GetImageCache

 ******************************************************************************/

ImageCache*
GetImageCache()
{
	assert( theImageCache != nullptr );
	return theImageCache;
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
	return "jx_layout_editor";
}

const JUtf8Byte*
GetLayoutDocumentClass()
{
	return "jx_layout_editor_Layout";
}
