/******************************************************************************
 scGlobals.h

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_scGlobals
#define _H_scGlobals

// we include these for convenience

#include <jXGlobals.h>
#include "SymcirApp.h"

class JSymbolicMath;

SymcirApp*		SCGetApplication();
JSymbolicMath*	SCGetSymbolicMath();

	// called by SymcirApp

void SCCreateGlobals(SymcirApp* app);
void SCDeleteGlobals();

#endif
