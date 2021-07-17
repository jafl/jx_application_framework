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
 BuildInfo1 (private)

 ******************************************************************************/

void
JDirInfo::BuildInfo1
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

	struct dirent* data = nullptr;
	#if defined J_USE_READDIR_R || defined J_USE_POSIX_READDIR_R
	data = (struct dirent*) calloc(1, sizeof(struct dirent) + _POSIX_PATH_MAX);
	assert( data != nullptr );
	#endif

	struct dirent* direntry;
	#ifdef J_USE_POSIX_READDIR_R
	while (readdir_r(dir, data, &direntry) == 0 && direntry != nullptr)
	#elif defined J_USE_READDIR_R
	while ((direntry = readdir_r(dir, data)) != nullptr)
	#else
	while ((direntry = readdir(dir)) != nullptr)
	#endif
		{
		if (strcmp(direntry->d_name, "." ) == 0 ||
			strcmp(direntry->d_name, "..") == 0)
			{
			continue;
			}

		JDirEntry* newEntry = jnew JDirEntry(itsCWD, JString(direntry->d_name, JString::kNoCopy));
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

	free(data);
	closedir(dir);
}
