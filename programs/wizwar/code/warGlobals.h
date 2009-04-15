/******************************************************************************
 warGlobals.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_warGlobals
#define _H_warGlobals

// we include these for convenience

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "WarSetup.h"
#include "War2Wiz.h"

WarSetup*	WarGetSetup();
War2Wiz*	WarGetConnectionManager();

	// called by main()

void WarCreateGlobals(const JSize argc, char* argv[], const JCharacter* versionStr);
void WarDeleteGlobals();

#endif
