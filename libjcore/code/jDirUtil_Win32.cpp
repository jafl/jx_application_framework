/******************************************************************************
 jDirUtil_Win32.cpp

	Directory utilities implemented for the Windows System.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jDirUtil.h>
#include <JDirInfo.h>
#include <JString.h>
#include <sys/stat.h>
#include <io.h>
#include <shlobj.h>
#include <jSysUtil.h>
#include <jErrno.h>
#include <jMissingProto.h>
#include <jAssert.h>

/******************************************************************************
 JNameUsed

	Returns kJTrue if the specified name exists. (file, directory, link, etc).

 ******************************************************************************/

JBoolean
JNameUsed
	(
	const JCharacter* name
	)
{
	JString s = name;
	JStripTrailingDirSeparator(&s);

	ACE_stat info;
	return JI2B( ACE_OS::stat(s, &info) == 0 );
}

/******************************************************************************
 JSameDirEntry

	Returns kJTrue if the given names point to the same inode in the
	file system.

 ******************************************************************************/

JBoolean
JSameDirEntry
	(
	const JCharacter* name1,
	const JCharacter* name2
	)
{
	if(_stricmp(name1, name2) == 0)
		{
		return kJTrue;
		}

	JCharacter buf1[ _MAX_PATH+1 ], buf2[ _MAX_PATH+1 ];
	if (GetShortPathName(name1, buf1, _MAX_PATH) == 0 ||
		GetShortPathName(name2, buf2, _MAX_PATH) == 0)
		{
		return kJFalse;
		}
	buf1[ _MAX_PATH ] = '\0';	// paranoia
	buf2[ _MAX_PATH ] = '\0';

	return JI2B( _stricmp(buf1, buf2) == 0 );
}

/******************************************************************************
 JGetModificationTime

	Returns the last time that the file was modified.
	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

JError
JGetModificationTime
	(
	const JCharacter*	name,
	time_t*				modTime
	)
{
	JString s = name;
	JStripTrailingDirSeparator(&s);

	ACE_stat info;
	if (ACE_OS::stat(s, &info) == 0)
		{
		*modTime = info.st_mtime;
		return JNoError();
		}

	*modTime = 0;

	const int err = jerrno();
	if (err == ENOENT)
		{
		return JDirEntryDoesNotExist(name);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JGetPermissions

	Returns the access permissions for the specified file.
	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

JError
JGetPermissions
	(
	const JCharacter*	name,
	mode_t*				perms
	)
{
	JString s = name;
	JStripTrailingDirSeparator(&s);

	ACE_stat info;
	if (ACE_OS::stat(s, &info) == 0)
		{
		*perms &= 0x0F00;
		return JNoError();
		}

	*perms = 0;

	const int err = jerrno();
	if (err == ENOENT)
		{
		return JDirEntryDoesNotExist(name);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JSetPermissions

	Sets the access permissions for the specified file.

	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

JError
JSetPermissions
	(
	const JCharacter*	name,
	const mode_t		perms
	)
{
	int pmode = 0;
	if (perms & 0400)
		{
		pmode |= _S_IREAD;
		}
	if (perms & 0200)
		{
		pmode |= _S_IWRITE;
		}

	jclear_errno();
	if (_chmod(name, pmode) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == ENOENT)
		{
		return JDirEntryDoesNotExist(name);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/*******************************************************************************
 JCreateSymbolicLink

 *******************************************************************************/

JError
JCreateSymbolicLink
	(
	const JCharacter* origSrc,
	const JCharacter* origDest
	)
{
	if (FAILED(OleInitialize(NULL)))
		{
		return JAccessDenied("OLE");
		}

	JBoolean success = kJFalse;

	IShellLink* link;
	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
								   IID_IShellLink, (void**) &link)))
		{
		IPersistFile* file;
		if (SUCCEEDED(link->QueryInterface(IID_IPersistFile, (void**) &file)))
			{
			JString src = origSrc;
			JStripTrailingDirSeparator(&src);
			link->SetPath(src);

			JString path, name;
			JSplitPathAndName(src, &path, &name);
			link->SetWorkingDirectory(path);

			JString dest = origDest;
			if (!dest.EndsWith(".lnk", kJFalse))
				{
				dest += ".lnk";
				}

			WCHAR s[ MAX_PATH ];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, s, MAX_PATH);
			success = JI2B(SUCCEEDED(file->Save(s, TRUE)));

			file->Release();
			}
		link->Release();
		}

	if (success)
		{
		return JNoError();
		}
	else
		{
		return JUnknownError(errno);
		}
}

