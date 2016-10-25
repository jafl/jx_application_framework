/******************************************************************************
 jDirUtil.cpp

	System independent directory utilities.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jDirUtil.h>
#include <jFileUtil.h>
#include <JDirInfo.h>
#include <JLatentPG.h>
#include <JString.h>
#include <limits.h>
#include <jAssert.h>

/******************************************************************************
 JIsRelativePath

 ******************************************************************************/

JBoolean
JIsRelativePath
	(
	const JString& path
	)
{
	return !JIsAbsolutePath(path);
}

/******************************************************************************
 JRenameDirectory

	Renames the specified directory.

 ******************************************************************************/

JError
JRenameDirectory
	(
	const JString& oldName,
	const JString& newName
	)
{
	return JRenameDirEntry(oldName, newName);
}

/******************************************************************************
 JGetUniqueDirEntryName

	*** Just like tmpnam, this function is not safe, because it does not
		perform atomic check-create.  The safe functions are
		JCreateTempDirectory() and JCreateTempFile().

	Returns a unique name, starting with namePrefix, for use in the specified
	directory.  If path is empty, it tries to use /tmp.  If this fails, it
	uses the current working directory.

	nameSuffix can be NULL.

	We ignore the possibility of not finding a valid name because the file
	system will fill up long before we run out of possibilities.

	startIndex lets you optimize when creating a large number of temporary
	files.  By setting startIndex to the number of files that you have already
	created, you make the process of generating the file names O(N) instead
	of O(N^2).

 ******************************************************************************/

JString
JGetUniqueDirEntryName
	(
	const JString&		path,
	const JString&		namePrefix,
	const JUtf8Byte*	nameSuffix,
	const JIndex		startIndex
	)
{
	assert( !namePrefix.IsEmpty() );

	JString fullPath;
	if (JString::IsEmpty(path))
		{
		if (!JGetTempDirectory(&fullPath))
			{
			fullPath = JGetCurrentDirectory();
			}
		}
	else
		{
		const JBoolean ok = JConvertToAbsolutePath(path, NULL, &fullPath);
		assert( ok );
		}
	assert( JDirectoryExists(fullPath.GetBytes()) );

	const JString prefix = JCombinePathAndName(fullPath.GetBytes(), namePrefix);

	JString name;
	for (JIndex i=startIndex; i<=kJIndexMax; i++)
		{
		name = prefix;
		if (i > 1)
			{
			name += JString(i, JString::kBase10);
			}
		if (!JString::IsEmpty(nameSuffix))
			{
			name += nameSuffix;
			}
		if (!JNameUsed(name.GetBytes()))
			{
			break;
			}
		}
	return name;
}

/******************************************************************************
 JGetPermissionsString

	Converts the low 9 bits of the st_mode field from stat() to a string
	using the same format as ls.

 *****************************************************************************/

JString
JGetPermissionsString
	(
	const mode_t mode
	)
{
	const JUtf8Byte* modeTemplate = "---------";

	JUtf8Byte* modeString = jnew JUtf8Byte[ strlen(modeTemplate)+1 ];
	assert( modeString != NULL );
	strcpy(modeString, modeTemplate);

	if (mode & S_IRUSR)
		{
		modeString[0] = 'r';
		}
	if (mode & S_IWUSR)
		{
		modeString[1] = 'w';
		}
	if (mode & S_IXUSR)
		{
		modeString[2] = 'x';
		}
	if (mode & S_IRGRP)
		{
		modeString[3] = 'r';
		}
	if (mode & S_IWGRP)
		{
		modeString[4] = 'w';
		}
	if (mode & S_IXGRP)
		{
		modeString[5] = 'x';
		}
	if (mode & S_IROTH)
		{
		modeString[6] = 'r';
		}
	if (mode & S_IWOTH)
		{
		modeString[7] = 'w';
		}
	if (mode & S_IXOTH)
		{
		modeString[8] = 'x';
		}

	return JString(modeString);
}

