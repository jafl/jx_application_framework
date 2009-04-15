/******************************************************************************
 JDirInfo_UNIX.cpp

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JDirInfo.h>
#include <JProgressDisplay.h>
#include <JStdError.h>
#include <jMountUtil.h>
#include <dirent.h>
#include <jAssert.h>

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
	JBoolean writable, isTop;
	JString device, fsType;

	const JBoolean ignoreExecPermFlag = JI2B(
		JIsMounted(*itsCWD, &writable, &isTop, &device, &fsType) &&
		JStringCompare(fsType, "vfat", kJFalse) == 0);
*/
	// scan directory

	DIR* dir = opendir(".");
	assert( dir != NULL );

	struct dirent* data = NULL;
	#if defined J_USE_READDIR_R || defined J_USE_POSIX_READDIR_R
	data = (struct dirent*) calloc(1, sizeof(struct dirent) + _POSIX_PATH_MAX);
	assert( data != NULL );
	#endif

	struct dirent* direntry;
	#ifdef J_USE_POSIX_READDIR_R
	while (readdir_r(dir, data, &direntry) == 0 && direntry != NULL)
	#elif defined J_USE_READDIR_R
	while ((direntry = readdir_r(dir, data)) != NULL)
	#else
	while ((direntry = readdir(dir)) != NULL)
	#endif
		{
		if (strcmp(direntry->d_name, "." ) == 0 ||
			strcmp(direntry->d_name, "..") == 0)
			{
			continue;
			}

		JDirEntry* newEntry = new JDirEntry(*itsCWD, direntry->d_name);
		assert( newEntry != NULL );
		if (MatchesContentFilter(*newEntry))
			{
			itsDirEntries->InsertSorted(newEntry, kJTrue);

/*			if (ignoreExecPermFlag)
				{
				newEntry->itsIsExecutableFlag  = kJFalse;
				newEntry->itsMode             &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
				}
*/			}
		else
			{
			delete newEntry;
			}

		if (!pg.IncrementProgress())
			{
			break;
			}
		}

	free(data);
	closedir(dir);
}
