/******************************************************************************
 gpmGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 2001 by Glenn W. Bach.

 ******************************************************************************/

#include "gpmGlobals.h"
#include "GPMApp.h"
#include "GPMPrefsManager.h"
#include "GPMMDIServer.h"
#include <JRegex.h>
#include <jStreamUtil.h>

#ifdef _J_HAS_SYSCTL
#include <sys/sysctl.h>
#endif

#include <jAssert.h>

static GPMApp*			theApplication  = nullptr;		// owns itself
static GPMPrefsManager*	thePrefsManager = nullptr;
static GPMMDIServer*	theMDIServer    = nullptr;

static JSize			theSystemMemory      = 0;
static const JRegex		totalMemoryPattern   = "^MemTotal:\\s*([0-9]+)";

/******************************************************************************
 GPMCreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

bool
GPMCreateGlobals
	(
	GPMApp* app
	)
{
	theApplication = app;

	bool isNew;
	thePrefsManager	= jnew GPMPrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theMDIServer = jnew GPMMDIServer;
	assert( theMDIServer != nullptr );

#ifdef _J_HAS_PROC
	{
	std::ifstream ms("/proc/meminfo");
	JString line;
	while (ms.good() && !ms.eof())
		{
		line = JReadLine(ms);

		const JStringMatch m = totalMemoryPattern.Match(line, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
			{
			m.GetSubstring(1).ConvertToUInt(&theSystemMemory);	// usually kB
			break;
			}
		}
	}
#elif defined _J_HAS_SYSCTL
	{
	int mib[] = { CTL_HW, HW_PHYSMEM };
	int memPages;
	size_t len = sizeof(memPages);
	if (sysctl(mib, 2, &memPages, &len, nullptr, 0) == 0)
		{
		theSystemMemory = memPages;	// bytes
		}
	}
#endif

	return isNew;
}

/******************************************************************************
 GPMDeleteGlobals

 ******************************************************************************/

void
GPMDeleteGlobals()
{
	theApplication = nullptr;
	theMDIServer   = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 GPMCleanUpBeforeSuddenDeath

	This must be the last one called by GPMApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
GPMCleanUpBeforeSuddenDeath
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
 GPMGetApplication

 ******************************************************************************/

GPMApp*
GPMGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 GPMGetPrefsManager

 ******************************************************************************/

GPMPrefsManager*
GPMGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 GPMGetMDIServer

 ******************************************************************************/

GPMMDIServer*
GPMGetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 GPMGetVersionNumberStr

 ******************************************************************************/

const JString&
GPMGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 GPMGetVersionStr

 ******************************************************************************/

JString
GPMGetVersionStr()
{
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::gpmGlobals", map, sizeof(map));
}

/******************************************************************************
 GPMGetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
GPMGetWMClassInstance()
{
	return "Drakon";
}

const JUtf8Byte*
GPMGetMainWindowClass()
{
	return "Drakon_Main_Window";
}

/******************************************************************************
 GetSystemMemory

 ******************************************************************************/

bool
GPMGetSystemMemory
	(
	JSize* mem
	)
{
	*mem = theSystemMemory;
	return theSystemMemory != 0;
}
