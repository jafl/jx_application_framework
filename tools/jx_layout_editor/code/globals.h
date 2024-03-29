/******************************************************************************
 globals.h

   Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_globals
#define _H_globals

#include <jx-af/jx/jXGlobals.h>

// we include these for convenience

#include "App.h"
#include "PrefsManager.h"
#include "DocumentManager.h"

class MDIServer;
class ImageCache;

App*				GetApplication();
DocumentManager*	GetDocumentManager();
PrefsManager*		GetPrefsManager();
void				ForgetPrefsManager();
MDIServer*			GetMDIServer();
ImageCache*			GetImageCache();

const JString&	GetVersionNumberStr();
JString			GetVersionStr();

	// called by App

bool	CreateGlobals(App* app);
void	DeleteGlobals();
void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

#endif
