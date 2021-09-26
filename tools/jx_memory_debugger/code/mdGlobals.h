/******************************************************************************
 mdGlobals.h

   Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_mdGlobals
#define _H_mdGlobals

#include <jx-af/jx/jXGlobals.h>

// we include these for convenience

#include "MDApp.h"
#include "MDPrefsManager.h"
#include <jx-af/jx/JXDocumentManager.h>

MDApp*			MDGetApplication();
MDPrefsManager*	MDGetPrefsManager();

const JUtf8Byte*	MDGetVersionNumberStr();
JString				MDGetVersionStr();

	// called by MDApp

bool	MDCreateGlobals(MDApp* app);
void		MDDeleteGlobals();
void		MDCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	MDGetWMClassInstance();
const JUtf8Byte*	MDGetMainWindowClass();

#endif
