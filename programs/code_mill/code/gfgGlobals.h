/******************************************************************************
 gfgGlobals.h

   Copyright (C) 2002 by Glenn W. Bach.

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

const JString&		GFGGetVersionNumberStr();
JString				GFGGetVersionStr();

	// called by GFGApp

bool	GFGCreateGlobals(GFGApp* app);
void		GFGDeleteGlobals();
void		GFGCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

#endif
