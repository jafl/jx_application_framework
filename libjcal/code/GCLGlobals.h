/******************************************************************************
 GCLGlobals.h

 *****************************************************************************/

#ifndef _H_GCLGlobals
#define _H_GCLGlobals

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jXGlobals.h>
#include <jTypes.h>

class GCLAlarmManager;

GCLAlarmManager*	GCLGetAlarmMgr();

	// called by TestApp

void	GCLCreateGlobals();
void	GCLDeleteGlobals();

#endif
