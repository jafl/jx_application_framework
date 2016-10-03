/******************************************************************************
 wizGlobals.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_wizGlobals
#define _H_wizGlobals

// we include these for convenience

#include <jXGlobals.h>
#include "WizApp.h"
#include "Wiz2War.h"

WizApp*				WizGetApplication();
Wiz2War*			WizGetConnectionManager();

const JCharacter*	WizGetVersionNumberStr();
JString				WizGetVersionStr();

	// called by WizApp

void WizCreateGlobals(WizApp* app);
void WizDeleteGlobals();

	// called by Directors

const JCharacter*	WizGetWMClassInstance();
const JCharacter*	WizGetMapWindowClass();
const JCharacter*	WizGetChatWindowClass();

#endif
