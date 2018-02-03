/******************************************************************************
 cmFileVersions.h

	Version information for unstructured data files

	Copyright (C) 2000-04 by John Lindal.

 ******************************************************************************/

#ifndef _H_cmFileVersions
#define _H_cmFileVersions

#include <jTypes.h>

const JFileVersion kCurrentPrefsFileVersion	= 10;

// version 10:
//	Added PHP suffixes.
// version 9:
//	Inserted kJDBCommandID.
// version 8:
//	Added kDebuggerTypeID.
// version 7:
//	Moved FileList prefs from CMPrefsManager to CMFileListDir.
// version 6:
//	Added Fortran and Java suffixes.
// version 5:
//	Removed ID = 34.
// version 4:
//	Converted %f and %l in edit commands to $f and $l.
// version 3:
//	Removed ID = 33.

const JFileVersion kCurrentConfigVersion = 3;

// version 3:
//	Stores memory windows.
// version 2:
//	Removed breakpoint commands.
// version 1:
//	Stores commands for each breakpoint.

#endif
