/******************************************************************************
 JDirInfo_UNIX.cpp

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JDirInfo.h"
#include "jx-af/jcore/JProgressDisplay.h"
#include "jx-af/jcore/JStdError.h"
#include "jx-af/jcore/jMountUtil.h"
#include <dirent.h>
#include "jx-af/jcore/jAssert.h"

/*****************************************************************************
 CalledByBuildInfo (private)

 ******************************************************************************/

void
JDirInfo::CalledByBuildInfo
	(
	JProgressDisplay& pg
	)
{
	// compensate for file system deficiencies
	// (This won't work unless JDirEntry does it, which is too expensive)
/*
	bool writable, isTop;
	JString device, fsType;

	const bool ignoreExecPermFlag = JIsMounted(*itsCWD, &writable, &isTop, &device, &fsType) &&
		JString::Compare(fsType, "vfat", false) == 0;
*/
	// scan directory

	DIR* dir = opendir(".");
	assert( dir != nullptr );

	struct dirent* direntry;
	while ((direntry = readdir(dir)) != nullptr)
	{
		if (strcmp(direntry->d_name, "." ) == 0 ||
			strcmp(direntry->d_name, "..") == 0)
		{
			continue;
		}

		auto* newEntry = jnew JDirEntry(itsCWD, JString(direntry->d_name, JString::kNoCopy));
		assert( newEntry != nullptr );
		if (MatchesContentFilter(*newEntry))
		{
			itsDirEntries->InsertSorted(newEntry, true);

/*			if (ignoreExecPermFlag)
			{
				newEntry->itsIsExecutableFlag  = false;
				newEntry->itsMode             &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
			}
*/			}
		else
		{
			jdelete newEntry;
		}

		if (!pg.IncrementProgress())
		{
			break;
		}
	}

	closedir(dir);
}
