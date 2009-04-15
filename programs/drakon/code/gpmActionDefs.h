/******************************************************************************
 gpmActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright © 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_gpmActionDefs
#define _H_gpmActionDefs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jXActionDefs.h>	// for convenience

// Process menu

#define kGPMShowAllAction   "GPMShowAll"
#define kGPMPauseAction     "GPMPause"
#define kGPMContinueAction  "GPMContinue"
#define kGPMReNiceAction    "GPMReNice"
#define kGPMStopAction      "GPMStop"
#define kGPMKillAction      "GPMKill"

// Preferences menu

#define kGPMToolbarButtonsAction "GPMEditToolBar"

#endif
