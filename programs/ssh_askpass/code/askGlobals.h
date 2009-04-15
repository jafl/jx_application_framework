/******************************************************************************
 askGlobals.h

   Copyright © 2006 by New Planet Software, Inc.. All rights reserved.

 *****************************************************************************/

#ifndef _H_askGlobals
#define _H_askGlobals

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jXGlobals.h>

// we include these for convenience

#include "ASKApp.h"

class ASKMDIServer;

ASKApp*				ASKGetApplication();

const JCharacter*	ASKGetVersionNumberStr();
JString				ASKGetVersionStr();

	// called by ASKApp

void	ASKCreateGlobals(ASKApp* app);
void	ASKDeleteGlobals();

#endif
