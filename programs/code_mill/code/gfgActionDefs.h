/******************************************************************************
 gfgActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright © 2002 by New Planet Software. All rights reserved.

 ******************************************************************************/

#ifndef _H_gfgActionDefs
#define _H_gfgActionDefs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jXActionDefs.h>	// for convenience

// Preferences menu

#define kGFGEditPreferencesAction "EditPreferences::GFGMainDirector"
#define kGFGEditToolBarAction     "EditToolBar::GFGMainDirector"

#endif
