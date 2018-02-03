/******************************************************************************
 SyGActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SyGActionDefs
#define _H_SyGActionDefs

#include <jXActionDefs.h>	// for convenience

// File menu

#define kSGNewFolderAction      "SGNewFolder"
#define kSGNewTextFileAction    "SGNewTextFile"
#define kSGOpenAction           "SGOpen"
#define kSGOpenAndCloseAction   "SGOpenAndClose"
#define kSGOpenAndIconifyAction "SGOpenAndIconify"
#define kSGAlternateOpenAction  "SGAlternateOpen"
#define kSGRunOnFileAction      "SGRunOnFile"
#define kSGFindAction           "SGFind"
#define kSGRenameAction         "SGRename"
#define kSGDuplicateAction      "SGDuplicate"
#define kSGMakeAliasAction      "SGMakeAlias"
#define kSGConvertToFile        "SGConvertToFile"
#define kSGConvertToProgram     "SGConvertToProgram"
#define kSGFindOriginalAction   "SGFindOriginal"
#define kSGMountAction          "SGMount"
#define kSGEraseAction          "SGErase"
#define kSGOpenHomeWindowAction "SGOpenHomeWindow"
#define kSGViewManPageAction    "SGViewManPage"
#define kSGOpenTerminalAction   "SGOpenTerminal"
#define kSGRunCommandAction     "SGRunCommand"

// Edit menu

#define kSGCopyWithPathAction "SGCopyWithPath"

// View menu

#define kSGShowFilterAction      "SGShowFilter"
#define kSGShowHiddenAction      "SGShowHidden"
#define kSGShowUserAction        "SGShowUser"
#define kSGShowGroupAction       "SGShowGroup"
#define kSGShowSizeAction        "SGShowSize"
#define kSGShowPermissionsAction "SGShowPermissions"
#define kSGShowDateAction        "SGShowDate"
#define kSGShowAllAction         "SGShowAll"
#define kSGShowNoneAction        "SGShowNone"
#define kSGRefreshAction         "SGRefresh"

// Git menu

#define kSGGitStatusAction       "SGGitStatus"
#define kSGGitHistoryAction      "SGGitHistory"
#define kSGGitCommitAllAction    "SGGitCommitAll"
#define kSGGitRevertAllAction    "SGGitRevertAll"
#define kSGGitStashChangesAction "SGGitStashChanges"
#define kSGGitCreateBranchAction "SGGitCreateBranch"
#define kSGGitAddRemoteAction    "SGGitAddRemote"
#define kSGGitPruneRemoteAction  "SGGitPruneRemote"

// Shortcuts menu

#define kSGAddShortcutAction        "SGAddShortcut"
#define kSGRemoveShortcutAction     "SGRemoveShortcut"
#define kSGRemoveAllShortcutsAction "SGRemoveAllShortcuts"

// Windows menu

#define kSGBringWindowsToFrontAction  "SGBringWindowsToFront"
#define kSGCloseAllOtherWindowsAction "SGCloseAllOtherWindows"

#endif
