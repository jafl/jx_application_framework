/******************************************************************************
 actionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_actionDefs
#define _H_actionDefs

#include <jx-af/jx/jXActionDefs.h>	// for convenience

#define kSaveLayoutAction		"Save::LayoutDirector"
#define kSaveLayoutAsAction		"SaveAs::LayoutDirector"
#define kRevertLayoutsAction	"Revert::LayoutDirector"

#define kNewLayoutAction		"NewLayout::LayoutList"

#define kEditLayoutNameAction	"EditName::LayoutDirector"

#endif
