/******************************************************************************
 svnGlobals.h

   Copyright (C) 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_svnGlobals
#define _H_svnGlobals

#include <jXGlobals.h>

// we include these for convenience

#include "SVNApp.h"
#include "SVNPrefsManager.h"
#include "SVNWDManager.h"
#include <JXDocumentManager.h>

class SVNMDIServer;

SVNApp*				SVNGetApplication();
SVNPrefsManager*	SVNGetPrefsManager();
SVNWDManager*		SVNGetWDManager();
SVNMDIServer*		SVNGetMDIServer();

const JCharacter*	SVNGetVersionNumberStr();
JString				SVNGetVersionStr();

	// called by SVNApp

JBoolean	SVNCreateGlobals(SVNApp* app);
void		SVNDeleteGlobals();
void		SVNCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JCharacter*	SVNGetWMClassInstance();
const JCharacter*	SVNGetMainWindowClass();

#endif
