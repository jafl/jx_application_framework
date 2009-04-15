/******************************************************************************
 mdGlobals.h

   Copyright © 2008 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_mdGlobals
#define _H_mdGlobals

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jXGlobals.h>

// we include these for convenience

#include "MDApp.h"
#include "MDPrefsManager.h"
#include <JXDocumentManager.h>

class MDMDIServer;

MDApp*			MDGetApplication();
MDPrefsManager*	MDGetPrefsManager();
MDMDIServer*		MDGetMDIServer();

const JCharacter*	MDGetVersionNumberStr();
JString				MDGetVersionStr();

	// called by MDApp

JBoolean	MDCreateGlobals(MDApp* app);
void		MDDeleteGlobals();
void		MDCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

#endif
