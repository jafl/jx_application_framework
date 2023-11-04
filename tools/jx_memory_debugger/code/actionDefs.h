/******************************************************************************
 actionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_mdActionDefs
#define _H_mdActionDefs

#include <jx-af/jx/jXActionDefs.h>	// for convenience

#define kGetRecordsAction			"GetRecords::MDStatsDirector"
#define kSaveSnapshotAction			"SaveSnapshot::MDStatsDirector"
#define kDiffSnapshotAction			"DiffSnapshot::MDStatsDirector"
#define kShowAppRecordsAction		"ShowAppRecordsAction::MDStatsDirector"
#define kShowBucket1RecordsAction	"ShowBucket1RecordsAction::MDStatsDirector"
#define kShowBucket2RecordsAction	"ShowBucket2RecordsAction::MDStatsDirector"
#define kShowBucket3RecordsAction	"ShowBucket3RecordsAction::MDStatsDirector"
#define kShowLibraryRecordsAction	"ShowLibraryRecordsAction::MDStatsDirector"
#define kShowInternalRecordsAction	"ShowInternalRecordsAction::MDStatsDirector"
#define kShowUnknownRecordsAction	"ShowUnknownRecordsAction::MDStatsDirector"

#define kOpenFilesAction	"OpenFiles::MDRecordDirector"

#endif
