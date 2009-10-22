/******************************************************************************
 jDirUtil_UNIX.cpp

	Directory utilities implemented for the UNIX System.

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jDirUtil.h>
#include <jProcessUtil.h>
#include <JDirInfo.h>
#include <JProgressDisplay.h>
#include <JLatentPG.h>
#include <jGlobals.h>
#include <JStdError.h>
#include <pwd.h>
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
	ACE_stat info;
	return JI2B( ACE_OS::lstat(name, &info) == 0 );
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
	ACE_stat stbuf1, stbuf2;
	return JI2B(ACE_OS::stat(name1, &stbuf1) == 0 &&
				ACE_OS::stat(name2, &stbuf2) == 0 &&
				stbuf1.st_dev == stbuf2.st_dev &&
				stbuf1.st_ino == stbuf2.st_ino);
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
	ACE_stat info;
	if (ACE_OS::stat(name, &info) == 0)
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
	ACE_stat info;
	if (ACE_OS::stat(name, &info) == 0)
		{
		*perms = info.st_mode;
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

	Can return JAccessDenied, JFileSystemReadOnly, JSegFault, JNameTooLong,
	JDirEntryDoesNotExist, JNoKernelMemory, JComponentNotDirectory,
	JPathContainsLoop.

 ******************************************************************************/

JError
JSetPermissions
	(
	const JCharacter*	name,
	const mode_t		perms
	)
{
	jclear_errno();
	if (chmod(name, perms) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EPERM || err == EACCES)
		{
		return JAccessDenied(name);
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JDirEntryDoesNotExist(name);
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(name);
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(name);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JGetOwnerID

	Returns the uid of the owner.

 ******************************************************************************/

JError
JGetOwnerID
	(
	const JCharacter*	name,
	uid_t*				uid
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name, &info) == 0)
		{
		*uid = info.st_uid;
		return JNoError();
		}

	*uid = 0;

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
 JGetOwnerGroup

	Returns the gid of the owner.

 ******************************************************************************/

JError
JGetOwnerGroup
	(
	const JCharacter*	name,
	gid_t*				gid
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name, &info) == 0)
		{
		*gid = info.st_gid;
		return JNoError();
		}

	*gid = 0;

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
 JCreateSymbolicLink

	Creates a symbolic link dest that points to src.

 ******************************************************************************/

JError
JCreateSymbolicLink
	(
	const JCharacter* src,
	const JCharacter* dest
	)
{
	jclear_errno();
	if (symlink(src, dest) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EPERM || err == EACCES)
		{
		return JAccessDenied(dest);
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JBadPath(dest);
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(dest);
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
		}
	else if (err == EEXIST)
		{
		return JDirEntryAlreadyExists(dest);
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(dest);
		}
	else if (err == ENOSPC)
		{
		return JFileSystemFull();
		}
	else if (err == EIO)
		{
		return JGeneralIO();
		}
	else
		{
		return JUnexpectedError(err);
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
	ACE_stat info;
	return JI2B(
			ACE_OS::lstat(dirName, &info) == 0 &&
			ACE_OS::stat( dirName, &info) == 0 &&
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
				(getuid() == 0 || access(dirName, R_OK) == 0));
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
				(getuid() == 0 || access(dirName, W_OK) == 0));
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
	return JI2B(JDirectoryExists(dirName) &&
				(getuid() == 0 || access(dirName, X_OK) == 0));
}

