/******************************************************************************
 GCLGlobals.h

 *****************************************************************************/

#ifndef _H_GCLGlobals
#define _H_GCLGlobals

#include <jXGlobals.h>
#include <jTypes.h>

class GCLAlarmManager;

GCLAlarmManager*	GCLGetAlarmMgr();

	// called by TestApp

void	GCLCreateGlobals();
void	GCLDeleteGlobals();

#endif
