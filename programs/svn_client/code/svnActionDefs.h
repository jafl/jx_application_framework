/******************************************************************************
 svnActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_svnActionDefs
#define _H_svnActionDefs

#include <jXActionDefs.h>	// for convenience

// File menu

#define kSVNOpenDirectoryAction       "OpenDirectory::SVNMainDirector"
#define kSVNBrowseRepoAction          "BrowseRepo::SVNMainDirector"
#define kSVNCheckOutRepoAction        "CheckOutRepo::SVNMainDirector"
#define kSVNCheckOutCurrentRepoAction "CheckOutCurrentRepo::SVNMainDirector"
#define kSVNCheckOutSelectionAction   "CheckOutSelection::SVNMainDirector"
#define kSVNOpenFilesAction           "OpenFiles::SVNMainDirector"
#define kSVNShowFilesAction           "ShowFiles::SVNMainDirector"

// Edit menu

#define kSVNCopyFullPathAction "CopyFullPath::SVNMainDirector"

// Actions menu

#define kSVNRefreshStatusAction            "RefreshStatus::SVNMainDirector"
#define kSVNCloseTabAction                 "CloseTab::SVNMainDirector"
#define kSVNUpdateWorkingCopyAction        "UpdateWorkingCopy::SVNMainDirector"
#define kSVNCleanUpWorkingCopyAction       "CleanUpWorkingCopy::SVNMainDirector"
#define kSVNAddFilesAction                 "AddFiles:SVNMainDirector"
#define kSVNRemoveFilesAction              "RemoveFiles:SVNMainDirector"
#define kSVNForceRemoveFilesAction         "ForceRemoveFiles:SVNMainDirector"
#define kSNVResolveSelectedConflictsAction "ResolveSelectedConflicts::SVNMainDirector"
#define kSNVCommitSelectedFilesAction      "CommitSelectedFiles::SVNMainDirector"
#define kSVNCommitAllFilesAction           "CommitAllFiles::SVNMainDirector"
#define kSVNRevertSelectedFilesAction      "RevertSelectedFiles::SVNMainDirector"
#define kSVNRevertAllFilesAction           "RevertAllFiles::SVNMainDirector"
#define kSVNCreateDirectoryAction          "CreateDirectory::SVNMainDirector"
#define kSVNDuplicateSelectedItemAction    "DuplicateSelectedItem::SVNMainDirector"
#define kSVNCreatePropertyAction           "CreateProperty::SVNMainDirector"
#define kSVNRemovePropertiesAction         "RemoveProperties::SVNMainDirector"
#define kSVNIgnoreSelectionAction          "IgnoreSelection::SVNMainDirector"

// Info menu

#define kSVNInfoLogSelectedFilesAction     "InfoLogSelectedFiles::SVNMainDirector"
#define kSVNPropSelectedFilesAction        "PropSelectedFiles::SVNMainDirector"
#define kSVNDiffEditedSelectedFilesAction  "DiffEditedSelectedFiles::SVNMainDirector"
#define kSVNDiffCurrentSelectedFilesAction "DiffCurrentSelectedFiles::SVNMainDirector"
#define kSVNDiffPrevSelectedFilesAction    "DiffPrevSelectedFiles::SVNMainDirector"
#define kSVNCommitDetailsAction            "CommitDetails::SVNMainDirector"
#define kSVNBrowseRevisionAction           "BrowseRevision::SVNMainDirector"
#define kSVNBrowseSelectedRevisionAction   "BrowseSelectedRevision::SVNMainDirector"

#endif
