/******************************************************************************
 askGlobals.h

   Copyright (C) 2006 by John Lindal.

 *****************************************************************************/

#ifndef _H_askGlobals
#define _H_askGlobals

#include <jXGlobals.h>

// we include these for convenience

#include "ASKApp.h"

class ASKMDIServer;

ASKApp*			ASKGetApplication();

const JString&	ASKGetVersionNumberStr();
JString			ASKGetVersionStr();

	// called by ASKApp

void	ASKCreateGlobals(ASKApp* app);
void	ASKDeleteGlobals();

#endif