/*****************************************************************************
 JGetClosestDirectory

	If the directory does not exist, go as far down the
	directory tree as possible towards the specified directory.

	As an example, /usr/include/junk doesn't normally exist, so it will
	return /usr/include.

	If a partial path is passed in, it is assumed to be relative to the
	given basePath.  If this is empty, the current working directory is
	used instead.  If the base path is valid, a partial path will be
	returned.

	If the path begins with ~ and the home directory exists, a ~ path
	will be returned.

 ******************************************************************************/

JString
JGetClosestDirectory
	(
	const JString&	origDirName,
	const JBoolean	requireWrite,
	const JString*	basePath
	)
{
	assert( !JString::IsEmpty(origDirName) );

	JString workingDir;
	if (!JString::IsEmpty(basePath))
		{
		workingDir = basePath;
		JAppendDirSeparator(&workingDir);
		}
	else
		{
		workingDir = JGetCurrentDirectory();
		}

	JString dirName = origDirName;
	JString homeDir;
	JSize homeLength;
	if (origDirName[0] == '~' &&
		!JExpandHomeDirShortcut(origDirName, &dirName, &homeDir, &homeLength))
		{
		return JGetRootDirectory();
		}
	else if (JIsRelativePath(origDirName))
		{
		dirName.Prepend(workingDir);
		}

	assert( JIsAbsolutePath(dirName.GetBytes()) );

	JString newDir, junkName;
	while (!JDirectoryExists(dirName.GetBytes())   ||
		   !JCanEnterDirectory(dirName.GetBytes()) ||
		   !JDirectoryReadable(dirName.GetBytes()) ||
		   (requireWrite && !JDirectoryWritable(dirName.GetBytes())))
		{
		JStripTrailingDirSeparator(&dirName);
		if (JIsRootDirectory(dirName.GetBytes()))
			{
			break;
			}
		JSplitPathAndName(dirName.GetBytes(), &newDir, &junkName);
		dirName = newDir;
		}

	// convert back to partial path, if possible

	if (origDirName[0] == '~' &&
		dirName.BeginsWith(homeDir))
		{
		dirName.ReplaceSubstring(1, homeDir.GetLength(), origDirName, homeLength);
		}
	else if (JIsRelativePath(origDirName) &&
			 dirName.GetLength() > workingDir.GetLength() &&
			 dirName.BeginsWith(workingDir))
		{
		dirName.RemoveSubstring(1, workingDir.GetLength());
		}

	return dirName;
}

/******************************************************************************
 JSearchSubdirs

	Recursively search for the given file or directory, starting from the
	specified directory.  We do this in two passes:
		1)  Search in the given directory
		2)  Search in the sub-directories

	This mirrors the user's search strategy and helps insure that the
	expected file is found, if there are duplicates.

	caseSensitive is used only if name does not include a partial path,
	because it is just too messy otherwise.

	If newName is not NULL, it is set to the name of the file that was
	found.  This is critical if !caseSensitive, but useless otherwise.

	A progress display is used if the search takes more than 3 seconds.
	If you do not pass one in, JNewPG() will be used to create one.

 ******************************************************************************/

static JBoolean JSearchSubdirs_private(
	const JString& startPath, const JString& name,
	const JBoolean isFile, const JBoolean caseSensitive,
	JString* path, JString* newName,
	JProgressDisplay& pg, JBoolean* cancelled);

