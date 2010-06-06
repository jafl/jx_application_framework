/******************************************************************************
 gpmGlobals.h

   Copyright © 2001 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_gpmGlobals
#define _H_gpmGlobals

// we include these for convenience

#include "GPMApp.h"
#include "GPMPrefsManager.h"
#include <JXDocumentManager.h>
#include <jXGlobals.h>

class GPMMDIServer;

GPMApp*				GPMGetApplication();
GPMPrefsManager*	GPMGetPrefsManager();
GPMMDIServer*		GPMGetMDIServer();

const JCharacter*	GPMGetVersionNumberStr();
JString				GPMGetVersionStr();

JBoolean			GPMGetSystemMemory(JSize* mem);

	// called by GPMApp

JBoolean	GPMCreateGlobals(GPMApp* app);
void		GPMDeleteGlobals();
void		GPMCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JCharacter*	GPMGetWMClassInstance();
const JCharacter*	GPMGetMainWindowClass();

#endif