/******************************************************************************
 JDirectoryExists

	Returns kJTrue if the specified directory exists.

 ******************************************************************************/

JBoolean
JDirectoryExists
	(
	const JCharacter* dirName
	)
{
	JString s = dirName;
	JStripTrailingDirSeparator(&s);

	ACE_stat info;
	return JI2B(ACE_OS::stat( s, &info) == 0 &&
				S_ISDIR(info.st_mode) );
}

/******************************************************************************
 JDirectoryReadable

	Returns kJTrue if the specified directory can be read from.

 ******************************************************************************/

JBoolean
JDirectoryReadable
	(
	const JCharacter* dirName
	)
{
	return JI2B(JDirectoryExists(dirName) &&
				(JUserIsAdmin() || _access(dirName, R_OK) == 0));
}

/******************************************************************************
 JDirectoryWritable

	Returns kJTrue if the specified directory can be written to.

 ******************************************************************************/

JBoolean
JDirectoryWritable
	(
	const JCharacter* dirName
	)
{
	return JI2B(JDirectoryExists(dirName) &&
				(JUserIsAdmin() || _access(dirName, W_OK) == 0));
}

/******************************************************************************
 JCanEnterDirectory

	Returns kJTrue if it is possible to make the specified directory
	the working directory.

 ******************************************************************************/

JBoolean
JCanEnterDirectory
	(
	const JCharacter* dirName
	)
{
	return kJTrue;
}

/******************************************************************************
 JCreateDirectory

	Creates the requested directory.  Equivalent to "mkdir -p".

	These are two separate functions because the default mode is
	system dependent, so it can't be included in jDirUtil.h.

	Can return JBadPath, JDirEntryAlreadyExists.

 ******************************************************************************/

static JError JMkDir(const JCharacter* dirName, const mode_t mode);

JError
JCreateDirectory
	(
	const JCharacter* dirName
	)
{
	return JCreateDirectory(dirName, 0755);
}

JError
JCreateDirectory
	(
	const JCharacter*	dirName,
	const mode_t		mode
	)
{
	if (JDirectoryExists(dirName))
		{
		return JSetPermissions(dirName, mode);
		}

	JString path = dirName;
	JCleanPath(&path);
	JAppendDirSeparator(&path);

	JString dir;
	JIndex slashIndex = 2;
	while (path.LocateNextSubstring("\\", &slashIndex))
		{
		dir = path.GetSubstring(1, slashIndex);
		if (dir.GetLastCharacter() != ':' &&
			!JDirectoryExists(dir))
			{
			const JError err = JMkDir(dir, mode);
			if (!err.OK())
				{
				return err;
				}
			}
		slashIndex++;	// move past the slash we found
		}

	return JNoError();
}

// static