JBoolean
JSearchSubdirs
	(
	const JString&		startPath,
	const JString&		name,
	const JBoolean		isFile,
	const JBoolean		caseSensitive,
	JString*			path,
	JString*			newName,
	JProgressDisplay*	userPG,
	JBoolean*			userCancelled
	)
{
	assert( !JString::IsEmpty(startPath) );
	assert( !JString::IsEmpty(name) && name[0] != ACE_DIRECTORY_SEPARATOR_CHAR );

	JLatentPG pg(100);
	if (userPG != NULL)
		{
		pg.SetPG(userPG, kJFalse);
		}
	JString msg = "Searching for \"";
	msg += name;
	msg += "\"...";
	pg.VariableLengthProcessBeginning(msg, kJTrue, kJFalse);

	JBoolean cancelled = kJFalse;
	const JBoolean found =
		JSearchSubdirs_private(startPath, name, isFile, caseSensitive,
							   path, newName, pg, &cancelled);
	if (!found)
		{
		path->Clear();
		if (newName != NULL)
			{
			newName->Clear();
			}
		}

	pg.ProcessFinished();

	if (userCancelled != NULL)
		{
		*userCancelled = cancelled;
		}
	return found;
}

JBoolean
JSearchSubdirs_private
	(
	const JString&		startPath,
	const JString&		name,
	const JBoolean		isFile,
	const JBoolean		caseSensitive,
	JString*			path,
	JString*			newName,
	JProgressDisplay&	pg,
	JBoolean*			cancelled
	)
{
	// checking this way covers partial path cases like "X11/Xlib.h"

	const JString fullName = JCombinePathAndName(startPath, name);
	if (( isFile && JFileExists(fullName)) ||
		(!isFile && JDirectoryExists(fullName)))
		{
		const JBoolean ok = JGetTrueName(startPath, path);
		assert( ok );
		if (newName != NULL)
			{
			*newName = name;
			}
		return kJTrue;
		}

	JDirInfo* info;
	if (!JDirInfo::Create(startPath, &info))
		{
		return kJFalse;
		}

	JBoolean found    = kJFalse;
	const JSize count = info->GetEntryCount();

	// check each entry (if case sensitive, the initial check is enough)

	if (!caseSensitive)
		{
		for (JIndex i=1; i<=count; i++)
			{
			const JDirEntry& entry = info->GetEntry(i);

			if ((( isFile && entry.IsFile()) ||
				 (!isFile && entry.IsDirectory())) &&
				JStringCompare(name, entry.GetName(), caseSensitive) == 0)
				{
				const JBoolean ok = JGetTrueName(startPath, path);
				assert( ok );
				if (newName != NULL)
					{
					*newName = entry.GetName();
					}
				found = kJTrue;
				break;
				}

			if (!pg.IncrementProgress())
				{
				*cancelled = kJTrue;
				break;
				}
			}
		}

	// recurse on each directory

	if (!found && !(*cancelled))
		{
		for (JIndex i=1; i<=count; i++)
			{
			const JDirEntry& entry = info->GetEntry(i);

			if (entry.IsDirectory() && !entry.IsLink())
				{
				const JString& newPath = entry.GetFullName();
				if (JSearchSubdirs_private(newPath, name, isFile,
										   caseSensitive, path, newName,
										   pg, cancelled))
					{
					found = kJTrue;
					break;
					}
				}

			if (*cancelled || (caseSensitive && !pg.IncrementProgress()))
				{
				*cancelled = kJTrue;
				break;
				}
			}
		}

	jdelete info;
	return found;
}

/******************************************************************************
 JConvertToHomeDirShortcut

	If the directory is below the user's home directory, converts the given
	path to start with ~.  Otherwise, returns the input string.

 ******************************************************************************/

JString
JConvertToHomeDirShortcut
	(
	const JString& path
	)
{
	JString s = path;
	JCleanPath(&s);
	JAppendDirSeparator(&s);

	JString homeDir, trueDir;
	if (JGetHomeDirectory(&homeDir) && JGetTrueName(homeDir, &trueDir) &&
		s.BeginsWith(trueDir))
		{
		s.ReplaceSubstring(1, trueDir.GetLength(), "~" ACE_DIRECTORY_SEPARATOR_STR);
		}

	JStripTrailingDirSeparator(&s);
	return s;
}
