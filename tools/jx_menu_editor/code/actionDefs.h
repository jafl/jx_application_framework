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

#define kNewMenuAction			"New::MenuDocument"
#define kOpenMenuAction			"Open::MenuDocument"
#define kSaveMenuAction			"Save::MenuDocument"
#define kSaveMenuAsAction		"SaveAs::MenuDocument"
#define kSaveMenuCopyAsAction	"SaveCopyAs::MenuDocument"
#define kRevertMenusAction		"Revert::MenuDocument"
#define kSaveAllMenusAction		"SaveAll::MenuDocument"
#define kShowInFileMgrAction	"ShowInFileMgr::MenuDocument"

#define kNewItemAction			"New::MenuTable"
#define kDuplicateItemsAction	"Duplicate::MenuTable"
#define kImportMenuAction		"Import::MenuTable"

#endif