/******************************************************************************
 JCreateDirectory

	Creates the requested directory.  Equivalent to "mkdir -p".

	These are two separate functions because the default mode is
	system dependent, so it can't be included in jDirUtil.h.

	Can return JSegFault, JAccessDenied, JNameTooLong, JBadPath,
	JComponentNotDirectory, JNoKernelMemory, JFileSystemReadOnly,
	JPathContainsLoop, JFileSystemFull.

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
	while (path.LocateNextSubstring("/", &slashIndex))
		{
		dir = path.GetSubstring(1, slashIndex);
		if (!JDirectoryExists(dir))
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

// private

JError
JMkDir
	(
	const JCharacter*	dirName,
	const mode_t		mode
	)
{
	jclear_errno();
	if (mkdir(dirName, mode) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EEXIST)
		{
		return JDirEntryAlreadyExists(dirName);
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == EACCES)
		{
		return JAccessDenied(dirName);
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JBadPath(dirName);
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(dirName);
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(dirName);
		}
	else if (err == ENOSPC)
		{
		return JFileSystemFull();
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JRenameDirEntry

	Renames the specified directory entry.

	Can return JCantRenameFileToDirectory, JCantRenameAcrossFilesystems,
	JCantRenameToNonemptyDirectory, JFileBusy, JDirectoryCantBeOwnChild,
	JTooManyLinks, JComponentNotDirectory, JSegFault, JAccessDenied,
	JNameTooLong, JBadPath, JNoKernelMemory, JFileSystemReadOnly,
	JPathContainsLoop, JFileSystemFull.

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
	if (err == EISDIR)
		{
		return JCantRenameFileToDirectory(oldName, newName);
		}
	else if (err == EXDEV)
		{
		return JCantRenameAcrossFilesystems();
		}
	else if (err == ENOTEMPTY)
		{
		return JCantRenameToNonemptyDirectory();
		}
	else if (err == EEXIST)
		{
		return JDirEntryAlreadyExists(newName);
		}
	else if (err == EBUSY)
		{
		return JFileBusy(newName);
		}
	else if (err == EINVAL)
		{
		return JDirectoryCantBeOwnChild();
		}
	else if (err == EMLINK)
		{
		return JTooManyLinks(oldName);
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(oldName, newName);
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == EACCES || err == EPERM)
		{
		return JAccessDenied(oldName, newName);
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JBadPath(oldName, newName);
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(oldName, newName);
		}
	else if (err == ENOSPC)
		{
		return JFileSystemFull();
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
	if (chdir(dirName) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EPERM || err == EACCES)
		{
		return JAccessDenied(dirName);
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JBadPath(dirName);
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(dirName);
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(dirName);
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JRemoveDirectory

	Removes the specified directory.  This only works if the directory is empty.

	Can return JAccessDenied, JSegFault, JNameTooLong, JBadPath,
	JComponentNotDirectory, JDirectoryNotEmpty, JDirectoryBusy,
	JNoKernelMemory, JFileSystemReadOnly, JPathContainsLoop.

 ******************************************************************************/

