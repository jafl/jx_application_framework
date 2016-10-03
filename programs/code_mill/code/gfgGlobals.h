/******************************************************************************
 gfgGlobals.h

   Copyright (C) 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_gfgGlobals
#define _H_gfgGlobals

#include <jXGlobals.h>

// we include these for convenience

#include "GFGApp.h"
#include "GFGPrefsManager.h"
#include <JXDocumentManager.h>

class GFGMDIServer;

GFGApp*				GFGGetApplication();
GFGPrefsManager*	GFGGetPrefsManager();
GFGMDIServer*		GFGGetMDIServer();

const JCharacter*	GFGGetVersionNumberStr();
JString				GFGGetVersionStr();

	// called by GFGApp

JBoolean	GFGCreateGlobals(GFGApp* app);
void		GFGDeleteGlobals();
void		GFGCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

#endif
