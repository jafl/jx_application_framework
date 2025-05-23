/******************************************************************************
 jDirUtil_UNIX.cpp

	Directory utilities implemented for the UNIX System.

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#include "jDirUtil.h"
#include "JThisProcess.h"
#include "JDirInfo.h"
#include "JProgressDisplay.h"
#include "JLatentPG.h"
#include "JSimpleProcess.h"
#include "JStringIterator.h"
#include "jGlobals.h"
#include "JStdError.h"
#include <pwd.h>
#include "jErrno.h"
#include "jAssert.h"

/******************************************************************************
 JNameUsed

	Returns true if the specified name exists. (file, directory, link, etc).

 ******************************************************************************/

bool
JNameUsed
	(
	const JString& name
	)
{
	ACE_stat info;
	return ACE_OS::lstat(name.GetBytes(), &info) == 0;
}

/******************************************************************************
 JSameDirEntry

	Returns true if the given names point to the same inode in the
	file system.

 ******************************************************************************/

bool
JSameDirEntry
	(
	const JString& name1,
	const JString& name2
	)
{
	ACE_stat stbuf1, stbuf2;
	return (ACE_OS::stat(name1.GetBytes(), &stbuf1) == 0 &&
			ACE_OS::stat(name2.GetBytes(), &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev &&
			stbuf1.st_ino == stbuf2.st_ino);
}

/******************************************************************************
 JGetModificationTime

	Returns the last time that the file was modified.
	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

bool
JGetModificationTime
	(
	const JString&	name,
	time_t*			modTime,
	JError*			err
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
	{
		*modTime = info.st_mtime;
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	*modTime = 0;

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(name);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JGetPermissions

	Returns the access permissions for the specified file.
	Can return JDirEntryDoesNotExist.

 ******************************************************************************/

bool
JGetPermissions
	(
	const JString&	name,
	mode_t*			perms,
	JError*			err
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
	{
		*perms = info.st_mode;
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	*perms = 0;

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(name);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JSetPermissions

	Sets the access permissions for the specified file.

	Can return JAccessDenied, JFileSystemReadOnly, JSegFault, JNameTooLong,
	JDirEntryDoesNotExist, JNoKernelMemory, JComponentNotDirectory,
	JPathContainsLoop.

 ******************************************************************************/

bool
JSetPermissions
	(
	const JString&	name,
	const mode_t	perms,
	JError*			err
	)
{
	jclear_errno();
	if (chmod(name.GetBytes(), perms) == 0)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EPERM || e == EACCES)
		{
			*err = JAccessDenied(name);
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(name);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(name);
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(name);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JGetOwnerID

	Returns the uid of the owner.

 ******************************************************************************/

bool
JGetOwnerID
	(
	const JString&	name,
	uid_t*			uid,
	JError*			err
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
	{
		*uid = info.st_uid;
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	*uid = 0;

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(name);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JGetOwnerGroup

	Returns the gid of the owner.

 ******************************************************************************/

bool
JGetOwnerGroup
	(
	const JString&	name,
	gid_t*			gid,
	JError*			err
	)
{
	ACE_stat info;
	if (ACE_OS::stat(name.GetBytes(), &info) == 0)
	{
		*gid = info.st_gid;
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	*gid = 0;

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(name);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JSetOwner

	Sets the owner to the specified uid and the group to the specified gid.

 ******************************************************************************/

bool
JSetOwner
	(
	const JString&	name,
	const uid_t		uid,
	const gid_t		gid,
	JError*			err
	)
{
	if (chown(name.GetBytes(), uid, gid) == 0)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EPERM || e == EACCES)
		{
			*err = JAccessDenied(name);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(name);
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(name);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(name);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JCreateSymbolicLink

	Creates a symbolic link dest that points to src.

 ******************************************************************************/

bool
JCreateSymbolicLink
	(
	const JString&	src,
	const JString&	dest,
	JError*			err
	)
{
	jclear_errno();
	if (symlink(src.GetBytes(), dest.GetBytes()) == 0)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EPERM || e == EACCES)
		{
			*err = JAccessDenied(dest);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JBadPath(dest);
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(dest);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == EEXIST)
		{
			*err = JDirEntryAlreadyExists(dest);
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(dest);
		}
		else if (e == ENOSPC)
		{
			*err = JFileSystemFull();
		}
		else if (e == EIO)
		{
			*err = JGeneralIO();
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JDirectoryExists

	Returns true if the specified directory exists.

 ******************************************************************************/

bool
JDirectoryExists
	(
	const JString& dirName
	)
{
	ACE_stat info;
	return ACE_OS::lstat(dirName.GetBytes(), &info) == 0 &&
			ACE_OS::stat( dirName.GetBytes(), &info) == 0 &&
			S_ISDIR(info.st_mode);
}

/******************************************************************************
 JDirectoryReadable

	Returns true if the specified directory can be read from.

 ******************************************************************************/

bool
JDirectoryReadable
	(
	const JString& dirName
	)
{
	return JDirectoryExists(dirName) &&
				(getuid() == 0 || access(dirName.GetBytes(), R_OK) == 0);
}

/******************************************************************************
 JDirectoryWritable

	Returns true if the specified directory can be written to.

 ******************************************************************************/

bool
JDirectoryWritable
	(
	const JString& dirName
	)
{
	return JDirectoryExists(dirName) &&
				(getuid() == 0 || access(dirName.GetBytes(), W_OK) == 0);
}

/******************************************************************************
 JCanEnterDirectory

	Returns true if it is possible to make the specified directory
	the working directory.

 ******************************************************************************/

bool
JCanEnterDirectory
	(
	const JString& dirName
	)
{
	return JDirectoryExists(dirName) &&
				(getuid() == 0 || access(dirName.GetBytes(), X_OK) == 0);
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

static bool jMkDir(const JString& dirName, const mode_t mode, JError* err);

bool
JCreateDirectory
	(
	const JString&	dirName,
	JError*			err
	)
{
	return JCreateDirectory(dirName, 0755, err);
}

bool
JCreateDirectory
	(
	const JString&	dirName,
	const mode_t	mode,
	JError*			err
	)
{
	if (JDirectoryExists(dirName))
	{
		return JSetPermissions(dirName, mode, err);
	}

	JString path = dirName;
	JCleanPath(&path);
	JAppendDirSeparator(&path);

	JStringIterator iter(path, JStringIterator::kStartAfterChar, 1);
	JString dir;
	while (iter.Next("/"))
	{
		JIndex i;
		if (!iter.GetPrevCharacterIndex(&i))
		{
			continue;
		}

		dir.Set(path, JCharacterRange(1, i));
		if (!JDirectoryExists(dir) && !jMkDir(dir, mode, err))
		{
			return false;
		}
	}

	if (err != nullptr)
	{
		*err = JNoError();
	}
	return true;
}

// private

static bool
jMkDir
	(
	const JString&	dirName,
	const mode_t	mode,
	JError*			err
	)
{
	jclear_errno();
	if (mkdir(dirName.GetBytes(), mode) == 0)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EEXIST)
		{
			*err = JDirEntryAlreadyExists(dirName);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == EACCES)
		{
			*err = JAccessDenied(dirName);
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JBadPath(dirName);
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(dirName);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(dirName);
		}
		else if (e == ENOSPC)
		{
			*err = JFileSystemFull();
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
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

bool
JRenameDirEntry
	(
	const JString&	oldName,
	const JString&	newName,
	JError*			err
	)
{
	if (JNameUsed(newName))
	{
		*err = JDirEntryAlreadyExists(newName);
		return false;
	}
	else if (JSameDirEntry(oldName, newName))
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	jclear_errno();
	if (rename(oldName.GetBytes(), newName.GetBytes()) == 0)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EISDIR)
		{
			*err = JCantRenameFileToDirectory(oldName, newName);
		}
		else if (e == EXDEV)
		{
			*err = JCantRenameAcrossFilesystems();
		}
		else if (e == ENOTEMPTY)
		{
			*err = JCantRenameToNonemptyDirectory();
		}
		else if (e == EEXIST)
		{
			*err = JDirEntryAlreadyExists(newName);
		}
		else if (e == EBUSY)
		{
			*err = JFileBusy(newName);
		}
		else if (e == EINVAL)
		{
			*err = JDirectoryCantBeOwnChild();
		}
		else if (e == EMLINK)
		{
			*err = JTooManyLinks(oldName);
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(oldName, newName);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == EACCES || e == EPERM)
		{
			*err = JAccessDenied(oldName, newName);
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JBadPath(oldName, newName);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(oldName, newName);
		}
		else if (e == ENOSPC)
		{
			*err = JFileSystemFull();
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JChangeDirectory

	Changes the current working directory to the specified directory.

	Can return JAccessDenied, JSegFault, JNameTooLong, JBadPath,
	JNoKernelMemory, JComponentNotDirectory, JPathContainsLoop.

 ******************************************************************************/

bool
JChangeDirectory
	(
	const JString&	dirName,
	JError*			err
	)
{
	jclear_errno();
	if (chdir(dirName.GetBytes()) == 0)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EPERM || e == EACCES)
		{
			*err = JAccessDenied(dirName);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JBadPath(dirName);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(dirName);
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(dirName);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JRemoveDirectory

	Removes the specified directory.  This only works if the directory is empty.

	Can return JAccessDenied, JSegFault, JNameTooLong, JBadPath,
	JComponentNotDirectory, JDirectoryNotEmpty, JDirectoryBusy,
	JNoKernelMemory, JFileSystemReadOnly, JPathContainsLoop.

 ******************************************************************************/

bool
JRemoveDirectory
	(
	const JString&	dirName,
	JError*			err
	)
{
	jclear_errno();
	if (rmdir(dirName.GetBytes()) == 0)
	{
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EPERM || e == EACCES)
		{
			*err = JAccessDenied(dirName);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JBadPath(dirName);
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(dirName);
		}
		else if (e == ENOTEMPTY)
		{
			*err = JDirectoryNotEmpty(dirName);
		}
		else if (e == EBUSY)
		{
			*err = JDirectoryBusy(dirName);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(dirName);
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JKillDirectory

	Deletes the directory and everything in it.
	Returns true if successful.

	if !sync, *p will contain the process

 ******************************************************************************/

bool
JKillDirectory
	(
	const JString&	dirName,
	const bool	sync,
	JProcess**		p
	)
{
	if (p != nullptr)
	{
		*p = nullptr;
	}

	const JUtf8Byte* argv[] = {"rm", "-rf", dirName.GetBytes(), nullptr};
	if (sync)
	{
		const JError err = JExecute(argv, sizeof(argv), nullptr);
		if (err.OK())
		{
			return !JNameUsed(dirName);
		}
		else
		{
			return false;
		}
	}
	else
	{
		JSimpleProcess* sp;
		const JError err = JSimpleProcess::Create(&sp, argv, sizeof(argv), true);
		if (err.OK())
		{
			JThisProcess::Ignore(sp);
			if (p != nullptr)
			{
				*p = sp;
			}
			return true;
		}
		else
		{
			err.ReportIfError();
			return false;
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
	JGetTemporaryDirectoryChangeMutex().lock();

	JUtf8Byte buf[1024];
	JUtf8Byte* result = getcwd(buf, 1024);

	JString dirName;
	if (result == buf)
	{
		dirName = buf;
	}
	else if (!JGetHomeDirectory(&dirName) || !JChangeDirectory(dirName))
	{
		dirName = JGetRootDirectory();
		JChangeDirectory(dirName);
	}

	JGetTemporaryDirectoryChangeMutex().unlock();

	JAppendDirSeparator(&dirName);
	return dirName;
}

/******************************************************************************
 JGetHomeDirectory

	Returns true if the current user has a home directory.

 ******************************************************************************/

bool
JGetHomeDirectory
	(
	JString* homeDir
	)
{
	// try HOME environment variable

	JUtf8Byte* envHomeDir = getenv("HOME");
	if (!JString::IsEmpty(envHomeDir))
	{
		const JString dir(envHomeDir, JString::kNoCopy);
		if (JDirectoryExists(dir))
		{
			*homeDir = dir;
			JAppendDirSeparator(homeDir);
			return true;
		}
	}

	// try password information

	const JUtf8Byte* envUserName = getenv("USER");

	struct passwd* pw;
	if (envUserName != nullptr)
	{
		pw = getpwnam(envUserName);
	}
	else
	{
		pw = getpwuid( getuid() );
	}

	if (pw != nullptr)
	{
		const JString dir(pw->pw_dir, JString::kNoCopy);
		if (JDirectoryExists(dir))
		{
			*homeDir = dir;
			JAppendDirSeparator(homeDir);
			return true;
		}
	}

	// give up

	homeDir->Clear();
	return false;
}

/******************************************************************************
 JGetHomeDirectory

	Returns true if the specified user has a home directory.

 ******************************************************************************/

bool
JGetHomeDirectory
	(
	const JString&	user,
	JString*		homeDir
	)
{
	struct passwd* pw = getpwnam(user.GetBytes());
	if (pw != nullptr)
	{
		const JString dir(pw->pw_dir, JString::kNoCopy);
		if (JDirectoryExists(dir))
		{
			*homeDir = dir;
			JAppendDirSeparator(homeDir);
			return true;
		}
	}

	homeDir->Clear();
	return false;
}

/******************************************************************************
 JGetPrefsDirectory

	Returns true if the current user has a prefs directory.

 ******************************************************************************/

bool
JGetPrefsDirectory
	(
	JString* prefsDir
	)
{
	return JGetHomeDirectory(prefsDir);
}

/******************************************************************************
 JGetPrefsDirectory

	Returns true if the specified user has a home directory.

 ******************************************************************************/

bool
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

bool
JGetTempDirectory
	(
	JString* tempDir
	)
{
	// inside function to ensure initialization
	static bool theTempPathInitFlag = false;
	static JString theTempPath;

	if (!theTempPathInitFlag)
	{
		const JUtf8Byte* path = getenv("TMPDIR");
		if (!JString::IsEmpty(path) && JDirectoryWritable(path))
		{
			theTempPath.Set(path);
		}
		else if (P_tmpdir != nullptr && JDirectoryWritable(P_tmpdir))
		{
			theTempPath.Set(P_tmpdir);
		}
		else
		{
			theTempPath.Set("/tmp/");
		}

		JAppendDirSeparator(&theTempPath);
		theTempPathInitFlag = true;
	}

	*tempDir = theTempPath;
	return true;
}

/******************************************************************************
 JCreateTempDirectory

	Creates a unique directory in the specified directory.
	If path is empty, it uses the system's scratch directory.
	If prefix is empty, uses temp_dir_.

 ******************************************************************************/

bool
JCreateTempDirectory
	(
	const JString*	path,
	const JString*	prefix,
	JString*		fullName,
	JError*			err
	)
{
	// inside function to ensure initialization
	static const JString theTmpDirForError("/tmp");
	static const JString theTmpDirPrefix("temp_dir_");
	static const JString theTmpDirTemplate("XXXXXX");

	JString p;
	if (!JString::IsEmpty(path))
	{
		p = *path;
	}
	else if (!JGetTempDirectory(&p))
	{
		*err = JDirEntryDoesNotExist(theTmpDirForError);
		return false;
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
	if (d != nullptr)
	{
		*fullName = s;
		JAppendDirSeparator(fullName);
		jdelete [] s;
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	fullName->Clear();
	jdelete [] s;

	// EINVAL counts as unexpected

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == EEXIST)
		{
			*err = JDirEntryAlreadyExists(p);
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == EACCES)
		{
			*err = JAccessDenied(p);
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JBadPath(p);
		}
		else if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(p);
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else if (e == EROFS)
		{
			*err = JFileSystemReadOnly();
		}
		else if (e == ELOOP)
		{
			*err = JPathContainsLoop(p);
		}
		else if (e == ENOSPC)
		{
			*err = JFileSystemFull();
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}

/******************************************************************************
 JGetTrueName

	Returns true if name is a valid file or a valid directory.
	*trueName is the full, true path to name, without symbolic links.

 ******************************************************************************/

bool
JGetTrueName
	(
	const JString&	name,
	JString*		trueName
	)
{
	trueName->Clear();

	if (!JNameUsed(name))
	{
		return false;
	}

	// check if it is a directory

	else if (JDirectoryExists(name))
	{
		JGetTemporaryDirectoryChangeMutex().lock();

		const JString currPath = JGetCurrentDirectory();

		if (!JChangeDirectory(name))
		{
			JGetTemporaryDirectoryChangeMutex().unlock();
			return false;
		}

		*trueName = JGetCurrentDirectory();

		const bool ok = JChangeDirectory(currPath);
		assert( ok );

		JGetTemporaryDirectoryChangeMutex().unlock();
		return true;
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
			return false;
		}

		// resolve symbolic link

		JString target;
		if (JGetSymbolicLinkTarget(name, &target))
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
			return true;
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
	// inside function to ensure initialization
	static const JRegex trailingDotPattern("(?:/\\.)+$");
	static const JRegex multiSlashPattern("(?<!:)/{2,}");

	JStringIterator iter(path);
	if (path->StartsWith("file://"))
	{
		iter.SkipNext(7);
	}
	while (iter.Next(multiSlashPattern))
	{
		iter.ReplaceLastMatch("/");
	}

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	while (iter.Next("/./"))
	{
		iter.ReplaceLastMatch("/");
	}

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	if (iter.Next(trailingDotPattern))		// max 1 match
	{
		iter.RemoveLastMatch();
	}
}

/******************************************************************************
 JIsAbsolutePath

 ******************************************************************************/

bool
JIsAbsolutePath
	(
	const JString& path
	)
{
	assert( !path.IsEmpty() );
	const JUtf8Character c = path.GetFirstCharacter();
	return c == '/' || c == '~';
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

bool
JIsRootDirectory
	(
	const JString& dirName
	)
{
	assert( !dirName.IsEmpty() );
	return dirName == "/";
}

/*****************************************************************************
 JConvertToAbsolutePath

	Attempts to convert 'path' to a full path.  Returns true if successful.

	If path begins with '/', there is nothing to do.
	If path begins with '~', the user's home directory is inserted.
	Otherwise, if base is not empty, it is prepended.
	Otherwise, the result of JGetCurrentDirectory() is prepended.

	As a final check, it calls JNameUsed() to check that the result exists.
	(This allows one to pass in a path+name as well as only a path.)

	If it is possible to expand the path, *result will contain the expanded
	path, even if JNameUsed() fails

 ******************************************************************************/

bool
JConvertToAbsolutePath
	(
	const JString&	path,
	const JString&	base,
	JString*		result
	)
{
	assert( !path.IsEmpty() && result != nullptr );

	bool ok = true;
	const JUtf8Character c = path.GetFirstCharacter();
	if (c == '/')
	{
		*result = path;
	}
	else if (c == '~')
	{
		ok = JExpandHomeDirShortcut(path, result);
	}
	else if (!base.IsEmpty())
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
		return false;
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

	bool hadTDS = true;
	if (path.GetLastCharacter() != '/')
	{
		path.Append("/");
		hadTDS = false;
	}

	JSize matchLength = JString::CalcCharacterMatchLength(path, base);

	JStringIterator pathIter(&path);
	if (!hadTDS)
	{
		pathIter.MoveTo(JStringIterator::kStartAtEnd, 0);
		pathIter.RemovePrev();
	}

	JStringIterator baseIter(&base, JStringIterator::kStartAfterChar, matchLength);
	const bool found = baseIter.Prev("/");
	assert( found );
	if (baseIter.AtBeginning())
	{
		return path;
	}
	matchLength = baseIter.GetNextCharacterIndex();

	baseIter.RemoveAllPrev();
	baseIter.RemoveNext();

	pathIter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	pathIter.SkipNext(matchLength);
	pathIter.RemoveAllPrev();
	pathIter.Invalidate();

	if (base.IsEmpty())
	{
		path.Prepend("./");
		return path;
	}

	// The number of remaining directory separators in base
	// is the number of levels to go up.

	baseIter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	while (baseIter.Next("/"))
	{
		path.Prepend("../");
	}

	return path;
}

/*****************************************************************************
 JExpandHomeDirShortcut

	If the given path begins with ~ or ~x, this is replaced by the appropriate
	home directory, if it exists.  Otherwise, false is returned and *result
	is empty.

	If homeDir != nullptr, it is set to the home directory that was specified
	by the ~.  If homeLength != nullptr it is set to the number of characters
	at the start of path that specified the home directory.

	This function does not check that the resulting expanded path is valid.

	If path doesn't begin with ~, returns true, *result = path, *homeDir
	is empty, and *homeLength = 0.

 ******************************************************************************/

bool
JExpandHomeDirShortcut
	(
	const JString&	path,
	JString*		result,
	JString*		homeDir,
	JSize*			homeLength
	)
{
	assert( !path.IsEmpty() );
	assert( result != nullptr );

	bool ok = true;
	if (path == "~")
	{
		ok = JGetHomeDirectory(result);
		if (ok && homeDir != nullptr)
		{
			*homeDir = *result;
		}
		if (ok && homeLength != nullptr)
		{
			*homeLength = 1;
		}
	}
	else if (path.StartsWith("~/"))
	{
		ok = JGetHomeDirectory(result);
		if (ok && homeDir != nullptr)
		{
			*homeDir = *result;
		}
		if (ok && homeLength != nullptr)
		{
			*homeLength = 2;
		}
		if (ok && path.GetCharacterCount() > 2)
		{
			JStringIterator iter(path);
			iter.SkipNext(2);
			iter.BeginMatch();
			iter.MoveTo(JStringIterator::kStartAtEnd, 0);
			const JStringMatch& m = iter.FinishMatch();
			*result = JCombinePathAndName(*result, m.GetString());
		}
	}
	else if (path.StartsWith("~"))
	{
		JStringIterator iter(path, JStringIterator::kStartAfterChar, 1);
		iter.BeginMatch();
		iter.Next("/");
		const JString userName = iter.FinishMatch().GetString();

		ok = JGetHomeDirectory(userName, result);
		if (ok && homeDir != nullptr)
		{
			*homeDir = *result;
		}
		if (ok && homeLength != nullptr)
		{
			*homeLength = iter.GetPrevCharacterIndex();
		}
		if (ok && !iter.AtEnd())
		{
			iter.BeginMatch();
			iter.MoveTo(JStringIterator::kStartAtEnd, 0);
			*result = JCombinePathAndName(*result, iter.FinishMatch().GetString());
		}
	}
	else
	{
		*result = path;
		if (homeDir != nullptr)
		{
			homeDir->Clear();
		}
		if (homeLength != nullptr)
		{
			*homeLength = 0;
		}
	}

	if (ok)
	{
		return true;
	}
	else
	{
		result->Clear();
		if (homeDir != nullptr)
		{
			homeDir->Clear();
		}
		if (homeLength != nullptr)
		{
			*homeLength = 0;
		}
		return false;
	}
}

/*******************************************************************************
 JGetSymbolicLinkTarget

 *******************************************************************************/

bool
JGetSymbolicLinkTarget
	(
	const JString&	linkFullName,
	JString*		targetFullName,
	JError*			err
	)
{
	const JSize kBufferSize = 10000;
	char buf[ kBufferSize ];
	const long linkNameSize = readlink(linkFullName.GetBytes(), buf, kBufferSize-1);
	if (linkNameSize != -1)
	{
		targetFullName->Set(buf, linkNameSize);
		if (err != nullptr)
		{
			*err = JNoError();
		}
		return true;
	}

	targetFullName->Clear();

	if (err != nullptr)
	{
		const int e = jerrno();
		if (e == ENOTDIR)
		{
			*err = JComponentNotDirectory(linkFullName);
		}
		else if (e == ENAMETOOLONG)
		{
			*err = JNameTooLong();
		}
		else if (e == ENOENT)
		{
			*err = JDirEntryDoesNotExist(linkFullName);
		}
		else if (e == EPERM || e == EACCES)
		{
			*err = JAccessDenied(linkFullName);
		}
		else if (e == ELOOP)
		{
			*err = JTooManyLinks(linkFullName);
		}
		else if (e == EINVAL)
		{
			*err = JNotSymbolicLink(linkFullName);
		}
		else if (e == EIO)
		{
			*err = JGeneralIO();
		}
		else if (e == EFAULT)
		{
			*err = JSegFault();
		}
		else if (e == ENOMEM)
		{
			*err = JNoKernelMemory();
		}
		else
		{
			*err = JUnexpectedError(e);
		}
	}
	return false;
}
