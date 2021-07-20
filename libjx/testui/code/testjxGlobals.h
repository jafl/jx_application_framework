/******************************************************************************
 testjxGlobals.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_testjxGlobals
#define _H_testjxGlobals

// we include these for convenience

#include <jXGlobals.h>
#include "TestApp.h"
#include "TestMDIServer.h"

TestApp*	TestjxGetApplication();
bool		TestjxGetMDIServer(TestMDIServer** server);

	// called by TestApp

void TestjxCreateGlobals(TestApp* app, const bool wantMDI);
void TestjxDeleteGlobals();

#endif
