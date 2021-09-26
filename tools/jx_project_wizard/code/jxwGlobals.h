/******************************************************************************
 jxwGlobals.h

   Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#ifndef _H_jxwGlobals
#define _H_jxwGlobals

// we include these for convenience

#include "JXWApp.h"
#include "JXWPrefsManager.h"
#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jx/jXGlobals.h>

class JXWMainDialog;

JXWApp*				JXWGetApplication();
JXWPrefsManager*	JXWGetPrefsManager();

const JUtf8Byte*	JXWGetVersionNumberStr();
JString				JXWGetVersionStr();

	// called by JXWApp

bool	JXWCreateGlobals(JXWApp* app);
void		JXWDeleteGlobals();
void		JXWCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by JXWMainDialog

void	JXWSetMainDialog(JXWMainDialog* dlog);

#endif
