/******************************************************************************
 svnMenus.h

	Copyright © 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_svnMenus
#define _H_svnMenus

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "svnActionDefs.h"

enum ActionMenu
{
	kRefreshCmd = 1,
	kCloseTabCmd,
	kUpdateWorkingCopyCmd,
	kCleanUpWorkingCopyCmd,
	kAddSelectedFilesCmd,
	kRemoveSelectedFilesCmd,
	kForceRemoveSelectedFilesCmd,
	kResolveSelectedConflictsCmd,
	kCommitSelectedChangesCmd,
	kCommitAllChangesCmd,
	kRevertSelectedChangesCmd,
	kRevertAllChangesCmd,
	kCreateDirectoryCmd,
	kDuplicateSelectedItemCmd,
	kCreatePropertyCmd,
	kRemoveSelectedPropertiesCmd,
	kIgnoreSelectionCmd
};

enum InfoMenu
{
	kInfoLogSelectedFilesCmd = 1,
	kPropSelectedFilesCmd,
	kDiffEditedSelectedFilesCmd,
	kDiffCurrentSelectedFilesCmd,
	kDiffPrevSelectedFilesCmd,
	kCommitDetailsCmd,
	kBrowseRepoRevisionCmd,
	kBrowseSelectedRepoRevisionCmd
};

#endif
