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

#define kNewLayoutAction		"New::LayoutDocument"
#define kOpenLayoutAction		"Open::LayoutDocument"
#define kSaveLayoutAction		"Save::LayoutDocument"
#define kSaveLayoutAsAction		"SaveAs::LayoutDocument"
#define kSaveLayoutCopyAsAction	"SaveCopyAs::LayoutDocument"
#define kRevertLayoutsAction	"Revert::LayoutDocument"
#define kSaveAllLayoutsAction	"SaveAll::LayoutDocument"
#define kShowInFileMgrAction	"ShowInFileMgr::LayoutDocument"

#endif
