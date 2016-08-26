/******************************************************************************
 thxGlobals.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_thxGlobals
#define _H_thxGlobals

// we include these for convenience

#include <jXGlobals.h>
#include "THXApp.h"

class THXMDIServer;
class THXPrefsManager;
class JXPTPrinter;
class JXPSPrinter;
class JX2DPlotEPSPrinter;

THXApp*				THXGetApplication();
THXMDIServer*		THXGetMDIServer();
THXPrefsManager*	THXGetPrefsManager();
JXPTPrinter*		THXGetTapePrinter();
JXPSPrinter*		THXGetPSGraphPrinter();
JX2DPlotEPSPrinter*	THXGetEPSGraphPrinter();

const JCharacter*	THXGetVersionNumberStr();
JString				THXGetVersionStr();

	// called by THXApp

void THXCreateGlobals(THXApp* app);
void THXDeleteGlobals();

	// called by Directors

const JCharacter*	THXGetWMClassInstance();
const JCharacter*	THXGetExprWindowClass();
const JCharacter*	THXGetPlotWindowClass();
const JCharacter*	THXGetBaseConvWindowClass();
const JCharacter*	THXGetVarWindowClass();

#endif
