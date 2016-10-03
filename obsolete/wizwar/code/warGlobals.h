/******************************************************************************
 warGlobals.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_warGlobals
#define _H_warGlobals

// we include these for convenience

#include "WarSetup.h"
#include "War2Wiz.h"

WarSetup*	WarGetSetup();
War2Wiz*	WarGetConnectionManager();

	// called by main()

void WarCreateGlobals(const JSize argc, char* argv[], const JCharacter* versionStr);
void WarDeleteGlobals();

#endif
