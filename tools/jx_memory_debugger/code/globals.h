/******************************************************************************
 globals.h

   Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_mdGlobals
#define _H_mdGlobals

#include <jx-af/jx/jXGlobals.h>

// we include these for convenience

#include "App.h"
#include "PrefsManager.h"
#include <jx-af/jx/JXDocumentManager.h>

App*			GetApplication();
PrefsManager*	GetPrefsManager();

const JUtf8Byte*	GetVersionNumberStr();
JString				GetVersionStr();

	// called by App

bool	CreateGlobals(App* app);
void		DeleteGlobals();
void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	GetWMClassInstance();
const JUtf8Byte*	GetMainWindowClass();

#endif
