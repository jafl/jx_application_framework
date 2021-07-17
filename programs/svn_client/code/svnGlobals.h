/******************************************************************************
 svnGlobals.h

   Copyright (C) 2008 by John Lindal.

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

const JString&		SVNGetVersionNumberStr();
JString				SVNGetVersionStr();

	// called by SVNApp

bool	SVNCreateGlobals(SVNApp* app);
void		SVNDeleteGlobals();
void		SVNCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	SVNGetWMClassInstance();
const JUtf8Byte*	SVNGetMainWindowClass();

#endif