JError
JMkDir
	(
	const JCharacter*	dirName,
	const mode_t		mode
	)
{
	jclear_errno();
	if (_mkdir(dirName) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EEXIST)
		{
		return JDirEntryAlreadyExists(dirName);
		}
	else if (err == ENOENT)
		{
		return JBadPath(dirName);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JRenameDirEntry

	Renames the specified directory entry.

	Can return JDirectoryCantBeOwnChild, JAccessDenied, JBadPath.

 ******************************************************************************/

JError
JRenameDirEntry
	(
	const JCharacter* oldName,
	const JCharacter* newName
	)
{
	if (JNameUsed(newName))
		{
		return JDirEntryAlreadyExists(newName);
		}
	else if (JSameDirEntry(oldName, newName))
		{
		return JNoError();
		}

	jclear_errno();
	if (rename(oldName, newName) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EACCES)
		{
		return JAccessDenied(oldName, newName);
		}
	else if (err == ENOENT)
		{
		return JBadPath(oldName, newName);
		}
	else if (err == EINVAL)
		{
		return JDirectoryCantBeOwnChild();
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JChangeDirectory

	Changes the current working directory to the specified directory.

	Can return JAccessDenied, JSegFault, JNameTooLong, JBadPath,
	JNoKernelMemory, JComponentNotDirectory, JPathContainsLoop.

 ******************************************************************************/

JError
JChangeDirectory
	(
	const JCharacter* dirName
	)
{
	jclear_errno();
	if (_chdir(dirName) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == ENOENT)
		{
		return JBadPath(dirName);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JRemoveDirectory

	Removes the specified directory.  This only works if the directory is empty.

	Can return JDirectoryNotEmpty, JBadPath.

 ******************************************************************************/

JError
JRemoveDirectory
	(
	const JCharacter* dirName
	)
{
	jclear_errno();
	if (_rmdir(dirName) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == ENOTEMPTY)
		{
		return JDirectoryNotEmpty(dirName);
		}
	else if (err == ENOENT)
		{
		return JBadPath(dirName);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JKillDirectory

	Deletes the directory and everything in it.
	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JKillDirectory
	(
	const JCharacter* dirName
	)
{
	JDirInfo* info;
	if (!JDirInfo::Create(dirName, &info))
		{
		return kJFalse;
		}

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<count; i++)
		{
		const JDirEntry& entry = info->GetEntry(i);
		if (entry.IsDirectory())
			{
			if (!JKillDirectory(entry.GetFullName()))
				{
				return kJFalse;
				}
			}
		else
			{
			JKillFile(entry.GetFullName());
			}
		}

	SetFileAttributes(dirName, FILE_ATTRIBUTE_NORMAL);
	_chmod(dirName, _S_IREAD | _S_IWRITE);
	return (JRemoveDirectory(dirName)).OK();
}

/******************************************************************************
 JGetCurrentDirectory

	Returns the full path of the current working directory.

 ******************************************************************************/

JString
JGetCurrentDirectory()
{
	JCharacter buf[ _MAX_PATH+1 ];
	JCharacter* result = _getcwd(buf, _MAX_PATH);
	buf[ _MAX_PATH ] = '\0';	// paranoia

	JString dirName;
	if (result == buf)
		{
		dirName = buf;
		}
	else if (!JGetHomeDirectory(&dirName) || !(JChangeDirectory(dirName)).OK())
		{
		dirName = JGetRootDirectory();
		JChangeDirectory(dirName);
		}

	JAppendDirSeparator(&dirName);
	return dirName;
}

/******************************************************************************
 JGetHomeDirectory

 ******************************************************************************/

JBoolean
JGetHomeDirectory
	(
	JString* homeDir
	)
{
	return JGetDirectoryFromCSIDL(CSIDL_PERSONAL, homeDir);
}

/******************************************************************************
 JGetPrefsDirectory

 ******************************************************************************/

JBoolean
JGetPrefsDirectory
	(
	JString* prefsDir
	)
{
	if (JGetDirectoryFromCSIDL(CSIDL_APPDATA, prefsDir))
		{
		*prefsDir = JCombinePathAndName(*prefsDir, "JX");
		JAppendDirSeparator(prefsDir);
		JCreateDirectory(*prefsDir);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 JGetTempDirectory

 ******************************************************************************/

JBoolean
JGetTempDirectory
	(
	JString* tempDir
	)
{
	JCharacter buf[ MAX_PATH+1 ];
	const DWORD result = GetTempPath(MAX_PATH, buf);
	if (result > 0)
		{
		tempDir->Set(buf, result);
		return kJTrue;
		}
	else
		{
		tempDir->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 JGetTrueName

	Returns kJTrue if name is a valid file or a valid directory.
	*trueName is the full, true path to name, without symbolic links.

 ******************************************************************************/

JBoolean
JGetTrueName
	(
	const JCharacter*	name,
	JString*			trueName
	)
{
	trueName->Clear();

	JString target;
	if (!JNameUsed(name))
		{
		return kJFalse;
		}
	else if ((JGetSymbolicLinkTarget(name, &target)).OK())
		{
		return JGetTrueName(target, trueName);
		}
	else
		{
		*trueName = name;
		return kJTrue;
		}
}

/******************************************************************************
 JCleanPath

	Removes fluff from the given path:

		\\
		\.\
		trailing \.

	We can't remove \x\..\ because if x is a symlink, the result would not
	be the same directory.

	This is required to work for files and directories.

 ******************************************************************************/

void
JCleanPath
	(
	JString* path
	)
{
	while (path->EndsWith("\\."))
		{
		path->RemoveSubstring(path->GetLength(), path->GetLength());
		}

	JIndex i;
	while (path->LocateSubstring("\\.\\", &i))
		{
		path->RemoveSubstring(i, i+1);
		}

	while (path->LocateSubstring("\\\\", &i))
		{
		path->RemoveSubstring(i, i);
		}
}

/******************************************************************************
 JIsRelativePath

 ******************************************************************************/

JBoolean
JIsRelativePath
	(
	const JCharacter* path
	)
{
	assert( !JStringEmpty(path) );
	return JI2B( path[1] != ':' && path[0] != '~' &&
				 !(path[0] == '\\' && path[1] == '\\'));
}

/******************************************************************************
 JGetRootDirectory

 ******************************************************************************/

JString
JGetRootDirectory()
{
	JString path;
	const JBoolean ok = JGetDirectoryFromCSIDL(CSIDL_WINDOWS, &path);
	assert( ok );

	JIndex i;
	if (path.LocateSubstring("\\", &i) && i < path.GetLength())
		{
		path.RemoveSubstring(i+1, path.GetLength());
		}
	return path;
}

/******************************************************************************
 JIsRootDirectory

 ******************************************************************************/

JBoolean
JIsRootDirectory
	(
	const JCharacter* dirName
	)
{
	return JI2B( strlen(dirName) == 3 && dirName[1] == ':' );
}

/*****************************************************************************
 JConvertToAbsolutePath

	Attempts to convert 'path' to a full path.  Returns kJTrue if successful.

	If path begins with '/', there is nothing to do.
	If path begins with '~', the user's home directory is inserted.
	Otherwise, if base is not NULL and not empty, it is prepended.
	Otherwise, the result of JGetCurrentDirectory() is prepended.

	As a final check, it calls JNameUsed() to check that the result exists.
	(This allows one to pass in a path+name as well as only a path.)

	If it is possible to expand the path, *result will contain the expanded
	path, even if JNameUsed() fails

 ******************************************************************************/

JBoolean
JConvertToAbsolutePath
	(
	const JCharacter*	path,
	const JCharacter*	base,		// can be NULL
	JString*			result
	)
{
	assert( !JStringEmpty(path) && result != NULL );

	JBoolean ok = kJTrue;
	if (!JIsRelativePath(path))
		{
		*result = path;
		}
	else if (path[0] == '~')
		{
		ok = JExpandHomeDirShortcut(path, result);
		}
	else if (!JStringEmpty(base))
		{
		*result = JCombinePathAndName(base, path);
		}
	else
		{
		const JString currDir = JGetCurrentDirectory();
		*result = JCombinePathAndName(currDir, path);
		}

	if (ok)
		{
		return JNameUsed(*result);
		}
	else
		{
		result->Clear();
		return kJFalse;
		}
}

/*****************************************************************************
 JConvertToRelativePath

	Converts 'path' to a path relative to 'base'.  Both inputs must be
	absolute paths.  'path' can include a file name on the end.

 ******************************************************************************/

JString
JConvertToRelativePath
	(
	const JCharacter* origPath,
	const JCharacter* origBase
	)
{
	// Check that they are both absolute paths.

	assert( origPath != NULL && !JIsRelativePath(origPath) &&
			origBase != NULL && !JIsRelativePath(origBase) );

	// Remove extra directory separators
	// and make sure that base ends with one.

	JString path = origPath;
	JCleanPath(&path);

	JString base = origBase;
	JCleanPath(&base);
	JAppendDirSeparator(&base);

	// Find and remove the matching directories at the beginning.

	JBoolean hadTDS = kJTrue;
	if (path.GetLastCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		path.AppendCharacter(ACE_DIRECTORY_SEPARATOR_CHAR);
		hadTDS = kJFalse;
		}

	JSize matchLength = JCalcMatchLength(path, base);

	if (!hadTDS)
		{
		path.RemoveSubstring(path.GetLength(), path.GetLength());
		}

	// Back up so we only consider complete directory names.

	while (base.GetCharacter(matchLength) != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		matchLength--;
		}
	if (matchLength <= 3)	// match more than C:\ or \\a
		{
		return path;
		}

	if (matchLength > path.GetLength())
		{
		base.RemoveSubstring(matchLength, matchLength);
		matchLength--;
		}

	path.RemoveSubstring(1, matchLength);
	base.RemoveSubstring(1, matchLength);

	if (base.IsEmpty())
		{
		path.Prepend("." ACE_DIRECTORY_SEPARATOR_STR);
		return path;
		}

	// The number of remaining directory separators in base
	// is the number of levels to go up.

	JSize upCount = 0;

	const JSize baseLength = base.GetLength();
	for (JIndex i=1; i<=baseLength; i++)
		{
		if (base.GetCharacter(i) == ACE_DIRECTORY_SEPARATOR_CHAR)
			{
			upCount++;
			path.Prepend(".." ACE_DIRECTORY_SEPARATOR_STR);
			}
		}
	assert( upCount > 0 );

	return path;
}

/*****************************************************************************
 JExpandHomeDirShortcut

	If the given path begins with ~, this is replaced by the appropriate
	home directory, if it exists.  Otherwise, kJFalse is returned and *result
	is empty.

	If homeDir != NULL, it is set to the home directory that was specified
	by the ~.  If homeLength != NULL it is set to the number of characters
	at the start of path that specified the home directory.

	This function does not check that the resulting expanded path is valid.

	If path doesn't begin with ~, returns kJTrue, *result = path, *homeDir
	is empty, and *homeLength = 0.

 ******************************************************************************/

JBoolean
JExpandHomeDirShortcut
	(
	const JCharacter*	path,
	JString*			result,
	JString*			homeDir,
	JSize*				homeLength
	)
{
	assert( !JStringEmpty(path) && result != NULL );

	JBoolean ok = kJTrue;
	if (path[0] == '~' && path[1] == '\0')
		{
		ok = JGetHomeDirectory(result);
		if (ok && homeDir != NULL)
			{
			*homeDir = *result;
			}
		if (ok && homeLength != NULL)
			{
			*homeLength = 1;
			}
		}
	else if (path[0] == '~' && path[1] == '\\')
		{
		ok = JGetHomeDirectory(result);
		if (ok && homeDir != NULL)
			{
			*homeDir = *result;
			}
		if (ok && homeLength != NULL)
			{
			*homeLength = 2;
			}
		if (ok && path[2] != '\0')
			{
			*result = JCombinePathAndName(*result, path+2);
			}
		}
	else
		{
		*result = path;
		if (homeDir != NULL)
			{
			homeDir->Clear();
			}
		if (homeLength != NULL)
			{
			*homeLength = 0;
			}
		}

	if (ok)
		{
		return kJTrue;
		}
	else
		{
		result->Clear();
		if (homeDir != NULL)
			{
			homeDir->Clear();
			}
		if (homeLength != NULL)
			{
			*homeLength = 0;
			}
		return kJFalse;
		}
}

/*******************************************************************************
 JGetSymbolicLinkTarget

 *******************************************************************************/

JError
JGetSymbolicLinkTarget
	(
	const JCharacter*	linkFullName,
	JString*			targetFullName
	)
{
	targetFullName->Clear();

	if (FAILED(OleInitialize(NULL)))
		{
		return JAccessDenied("OLE");
		}

	IShellLink* link;
	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
								   IID_IShellLink, (void**) &link)))
		{
		IPersistFile* file;
		if (SUCCEEDED(link->QueryInterface(IID_IPersistFile, (void**) &file)))
			{
			WCHAR path[ MAX_PATH ];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, linkFullName, -1, path, MAX_PATH);
			if (SUCCEEDED(file->Load(path, 0)))
				{
				WIN32_FIND_DATA findData;
				JCharacter buf[ 1024 ];
				link->GetPath(buf, 1023, &findData, SLGP_UNCPRIORITY);
				targetFullName->Set(buf);
				}

			file->Release();
			}
		link->Release();
		}

	if (!targetFullName->IsEmpty())
		{
		return JNoError();
		}
	else
		{
		return JUnknownError(errno);
		}
}

/*******************************************************************************
 JGetDirectoryFromCSIDL

 *******************************************************************************/

JBoolean
JGetDirectoryFromCSIDL
	(
	const int	folderCSIDL,
	JString*	path
	)
{
	path->Clear();

	if (FAILED(OleInitialize(NULL)))
		{
		return kJFalse;
		}

	JCharacter s[ MAX_PATH ];
	if (SHGetFolderPath(NULL, folderCSIDL | CSIDL_FLAG_CREATE, NULL, 0, s) == S_OK)
		{
		*path = s;
		JAppendDirSeparator(path);
		return kJTrue;
		} 
	else 
		{
		return kJFalse;
		}
}
