/******************************************************************************
 jDirUtil.cpp

	System independent directory utilities.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jDirUtil.h"
#include "jFileUtil.h"
#include "JDirInfo.h"
#include "JLatentPG.h"
#include "JStringIterator.h"
#include "jGlobals.h"
#include <limits.h>
#include "jAssert.h"

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

	nameSuffix can be nullptr.

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
	if (path.IsEmpty())
		{
		if (!JGetTempDirectory(&fullPath))
			{
			fullPath = JGetCurrentDirectory();
			}
		}
	else
		{
		const JBoolean ok = JConvertToAbsolutePath(path, nullptr, &fullPath);
		assert( ok );
		}
	assert( JDirectoryExists(fullPath) );

	const JString prefix = JCombinePathAndName(fullPath, namePrefix);

	JString name;
	for (JIndex i=startIndex; i<=kJIndexMax; i++)
		{
		name = prefix;
		if (i > 1)
			{
			name += JString((JUInt64) i);
			}
		if (!JString::IsEmpty(nameSuffix))
			{
			name += nameSuffix;
			}
		if (!JNameUsed(name))
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
	assert( modeString != nullptr );
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

	return JString(modeString, 0);
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
	assert( !origDirName.IsEmpty() );

	JString workingDir;
	if (!JString::IsEmpty(basePath))
		{
		workingDir = *basePath;
		JAppendDirSeparator(&workingDir);
		}
	else
		{
		workingDir = JGetCurrentDirectory();
		}

	JString dirName = origDirName;
	JString homeDir;
	JSize homeLength;
	if (origDirName.GetFirstCharacter() == '~' &&
		!JExpandHomeDirShortcut(origDirName, &dirName, &homeDir, &homeLength))
		{
		return JGetRootDirectory();
		}
	else if (JIsRelativePath(origDirName))
		{
		dirName.Prepend(workingDir);
		}

	assert( JIsAbsolutePath(dirName) );

	JString newDir, junkName;
	while (!JDirectoryExists(dirName)   ||
		   !JCanEnterDirectory(dirName) ||
		   !JDirectoryReadable(dirName) ||
		   (requireWrite && !JDirectoryWritable(dirName)))
		{
		JStripTrailingDirSeparator(&dirName);
		if (JIsRootDirectory(dirName))
			{
			break;
			}
		JSplitPathAndName(dirName, &newDir, &junkName);
		dirName = newDir;
		}

	// convert back to partial path, if possible

	if (origDirName.GetFirstCharacter() == '~' &&
		dirName.BeginsWith(homeDir))
		{
		JStringIterator iter(&dirName);
		const JBoolean found = iter.Next(homeDir);
		assert( found );
		iter.ReplaceLastMatch(origDirName, JCharacterRange(1, homeLength));
		}
	else if (JIsRelativePath(origDirName) &&
			 dirName.GetCharacterCount() > workingDir.GetCharacterCount() &&
			 dirName.BeginsWith(workingDir))
		{
		JStringIterator iter(&dirName);
		const JBoolean found = iter.Next(workingDir);
		assert( found );
		iter.RemoveLastMatch();
		}

	return dirName;
}

/******************************************************************************
 JSearchSubdirs

	Recursively search for the given file or directory, starting from the
	specified directory.  We do this in two passes:
		1)  Search in the given directory
		2)  Search in the sub-directories

	This mirrors the user's search strategy and helps ensure that the
	expected file is found, if there are duplicates.

	caseSensitive is used only if name does not include a partial path,
	because it is just too messy otherwise.

	If newName is not nullptr, it is set to the name of the file that was
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
	assert( !startPath.IsEmpty() );
	assert( !name.IsEmpty() && name.GetFirstCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR );

	JLatentPG pg(100);
	if (userPG != nullptr)
		{
		pg.SetPG(userPG, kJFalse);
		}
	const JUtf8Byte* map[] =
		{
		"name", name.GetBytes()
		};
	const JString msg = JGetString("Searching::jDirUtil", map, sizeof(map));
	pg.VariableLengthProcessBeginning(msg, kJTrue, kJFalse);

	JBoolean cancelled = kJFalse;
	const JBoolean found =
		JSearchSubdirs_private(startPath, name, isFile, caseSensitive,
							   path, newName, pg, &cancelled);
	if (!found)
		{
		path->Clear();
		if (newName != nullptr)
			{
			newName->Clear();
			}
		}

	pg.ProcessFinished();

	if (userCancelled != nullptr)
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
		if (newName != nullptr)
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

	JBoolean found = kJFalse;

	// check each entry (if case sensitive, the initial check is enough)

	if (!caseSensitive)
		{
		for (const JDirEntry* entry : *info)
			{
			if ((( isFile && entry->IsFile()) ||
				 (!isFile && entry->IsDirectory())) &&
				JString::Compare(name, entry->GetName(), caseSensitive) == 0)
				{
				const JBoolean ok = JGetTrueName(startPath, path);
				assert( ok );
				if (newName != nullptr)
					{
					*newName = entry->GetName();
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
		for (const JDirEntry* entry : *info)
			{
			if (entry->IsDirectory() && !entry->IsLink())
				{
				const JString& newPath = entry->GetFullName();
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
		JStringIterator iter(&s);
		const JBoolean found = iter.Next(trueDir);
		assert( found );
		iter.ReplaceLastMatch("~" ACE_DIRECTORY_SEPARATOR_STR);
		}

	JStripTrailingDirSeparator(&s);
	return s;
}
