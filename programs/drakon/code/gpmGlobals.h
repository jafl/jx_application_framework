/******************************************************************************
 gpmGlobals.h

   Copyright (C) 2001 by Glenn W. Bach.

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

const JString&		GPMGetVersionNumberStr();
JString				GPMGetVersionStr();

JBoolean			GPMGetSystemMemory(JSize* mem);

	// called by GPMApp

JBoolean	GPMCreateGlobals(GPMApp* app);
void		GPMDeleteGlobals();
void		GPMCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	GPMGetWMClassInstance();
const JUtf8Byte*	GPMGetMainWindowClass();

#endif
