/******************************************************************************
 jxwGlobals.h

   Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#ifndef _H_jxwGlobals
#define _H_jxwGlobals

// we include these for convenience

#include "JXWApp.h"
#include "JXWPrefsManager.h"
#include <JXDocumentManager.h>
#include <jXGlobals.h>

class JXWMainDialog;

JXWApp*				JXWGetApplication();
JXWPrefsManager*	JXWGetPrefsManager();

const JCharacter*	JXWGetVersionNumberStr();
JString				JXWGetVersionStr();

	// called by JXWApp

JBoolean	JXWCreateGlobals(JXWApp* app);
void		JXWDeleteGlobals();
void		JXWCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by JXWMainDialog

void	JXWSetMainDialog(JXWMainDialog* dlog);

#endif