JError
JRemoveDirectory
	(
	const JCharacter* dirName
	)
{
	jclear_errno();
	if (rmdir(dirName) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EPERM || err == EACCES)
		{
		return JAccessDenied(dirName);
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JBadPath(dirName);
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(dirName);
		}
	else if (err == ENOTEMPTY)
		{
		return JDirectoryNotEmpty(dirName);
		}
	else if (err == EBUSY)
		{
		return JDirectoryBusy(dirName);
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(dirName);
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
	const JCharacter* argv[] = {"rm", "-rf", dirName, NULL};
	const JError err = JExecute(argv, sizeof(argv), NULL,
								kJIgnoreConnection, NULL,
								kJIgnoreConnection, NULL,
								kJTossOutput,       NULL);
	if (err.OK())
		{
		return !JNameUsed(dirName);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 JGetCurrentDirectory

	Returns the full path of the current working directory.

 ******************************************************************************/

JString
JGetCurrentDirectory()
{
	char buf[1024];
	char* result = getcwd(buf, 1024);

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

	Returns kJTrue if the current user has a home directory.

 ******************************************************************************/

JBoolean
JGetHomeDirectory
	(
	JString* homeDir
	)
{
	// try HOME environment variable

	char* envHomeDir = getenv("HOME");
	if (envHomeDir != NULL && JDirectoryExists(envHomeDir))
		{
		*homeDir = envHomeDir;
		JAppendDirSeparator(homeDir);
		return kJTrue;
		}

	// try password information

	char* envUserName = getenv("USER");

	struct passwd* pw;
	if (envUserName != NULL)
		{
		pw = getpwnam(envUserName);
		}
	else
		{
		pw = getpwuid( getuid() );
		}

	if (pw != NULL && JDirectoryExists(pw->pw_dir))
		{
		*homeDir = pw->pw_dir;
		JAppendDirSeparator(homeDir);
		return kJTrue;
		}

	// give up

	homeDir->Clear();
	return kJFalse;
}

/******************************************************************************
 JGetHomeDirectory

	Returns kJTrue if the specified user has a home directory.

 ******************************************************************************/

JBoolean
JGetHomeDirectory
	(
	const JCharacter*	user,
	JString*			homeDir
	)
{
	struct passwd* pw = getpwnam(user);
	if (pw != NULL && JDirectoryExists(pw->pw_dir))
		{
		*homeDir = pw->pw_dir;
		JAppendDirSeparator(homeDir);
		return kJTrue;
		}
	else
		{
		homeDir->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 JGetPrefsDirectory

	Returns kJTrue if the current user has a prefs directory.

 ******************************************************************************/

JBoolean
JGetPrefsDirectory
	(
	JString* prefsDir
	)
{
	return JGetHomeDirectory(prefsDir);
}

/******************************************************************************
 JGetPrefsDirectory

	Returns kJTrue if the specified user has a home directory.

 ******************************************************************************/

JBoolean
JGetPrefsDirectory
	(
	const JCharacter*	user,
	JString*			prefsDir
	)
{
	return JGetHomeDirectory(user, prefsDir);
}

/******************************************************************************
 JGetTempDirectory

	Returns the system's scratch directory.

 ******************************************************************************/

JBoolean
JGetTempDirectory
	(
	JString* tempDir
	)
{
	// inside function to keep gcc 3.3.3 (cygwin) happy
	static JBoolean theTempPathInitFlag = kJFalse;
	static JString theTempPath;

	if (!theTempPathInitFlag)
		{
		char* path = getenv("TMPDIR");
		if (!JStringEmpty(path) && JDirectoryWritable(path))
			{
			theTempPath = path;
			}
		else if (P_tmpdir != NULL && JDirectoryWritable(P_tmpdir))
			{
			theTempPath = P_tmpdir;
			}
		else
			{
			theTempPath = "/tmp/";
			}

		JAppendDirSeparator(&theTempPath);
		theTempPathInitFlag = kJTrue;
		}

	*tempDir = theTempPath;
	return kJTrue;
}

/******************************************************************************
 JCreateTempDirectory

	Creates a unique directory in the specified directory.
	If path is empty, it uses the system's scratch directory.
	If prefix is empty, uses temp_dir_.

 ******************************************************************************/

JError
JCreateTempDirectory
	(
	const JCharacter*	path,
	const JCharacter*	prefix,
	JString*			fullName
	)
{
	JString p;
	if (!JStringEmpty(path))
		{
		p = path;
		}
	else if (!JGetTempDirectory(&p))
		{
		return JDirEntryDoesNotExist("/tmp");
		}

	if (!JStringEmpty(prefix))
		{
		p = JCombinePathAndName(p, prefix);
		}
	else
		{
		p = JCombinePathAndName(p, "temp_dir_");
		}

	p      += "XXXXXX";
	char* s = p.AllocateCString();

	jclear_errno();
	char* d = mkdtemp(s);
	if (d != NULL)
		{
		*fullName = s;
		JAppendDirSeparator(fullName);
		delete [] s;
		return JNoError();
		}

	fullName->Clear();
	delete [] s;

	// EINVAL counts as unexpected

	const int err = jerrno();
	if (err == EEXIST)
		{
		return JDirEntryAlreadyExists(p);
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == EACCES)
		{
		return JAccessDenied(p);
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JBadPath(p);
		}
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(p);
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else if (err == EROFS)
		{
		return JFileSystemReadOnly();
		}
	else if (err == ELOOP)
		{
		return JPathContainsLoop(p);
		}
	else if (err == ENOSPC)
		{
		return JFileSystemFull();
		}
	else
		{
		return JUnexpectedError(err);
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

	if (!JNameUsed(name))
		{
		return kJFalse;
		}

	// check if it is a directory

	else if (JDirectoryExists(name))
		{
		const JString currPath = JGetCurrentDirectory();

		JError err = JChangeDirectory(name);
		if (!err.OK())
			{
			return kJFalse;
			}

		*trueName = JGetCurrentDirectory();

		err = JChangeDirectory(currPath);
		assert_ok( err );

		return kJTrue;
		}

	// it is a file, socket, fifo, etc.

	else
		{
		JString origPath, fileName;
		JSplitPathAndName(name, &origPath, &fileName);

		// get true directory

		JString truePath;
		if (!JGetTrueName(origPath, &truePath))
			{
			return kJFalse;
			}

		// resolve symbolic link

		JString target;
		if ((JGetSymbolicLinkTarget(name, &target)).OK())
			{
			if (JIsRelativePath(target))
				{
				target.Prepend(truePath);
				}
			return JGetTrueName(target, trueName);
			}
		else
			{
			*trueName = JCombinePathAndName(truePath, fileName);
			return kJTrue;
			}
		}
}

/******************************************************************************
 JCleanPath

	Removes fluff from the given path:

		//
		/./
		trailing /.

	We can't remove /x/../ because if x is a symlink, the result would not
	be the same directory.

	This is required to work for files and directories.

 ******************************************************************************/

void
JCleanPath
	(
	JString* path
	)
{
	while (path->EndsWith("/."))
		{
		path->RemoveSubstring(path->GetLength(), path->GetLength());
		}

	JIndex i;
	while (path->LocateSubstring("/./", &i))
		{
		path->RemoveSubstring(i, i+1);
		}

	while (path->LocateSubstring("//", &i) &&
		   (i == 1 || path->GetCharacter(i-1) != ':'))
		{
		path->RemoveSubstring(i, i);
		}
}

/******************************************************************************
 JIsAbsolutePath

 ******************************************************************************/

JBoolean
JIsAbsolutePath
	(
	const JCharacter* path
	)
{
	assert( !JStringEmpty(path) );
	return JI2B( path[0] == '/' || path[0] == '~' );
}

/******************************************************************************
 JGetRootDirectory

 ******************************************************************************/

JString
JGetRootDirectory()
{
	return JString("/");
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
	assert( !JStringEmpty(dirName) );
	return JI2B( dirName[0] == '/' && dirName[1] == '\0' );
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
	if (path[0] == '/')
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

	assert( origPath != NULL && origPath[0] == '/' &&
			origBase != NULL && origBase[0] == '/' );

	// Remove extra directory separators
	// and make sure that base ends with one.

	JString path = origPath;
	JCleanPath(&path);

	JString base = origBase;
	JCleanPath(&base);
	JAppendDirSeparator(&base);

	// Find and remove the matching directories at the beginning.
	// The while loop backs us up so we only consider complete directory names.

	JBoolean hadTDS = kJTrue;
	if (path.GetLastCharacter() != '/')
		{
		path.AppendCharacter('/');
		hadTDS = kJFalse;
		}

	JSize matchLength = JCalcMatchLength(path, base);

	if (!hadTDS)
		{
		path.RemoveSubstring(path.GetLength(), path.GetLength());
		}

	while (base.GetCharacter(matchLength) != '/')
		{
		matchLength--;
		}
	assert( matchLength >= 1 );
	if (matchLength == 1)
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
		path.Prepend("./");
		return path;
		}

	// The number of remaining directory separators in base
	// is the number of levels to go up.

	JSize upCount = 0;

	const JSize baseLength = base.GetLength();
	for (JIndex i=1; i<=baseLength; i++)
		{
		if (base.GetCharacter(i) == '/')
			{
			upCount++;
			path.Prepend("../");
			}
		}
	assert( upCount > 0 );

	return path;
}

/*****************************************************************************
 JExpandHomeDirShortcut

	If the given path begins with ~ or ~x, this is replaced by the appropriate
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
	else if (path[0] == '~' && path[1] == '/')
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
	else if (path[0] == '~')
		{
		JString userName = path+1;
		JIndex i;
		const JBoolean found = userName.LocateSubstring("/", &i);
		if (found)
			{
			userName.RemoveSubstring(i, userName.GetLength());
			}

		ok = JGetHomeDirectory(userName, result);
		if (ok && homeDir != NULL)
			{
			*homeDir = *result;
			}
		if (ok && homeLength != NULL)
			{
			*homeLength = found ? i+1 : strlen(path);
			}
		if (ok && found && path[i+1] != '\0')
			{
			*result = JCombinePathAndName(*result, path+i+1);
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
	const char*	linkFullName,
	JString*	targetFullName
	)
{
	const JSize kBufferSize = 10000;
	char buf[ kBufferSize ];
	const long linkNameSize = readlink(linkFullName, buf, kBufferSize-1);
	if (linkNameSize != -1)
		{
		targetFullName->Set(buf, linkNameSize);
		return JNoError();
		}

	targetFullName->Clear();
	const int err = jerrno();
	if (err == ENOTDIR)
		{
		return JComponentNotDirectory(linkFullName);
		}
	else if (err == ENAMETOOLONG)
		{
		return JNameTooLong();
		}
	else if (err == ENOENT)
		{
		return JDirEntryDoesNotExist(linkFullName);
		}
	else if (err == EPERM || err == EACCES)
		{
		return JAccessDenied(linkFullName);
		}
	else if (err == ELOOP)
		{
		return JTooManyLinks(linkFullName);
		}
	else if (err == EINVAL)
		{
		return JNotSymbolicLink(linkFullName);
		}
	else if (err == EIO)
		{
		return JGeneralIO();
		}
	else if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == ENOMEM)
		{
		return JNoKernelMemory();
		}
	else
		{
		return JUnexpectedError(err);
		}
}
