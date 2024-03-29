/******************************************************************************
 JDirInfo_Win32.cpp

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include <JDirInfo.h>
#include <JProgressDisplay.h>
#include <windows.h>
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
	WIN32_FIND_DATA findData;
	HANDLE h = FindFirstFile("*", &findData);
	if (h == INVALID_HANDLE_VALUE)
		{
		return;
		}

	JString fullName;
	do
		{
		if ((findData.dwFileAttributes &
			(FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_TEMPORARY)) ||
			strcmp(findData.cFileName, "." ) == 0 ||
			strcmp(findData.cFileName, "..") == 0)
			{
			continue;
			}

		JDirEntry* newEntry = new JDirEntry(*itsCWD, findData.cFileName);
		assert( newEntry != nullptr );
		if (MatchesContentFilter(*newEntry))
			{
			itsDirEntries->InsertSorted(newEntry, true);
			}
		else
			{
			delete newEntry;
			}

		if (!pg.IncrementProgress())
			{
			break;
			}
		}
		while (FindNextFile(h, &findData));

	FindClose(h);
}
