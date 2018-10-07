/******************************************************************************
 thxGlobals.h

	Copyright (C) 1998 by John Lindal.

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

const JString&		THXGetVersionNumberStr();
JString				THXGetVersionStr();

	// called by THXApp

void THXCreateGlobals(THXApp* app);
void THXDeleteGlobals();

	// called by Directors

const JUtf8Byte*	THXGetWMClassInstance();
const JUtf8Byte*	THXGetExprWindowClass();
const JUtf8Byte*	THXGetPlotWindowClass();
const JUtf8Byte*	THXGetBaseConvWindowClass();
const JUtf8Byte*	THXGetVarWindowClass();

#endif
