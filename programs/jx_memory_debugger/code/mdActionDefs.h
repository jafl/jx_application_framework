/******************************************************************************
 mdActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_mdActionDefs
#define _H_mdActionDefs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jXActionDefs.h>	// for convenience

#define kMDGetRecords		"GetRecords::MDStatsDirector"

#define kMDOpenFilesAction	"OpenFiles::MDRecordDirector"

#endif
