/******************************************************************************
 globals.h

   Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#ifndef _H_jxwGlobals
#define _H_jxwGlobals

// we include these for convenience

#include "App.h"
#include "PrefsManager.h"
#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jx/jXGlobals.h>

class MainDialog;

App*			GetApplication();
PrefsManager*	GetPrefsManager();
void			ForgetPrefsManager();

const JUtf8Byte*	GetVersionNumberStr();
JString				GetVersionStr();

	// called by App

bool	CreateGlobals(App* app);
void	DeleteGlobals();
void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by MainDialog

void	SetMainDialog(MainDialog* dlog);

#endif
