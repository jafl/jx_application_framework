/******************************************************************************
 jDirUtil_UNIX.cpp

	Directory utilities implemented for the UNIX System.

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <jDirUtil.h>
#include <JThisProcess.h>
#include <JDirInfo.h>
#include <JProgressDisplay.h>
#include <JLatentPG.h>
#include <JSimpleProcess.h>
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
	const JString& name
	)
{
	ACE_stat info;
	return JI2B( ACE_OS::lstat(name.GetBytes(), &info) == 0 );
}

/******************************************************************************
 JSameDirEntry

	Returns kJTrue if the given names point to the same inode in the
	file system.

 ******************************************************************************/

JBoolean
JSameDirEntry
	(
	const JString& name1,
	const JString& name2
	)
{
	ACE_stat stbuf1, stbuf2;
	return JI2B(ACE_OS::stat(name1.GetBytes(), &stbuf1) == 0 &&
				ACE_OS::stat(name2.GetBytes(), &stbuf2) == 0 &&
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
	const JString&	name,
	time_t*			modTime
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
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
	const JString&	name,
	mode_t*			perms
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
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
	const JString&	name,
	const mode_t	perms
	)
{
	jclear_errno();
	if (chmod(name.GetBytes(), perms) == 0)
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
	const JString&	name,
	uid_t*			uid
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
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
	const JString&	name,
	gid_t*			gid
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
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
 JSetOwner

	Sets the owner to the specified uid and the group to the specified gid.

 ******************************************************************************/

JError
JSetOwner
	(
	const JString&	name,
	const uid_t		uid,
	const gid_t		gid
	)
{
	if (chown(name.GetBytes(), uid, gid) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EPERM || err == EACCES)
		{
		return JAccessDenied(name);
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
	else if (err == ENOTDIR)
		{
		return JComponentNotDirectory(name);
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
		return JPathContainsLoop(name);
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
	const JString& src,
	const JString& dest
	)
{
	jclear_errno();
	if (symlink(src.GetBytes(), dest.GetBytes()) == 0)
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
	const JString& dirName
	)
{
	ACE_stat info;
	return JI2B(
			ACE_OS::lstat(dirName.GetBytes(), &info) == 0 &&
			ACE_OS::stat( dirName.GetBytes(), &info) == 0 &&
			S_ISDIR(info.st_mode) );
}

/******************************************************************************
 JDirectoryReadable

	Returns kJTrue if the specified directory can be read from.

 ******************************************************************************/

JBoolean
JDirectoryReadable
	(
	const JString& dirName
	)
{
	return JI2B(JDirectoryExists(dirName) &&
				(getuid() == 0 || access(dirName.GetBytes(), R_OK) == 0));
}

/******************************************************************************
 JDirectoryWritable

	Returns kJTrue if the specified directory can be written to.

 ******************************************************************************/

JBoolean
JDirectoryWritable
	(
	const JString& dirName
	)
{
	return JI2B(JDirectoryExists(dirName) &&
				(getuid() == 0 || access(dirName.GetBytes(), W_OK) == 0));
}

/******************************************************************************
 JCanEnterDirectory

	Returns kJTrue if it is possible to make the specified directory
	the working directory.

 ******************************************************************************/

JBoolean
JCanEnterDirectory
	(
	const JString& dirName
	)
{
	return JI2B(JDirectoryExists(dirName) &&
				(getuid() == 0 || access(dirName.GetBytes(), X_OK) == 0));
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

static JError JMkDir(const JString& dirName, const mode_t mode);

JError
JCreateDirectory
	(
	const JString& dirName
	)
{
	return JCreateDirectory(dirName, 0755);
}

JError
JCreateDirectory
	(
	const JString&	dirName,
	const mode_t	mode
	)
{
	if (JDirectoryExists(dirName))
		{
		return JSetPermissions(dirName, mode);
		}

	JString path = dirName;
	JCleanPath(&path);
	JAppendDirSeparator(&path);

	JStringIterator iter(path, kJIteratorStartAfter, 1);
	JString dir;
	while (iter.Next("/"))
		{
		JIndex i;
		if (!iter.GetPrevCharacterIndex(&i))
			{
			continue;
			}

		dir.Set(path, JCharacterRange(1, i));
		if (!JDirectoryExists(dir))
			{
			const JError err = JMkDir(dir, mode);
			if (!err.OK())
				{
				return err;
				}
			}
		}

	return JNoError();
}

// private

JError
JMkDir
	(
	const JString&	dirName,
	const mode_t	mode
	)
{
	jclear_errno();
	if (mkdir(dirName.GetBytes(), mode) == 0)
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
	const JString& oldName,
	const JString& newName
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
	if (rename(oldName.GetBytes(), newName.GetBytes()) == 0)
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
	const JString& dirName
	)
{
	jclear_errno();
	if (chdir(dirName.GetBytes()) == 0)
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
	const JString& dirName
	)
{
	jclear_errno();
	if (rmdir(dirName.GetBytes()) == 0)
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

	if !sync, *p will contain the process

 ******************************************************************************/

JBoolean
JKillDirectory
	(
	const JString&	dirName,
	const JBoolean	sync,
	JProcess**		p
	)
{
	if (p != NULL)
		{
		*p = NULL;
		}

	const JUtf8Byte* argv[] = {"rm", "-rf", dirName.GetBytes(), NULL};
	if (sync)
		{
		const JError err = JExecute(argv, sizeof(argv), NULL);
		if (err.OK())
			{
			return !JNameUsed(dirName);
			}
		else
			{
			return kJFalse;
			}
		}
	else
		{
		JSimpleProcess* sp;
		const JError err = JSimpleProcess::Create(&sp, argv, sizeof(argv), kJTrue);
		if (err.OK())
			{
			JThisProcess::Ignore(sp);
			if (p != NULL)
				{
				*p = sp;
				}
			return kJTrue;
			}
		else
			{
			err.ReportIfError();
			return kJFalse;
			}
		}
}

/******************************************************************************
 JGetCurrentDirectory

	Returns the full path of the current working directory.

 ******************************************************************************/

JString
JGetCurrentDirectory()
{
	JUtf8Byte buf[1024];
	JUtf8Byte* result = getcwd(buf, 1024);

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

	JUtf8Byte* envHomeDir = getenv("HOME");
	if (!JString::IsEmpty(envHomeDir))
		{
		const JString dir = JString(envHomeDir, 0, kJFalse);
		if (JDirectoryExists(dir))
			{
			*homeDir = dir;
			JAppendDirSeparator(homeDir);
			return kJTrue;
			}
		}

	// try password information

	JUtf8Byte* envUserName = getenv("USER");

	struct passwd* pw;
	if (envUserName != NULL)
		{
		pw = getpwnam(envUserName);
		}
	else
		{
		pw = getpwuid( getuid() );
		}

	if (pw != NULL)
		{
		const JString dir = JString(pw->pw_dir, 0, kJFalse);
		if (JDirectoryExists(dir))
			{
			*homeDir = dir;
			JAppendDirSeparator(homeDir);
			return kJTrue;
			}
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
	const JString&	user,
	JString*		homeDir
	)
{
	struct passwd* pw = getpwnam(user.GetBytes());
	if (pw != NULL)
		{
		const JString dir = JString(pw->pw_dir, 0, kJFalse);
		if (JDirectoryExists(dir))
			{
			*homeDir = dir;
			JAppendDirSeparator(homeDir);
			return kJTrue;
			}
		}

	homeDir->Clear();
	return kJFalse;
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
	const JString&	user,
	JString*		prefsDir
	)
{
	return JGetHomeDirectory(user, prefsDir);
}

/******************************************************************************
 JGetTempDirectory

	Returns the system's scratch directory.

 ******************************************************************************/

static JBoolean theTempPathInitFlag = kJFalse;
static JString theTempPath;

JBoolean
JGetTempDirectory
	(
	JString* tempDir
	)
{
	if (!theTempPathInitFlag)
		{
		JUtf8Byte* path = getenv("TMPDIR");
		if (!JString::IsEmpty(path) && JDirectoryWritable(JString(path, 0, kJFalse)))
			{
			theTempPath.Set(path);
			}
		else if (P_tmpdir != NULL && JDirectoryWritable(JString(P_tmpdir, 0, kJFalse)))
			{
			theTempPath.Set(P_tmpdir);
			}
		else
			{
			theTempPath.Set("/tmp/");
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

static const JString theTmpDirForError("/tmp", 0, kJFalse);
static const JString theTmpDirPrefix("temp_dir_", 0, kJFalse);
static const JString theTmpDirTemplate("XXXXXX", 0, kJFalse);

JError
JCreateTempDirectory
	(
	const JString*	path,
	const JString*	prefix,
	JString*		fullName
	)
{
	JString p;
	if (!JString::IsEmpty(path))
		{
		p = *path;
		}
	else if (!JGetTempDirectory(&p))
		{
		return JDirEntryDoesNotExist(theTmpDirForError);
		}

	if (!JString::IsEmpty(prefix))
		{
		p = JCombinePathAndName(p, *prefix);
		}
	else
		{
		p = JCombinePathAndName(p, theTmpDirPrefix);
		}

	p           += theTmpDirTemplate;
	JUtf8Byte* s = p.AllocateBytes();

	jclear_errno();
	JUtf8Byte* d = mkdtemp(s);
	if (d != NULL)
		{
		*fullName = s;
		JAppendDirSeparator(fullName);
		jdelete [] s;
		return JNoError();
		}

	fullName->Clear();
	jdelete [] s;

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
	const JString&	name,
	JString*		trueName
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

static const JRegex trailingDotPattern = "(?:/\\.)+$";
static const JRegex multiSlashPattern  = "(?<!:)/{2,}";

void
JCleanPath
	(
	JString* path
	)
{
	JStringIterator iter(path);
	while (iter.Next(multiSlashPattern))
		{
		iter.ReplaceLastMatch("/");
		}

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next("/./"))
		{
		iter.ReplaceLastMatch("/");
		}

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	if (iter.Next(trailingDotPattern))		// max 1 match
		{
		iter.RemoveLastMatch();
		}
}

/******************************************************************************
 JIsAbsolutePath

 ******************************************************************************/

JBoolean
JIsAbsolutePath
	(
	const JString& path
	)
{
	assert( !path.IsEmpty() );
	const JUtf8Character c = path.GetFirstCharacter();
	return JI2B( c == '/' || c == '~' );
}

/******************************************************************************
 JGetRootDirectory

 ******************************************************************************/

JString
JGetRootDirectory()
{
	return JString("/", 0);
}

/******************************************************************************
 JIsRootDirectory

 ******************************************************************************/

JBoolean
JIsRootDirectory
	(
	const JString& dirName
	)
{
	assert( !dirName.IsEmpty() );
	return JI2B( dirName == "/" );
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
	const JString&	path,
	const JString*	base,		// can be NULL
	JString*		result
	)
{
	assert( !path.IsEmpty() && result != NULL );

	JBoolean ok = kJTrue;
	const JUtf8Character c = path.GetFirstCharacter();
	if (c == '/')
		{
		*result = path;
		}
	else if (c == '~')
		{
		ok = JExpandHomeDirShortcut(path, result);
		}
	else if (!JString::IsEmpty(base))
		{
		*result = JCombinePathAndName(*base, path);
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
	const JString& origPath,
	const JString& origBase
	)
{
	// Check that they are both absolute paths.

	assert( origPath.GetFirstCharacter() == '/' &&
			origBase.GetFirstCharacter() == '/' );

	// Remove extra directory separators
	// and make sure that base ends with one.

	JString path = origPath;
	JCleanPath(&path);

	JString base = origBase;
	JCleanPath(&base);
	JAppendDirSeparator(&base);

	// Find and remove the matching directories at the beginning.
	// We only consider complete directory names.

	JBoolean hadTDS = kJTrue;
	if (path.GetLastCharacter() != '/')
		{
		path.Append("/");
		hadTDS = kJFalse;
		}

	JSize matchLength = JString::CalcCharacterMatchLength(path, base);

	JStringIterator pathIter(&path);
	if (!hadTDS)
		{
		pathIter.MoveTo(kJIteratorStartAtEnd, 0);
		pathIter.RemovePrev();
		}

	JStringIterator baseIter(&base, kJIteratorStartAtEnd);
	const JBoolean found = baseIter.Prev("/");
	assert( found );
	if (baseIter.AtBeginning())
		{
		return path;
		}
	matchLength = baseIter.GetNextCharacterIndex();

	baseIter.RemoveAllPrev();
	baseIter.RemoveNext();

	pathIter.MoveTo(kJIteratorStartAtBeginning, 0);
	pathIter.SkipNext(matchLength);
	pathIter.RemoveAllPrev();

	if (base.IsEmpty())
		{
		path.Prepend("./");
		return path;
		}

	// The number of remaining directory separators in base
	// is the number of levels to go up.

	baseIter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (baseIter.Next("/"))
		{
		path.Prepend("../");
		}

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
	const JString&	path,
	JString*		result,
	JString*		homeDir,
	JSize*			homeLength
	)
{
	assert( !path.IsEmpty() );
	assert( result != NULL );

	JBoolean ok = kJTrue;
	if (path == "~")
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
	else if (path.BeginsWith("~/"))
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
		if (ok && path.GetCharacterCount() == 2)
			{
			JStringIterator iter(path);
			iter.SkipNext(2);
			iter.BeginMatch();
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			const JStringMatch& m = iter.FinishMatch();
			*result = JCombinePathAndName(*result, m.GetString());
			}
		}
	else if (path.BeginsWith("~"))
		{
		JStringIterator iter(path, kJIteratorStartAfter, 1);
		iter.BeginMatch();
		iter.Next("/");
		const JString userName = iter.FinishMatch().GetString();

		ok = JGetHomeDirectory(userName, result);
		if (ok && homeDir != NULL)
			{
			*homeDir = *result;
			}
		if (ok && homeLength != NULL)
			{
			*homeLength = iter.GetPrevCharacterIndex();
			}
		if (ok && !iter.AtEnd())
			{
			iter.BeginMatch();
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			*result = JCombinePathAndName(*result, iter.FinishMatch().GetString());
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
	const JString&	linkFullName,
	JString*		targetFullName
	)
{
	const JSize kBufferSize = 10000;
	char buf[ kBufferSize ];
	const long linkNameSize = readlink(linkFullName.GetBytes(), buf, kBufferSize-1);
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
