/******************************************************************************
 JDirInfo_UNIX.cpp

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include "JDirInfo.h"
#include "JProgressDisplay.h"
#include "JStdError.h"
#include "jMountUtil.h"
#include <dirent.h>
#include "jAssert.h"

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
	if (dir == nullptr)		// snaps are sandboxed
	{
		return;
	}

	struct dirent* direntry;
	while ((direntry = readdir(dir)) != nullptr)
	{
		if (strcmp(direntry->d_name, "." ) == 0 ||
			strcmp(direntry->d_name, "..") == 0)
		{
			continue;
		}

		auto* newEntry = jnew JDirEntry(itsCWD, JString(direntry->d_name, JString::kNoCopy));
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
