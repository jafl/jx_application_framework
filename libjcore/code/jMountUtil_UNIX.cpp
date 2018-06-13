/******************************************************************************
 jMountUtil_UNIX.cpp

	Utility functions for mounting UNIX file system.

	Copyright (C) 2000-2001 by John Lindal.

 ******************************************************************************/

#include <jMountUtil.h>
#include <jDirUtil.h>
#include <JThisProcess.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jSysUtil.h>

#if defined __OpenBSD__
// Unfortunately, <sys/mount.h> does not compile cleanly with C++ due to
// the use of the "export" keyword as a struct member name.  Use a #define
// to work around the problem
#define export Export
#endif

#if defined _J_OSX
	#include <JProcess.h>
	#include <jStreamUtil.h>
	#include <JRegex.h>
	#include <sys/param.h>
	#include <sys/ucred.h>
	#include <sys/mount.h>
	#define JMOUNT_OSX
#elif defined __FreeBSD__ || defined __OpenBSD__
	#include <fstab.h>
	#include <sys/param.h>
	#include <sys/ucred.h>
	#include <sys/mount.h>
	#define JMOUNT_BSD
#elif defined _J_SUNOS || defined _J_IRIX
	#include <sys/vfstab.h>
	#include <sys/mnttab.h>
	#define JMOUNT_SYSV
#elif defined _J_CYGWIN
	#include <mntent.h>
#else
	#include <fstab.h>
	#include <mntent.h>
#endif

#include <jAssert.h>

#if defined JMOUNT_OSX
static const JString kMountCmd("mount", kJFalse);
#elif defined JMOUNT_BSD
static const JString kAvailInfoName(_PATH_FSTAB, kJFalse);
#elif defined JMOUNT_SYSV
static const JString kAvailInfoName(VFSTAB, kJFalse);
static const JUtf8Byte* kMountedInfoName = MNTTAB;
#elif defined _J_CYGWIN
static const JUtf8Byte* kMountedInfoName = MOUNTED;
#else
static const JString kAvailInfoName(_PATH_FSTAB, kJFalse);
static const JUtf8Byte* kMountedInfoName = _PATH_MOUNTED;
#endif

/******************************************************************************
 JGetUserMountPointList

	Returns a list of mount points and types that satisfy the following
	conditions:

		spec	path	type	opts
		----	----	----	----
				/						=> always ignore
						swap			=> always ignore
						iso9660			=> kJCDROM
		hd*								=> kJHardDisk (IDE)
		sd*								=> kJHardDisk (SCSI)
		fd*								=> kJFloppyDisk

		FreeBSD differences:
						cd9660			=> kJCDROM
		ad*								=> kJHardDisk (IDE)
		da*								=> kJHardDisk (SCSI)

	Sample OSX output:

		/dev/disk1 on / (hfs, local, journaled)
		/dev/disk3s1 on /Volumes/XFER (msdos, local, nodev, nosuid, noowners)

	Unless the current uid is zero (root), opts must include "user" or
	"users".  The other possibility is that the *device* must be owned by
	the user, and opts must include "owner".

	If state != nullptr, its value is compared with the mod time of /etc/fstab
	or equivalent.  If the state is the same, nothing is done.

	Returns kJTrue if /etc/fstab was read in, i.e., if state was nullptr or
	outdated.

 ******************************************************************************/

#if defined JMOUNT_OSX

static const JRegex theLinePattern = "^(/[^\\s]+)\\s+on\\s+(/[^)]+?)\\s+\\((.+)\\)";

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	JProcess* p;
	int outFD;
	const JError err = JProcess::Create(&p, kMountCmd,
										kJIgnoreConnection, nullptr,
										kJCreatePipe, &outFD,
										kJIgnoreConnection, nullptr);
	if (!err.OK())
		{
		if (state != nullptr)
			{
			jdelete state->mountCmdOutput;
			state->mountCmdOutput = nullptr;
			}
		return kJFalse;
		}

	JString mountData;
	JReadAll(outFD, &mountData);

	p->WaitUntilFinished();
	const JBoolean success = p->SuccessfulFinish();
	jdelete p;
	p = nullptr;

	if (!success)
		{
		if (state != nullptr)
			{
			jdelete state->mountCmdOutput;
			state->mountCmdOutput = nullptr;
			}
		return kJFalse;
		}

	if (state != nullptr && state->mountCmdOutput != nullptr &&
		mountData == *(state->mountCmdOutput))
		{
		return kJFalse;
		}

	list->CleanOut();
	if (state != nullptr && state->mountCmdOutput == nullptr)
		{
		state->mountCmdOutput = jnew JString(mountData);
		assert( state->mountCmdOutput != nullptr );
		}
	else if (state != nullptr)
		{
		*(state->mountCmdOutput) = mountData;
		}

	JString options;
	ACE_stat stbuf;

	JStringIterator iter(mountData);
	while (iter.Next(theLinePattern))
		{
		const JStringMatch& m = iter.GetLastMatch();

		options = m.GetSubstring(3);
		if (options.Contains("nobrowse"))
			{
			continue;
			}

		JString* path = jnew JString(m.GetSubstring(2));
		assert( path != nullptr );
		JString* devicePath = jnew JString(m.GetSubstring(1));
		assert( devicePath != nullptr );

		const JMountType type =
			JGetUserMountPointType(*path, *devicePath, JString::empty);
		if (type == kJUnknownMountType ||
			ACE_OS::stat(path->GetBytes(), &stbuf) != 0)
			{
			jdelete path;
			jdelete devicePath;
			continue;
			}

		JFileSystemType fsType = kOtherFSType;
		if (options.Contains("msdos"))
			{
			fsType = kVFATType;
			}

		list->AppendElement(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
		}

	return kJTrue;
}

#elif defined JMOUNT_BSD

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	time_t t;
	if (state != nullptr &&
		(!(JGetModificationTime(_PATH_FSTAB, &t)).OK() ||
		 t == state->modTime))
		{
		return kJFalse;
		}

	list->CleanOut();
	if (state != nullptr)
		{
		state->modTime = t;
		}

	endfsent();

	const JBoolean isRoot = JI2B( getuid() == 0 );

	if (isRoot)
		{
		ACE_stat stbuf;
		while (const fstab* info = getfsent())
			{
			if (JIsRootDirectory(info->fs_file) ||
				strcmp(info->fs_type, FSTAB_SW) == 0)	// swap partition
				{
				continue;
				}

			const JMountType type =
				JGetUserMountPointType(info->fs_file, info->fs_spec, info->fs_vfstype);
			if (type == kJUnknownMountType ||
				ACE_OS::stat(info->fs_file, &stbuf) != 0)
				{
				continue;
				}

			JFileSystemType fsType = kOtherFSType;
			if (options.Contains("vfat"))
				{
				fsType = kVFATType;
				}

			JFileSystemType fsType = kOtherFSType;
			if (JString::Compare(info->fs_vfstype, "vfat", kJFalse) == 0)
				{
				fsType = kVFATType;
				}

			JString* path = jnew JString(info->fs_file);
			assert( path != nullptr );
			JString* devicePath = jnew JString(info->fs_spec);
			assert( devicePath != nullptr );
			list->AppendElement(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
			}
		}

	endfsent();
	return kJTrue;
}

#elif defined JMOUNT_SYSV

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	time_t t;
	if (state != nullptr &&
		(!(JGetModificationTime(kAvailInfoName, &t)).OK() ||
		 t == state->modTime))
		{
		return kJFalse;
		}

	list->CleanOut();
	if (state != nullptr)
		{
		state->modTime = t;
		}

	FILE* f = fopen(kAvailInfoName.GetBytes(), "r");
	if (f == nullptr)
		{
		return kJTrue;	// did clear list
		}

	const JBoolean isRoot = JI2B( getuid() == 0 );

	if (isRoot)
		{
		ACE_stat stbuf;
		vfstab info;
		while (getvfsent(f, &info) == 0)
			{
			if (JIsRootDirectory(info.vfs_mountp) ||
				strcmp(info.vfs_fstype, "swap") == 0)
				{
				continue;
				}

			const JMountType type =
				JGetUserMountPointType(info.vfs_mountp, info.vfs_special, info.vfs_fstype);
			if (type == kJUnknownMountType ||
				ACE_OS::stat(info.vfs_mountp, &stbuf) != 0)
				{
				continue;
				}

			JFileSystemType fsType = kOtherFSType;
			if (JString::Compare(info.fs_vfstype, "vfat", kJFalse) == 0)
				{
				fsType = kVFATType;
				}

			JString* path = jnew JString(info.vfs_mountp);
			assert( path != nullptr );
			JString* devicePath = jnew JString(info.vfs_special);
			assert( devicePath != nullptr );
			list->AppendElement(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
			}
		}

	fclose(f);
	return kJTrue;
}

#elif defined _J_CYGWIN

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	list->CleanOut();
	return kJFalse;
}

#else

inline int
jUserOwnsDevice
	(
	const JString& name
	)
{
	uid_t uid;
	return ((JGetOwnerID(name, &uid)).OK() && uid == getuid());
}

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	time_t t;
	if (state != nullptr &&
		(!(JGetModificationTime(kAvailInfoName, &t)).OK() ||
		 t == state->modTime))
		{
		return kJFalse;
		}

	list->CleanOut();
	if (state != nullptr)
		{
		state->modTime = t;
		}

	FILE* f = setmntent(kAvailInfoName.GetBytes(), "r");
	if (f == nullptr)
		{
		return kJTrue;	// did clear list
		}

	const JBoolean isRoot = JI2B( getuid() == 0 );

	ACE_stat stbuf;
	while (const mntent* info = getmntent(f))
		{
		if (!(isRoot ||
			  hasmntopt(info, "user") != nullptr  ||
			  hasmntopt(info, "users") != nullptr ||
			  hasmntopt(info, "pamconsole") != nullptr ||
			  (jUserOwnsDevice(info->mnt_fsname) &&
			   hasmntopt(info, "owner") != nullptr)) ||
			JIsRootDirectory(info->mnt_dir) ||
			strcmp(info->mnt_type, MNTTYPE_SWAP) == 0)
			{
			continue;
			}

		const JMountType type =
			JGetUserMountPointType(info->mnt_dir, info->mnt_fsname, info->mnt_type);
		if (type == kJUnknownMountType ||
			ACE_OS::stat(info->mnt_dir, &stbuf) != 0)
			{
			continue;
			}

		JFileSystemType fsType = kOtherFSType;
		if (JString::Compare(info->mnt_type, "vfat", kJFalse) == 0)
			{
			fsType = kVFATType;
			}

		JString* path = jnew JString(info->mnt_dir);
		assert( path != nullptr );
		JString* devicePath = jnew JString(info->mnt_fsname);
		assert( devicePath != nullptr );
		list->AppendElement(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
		}

	endmntent(f);
	return kJTrue;
}

#endif

/******************************************************************************
 JGetUserMountPointType

	Returns the type of the specified mount point.

 ******************************************************************************/

#if defined JMOUNT_OSX

JMountType
JGetUserMountPointType
	(
	const JString& path,
	const JString& device,
	const JString& fsType
	)
{
	return kJHardDisk;
}

#elif defined JMOUNT_BSD

JMountType
JGetUserMountPointType
	(
	const JString& path,
	const JString& device,
	const JString& fsType
	)
{
	if (strstr(fsType, "cd9660") != nullptr)
		{
		return kJCDROM;
		}
	else if (strncmp("/dev/ad", device, 7) == 0)		// IDE
		{
		return kJHardDisk;
		}
	else if (strncmp("/dev/da", device, 7) == 0)		// SCSI
		{
		return kJHardDisk;
		}
	else if (strncmp("/dev/fd", device, 7) == 0)
		{
		return kJFloppyDisk;
		}
	else if (JIsRootDirectory(path))
		{
		return kJHardDisk;
		}
	else
		{
		return kJUnknownMountType;
		}
}

#elif defined JMOUNT_SYSV

JMountType
JGetUserMountPointType
	(
	const JString& path,
	const JString& device,
	const JString& fsType
	)
{
	if (strstr(fsType, "iso9660") != nullptr)
		{
		return kJCDROM;
		}
	else if (strstr(fsType, "ufs") != nullptr)
		{
		return kJHardDisk;
		}
	else if (strncmp("/dev/fd", device, 7) == 0)
		{
		return kJFloppyDisk;
		}
	else if (JIsRootDirectory(path))
		{
		return kJHardDisk;
		}
	else
		{
		return kJUnknownMountType;
		}
}

#elif defined _J_CYGWIN

JMountType
JGetUserMountPointType
	(
	const JString& path,
	const JString& device,
	const JString& fsType
	)
{
	return kJHardDisk;
}

#else

static const JRegex devIndexPattern = "[0-9]+$";

inline void
jReadLine
	(
	const int	fd,
	JString&	buffer
	)
{
	JIndex i = 0;
	JUtf8Byte c;
	while (read(fd, &c, 1) == 1)
		{
		if (c == '\n')
			{
			break;
			}
		buffer[i] = c;
		i++;
		}
	buffer[i] = '\0';
}

JMountType
JGetUserMountPointType
	(
	const JString& path,
	const JString& device,
	const JString& fsType
	)
{
	if (strstr(fsType, "iso9660") != nullptr)
		{
		return kJCDROM;
		}
	else if (strstr(path, "floppy") != nullptr)
		{
		return kJFloppyDisk;	// hot-swappable drives are often /dev/sd*
		}
	else if (strncmp("/dev/hd", device, 7) == 0)		// IDE
		{
		return kJHardDisk;
		}
	else if (strncmp("/dev/sd", device, 7) == 0)		// SCSI
		{
		return kJHardDisk;
		}
	else if (strncmp("/dev/fd", device, 7) == 0)
		{
		return kJFloppyDisk;
		}
	else if (JIsRootDirectory(path))
		{
		return kJHardDisk;
		}
	else
		{
		return kJUnknownMountType;
		}
}

#endif

/******************************************************************************
 JIsMounted

	device can be nullptr

 ******************************************************************************/

#if defined JMOUNT_BSD || defined JMOUNT_OSX

JBoolean
JIsMounted
	(
	const JString&		path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path.GetBytes(), &stbuf1) != 0)
		{
		return kJFalse;
		}

	JBoolean isMounted = kJFalse;

	struct statfs* info;
	const JSize count = getmntinfo(&info, MNT_WAIT);

	JString p;
	for (JIndex i=0; i<count; i++)
		{
		if (strcmp(info[i].f_mntonname, "/") != 0 &&
			stat(info[i].f_mntonname, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
			{
			isMounted = kJTrue;
			if (writable != nullptr)
				{
				*writable = JNegate((info[i].f_flags & MNT_RDONLY) != 0);
				}
			if (isTop != nullptr)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != nullptr)
				{
				*device = info[i].f_mntfromname;
				}
			if (fsType != nullptr)
				{
				*fsType = kOtherFSType;
				if (JString::Compare(info[i].f_fstypename, "vfat", kJFalse) == 0 ||	// UNIX
					JString::Compare(info[i].f_fstypename, "msdos", kJFalse) == 0)	// OSX
					{
					*fsType = kVFATType;
					}
				}
			if (fsTypeString != nullptr)
				{
				*fsTypeString = info[i].f_fstypename;
				}
			break;
			}
		}

	return isMounted;
}

#elif defined JMOUNT_SYSV

JBoolean
JIsMounted
	(
	const JString&		path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
		{
		return kJFalse;
		}

	FILE* f = fopen(kMountedInfoName, "r");
	if (f == nullptr)
		{
		return kJFalse;
		}

	JBoolean isMounted = kJFalse;
	JString p;
	mnttab info;
	while (getmntent(f, &info) == 0)
		{
		if (strcmp(info.mnt_mountp, "/") != 0 &&
			stat(info.mnt_mountp, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
			{
			isMounted = kJTrue;
			if (writable != nullptr)
				{
				*writable = JI2B(hasmntopt(&info, "ro") == nullptr);
				}
			if (isTop != nullptr)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != nullptr)
				{
				*device = info.mnt_special;
				}
			if (fsType != nullptr)
				{
				*fsType = kOtherFSType;
				if (JString::Compare(info.mnt_fstype, "vfat", kJFalse) == 0)
					{
					*fsType = kVFATType;
					}
				}
			if (fsTypeString != nullptr)
				{
				*fsTypeString = info.mnt_fstype;
				}
			break;
			}
		}

	fclose(f);
	return isMounted;
}

#elif defined _J_CYGWIN

JBoolean
JIsMounted
	(
	const JString&		path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
		{
		return kJFalse;
		}

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == nullptr)
		{
		return kJFalse;
		}

	JBoolean isMounted = kJFalse;
	JString p;
	while (const mntent* info = getmntent(f))
		{
		if (strcmp(info->mnt_dir, "/") != 0 &&
			stat(info->mnt_dir, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
			{
			isMounted = kJTrue;
			if (writable != nullptr)
				{
				*writable = kJTrue;
				}
			if (isTop != nullptr)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != nullptr)
				{
				*device = info->mnt_fsname;
				}
			if (fsType != nullptr)
				{
				*fsType = kOtherFSType;
				if (JString::Compare(info->mnt_type, "vfat", kJFalse) == 0)
					{
					*fsType = kVFATType;
					}
				}
			if (fsTypeString != nullptr)
				{
				*fsTypeString = info->mnt_type;
				}
			break;
			}
		}

	endmntent(f);
	return isMounted;
}

#else

JBoolean
JIsMounted
	(
	const JString&		path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
		{
		return kJFalse;
		}

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == nullptr)
		{
		return kJFalse;
		}

	JBoolean isMounted = kJFalse;
	JString p;
	while (const mntent* info = getmntent(f))
		{
		if (strcmp(info->mnt_dir, "/") != 0 &&
			stat(info->mnt_dir, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
			{
			isMounted = kJTrue;
			if (writable != nullptr)
				{
				*writable = JI2B(hasmntopt(info, MNTOPT_RO) == nullptr);
				}
			if (isTop != nullptr)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != nullptr)
				{
				*device = info->mnt_fsname;
				}
			if (fsType != nullptr)
				{
				*fsType = kOtherFSType;
				if (JString::Compare(info->mnt_type, "vfat", kJFalse) == 0)
					{
					*fsType = kVFATType;
					}
				}
			if (fsTypeString != nullptr)
				{
				*fsTypeString = info->mnt_type;
				}
			break;
			}
		}

	endmntent(f);
	return isMounted;
}

#endif

/******************************************************************************
 JFindUserMountPoint

	Returns kJTrue if the given path is on a mounted partition.

 ******************************************************************************/

JBoolean
JFindUserMountPoint
	(
	const JString&			path,
	const JMountPointList&	list,
	JIndex*					index
	)
{
	*index = 0;

	struct stat stbuf;
	if (stat(path.GetBytes(), &stbuf) != 0)
		{
		return kJFalse;
		}

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JMountPoint pt = list.GetElement(i);
		if (pt.device == stbuf.st_dev)
			{
			*index = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 JMount

	To determine whether or not this function succeeded, call it with
	block=kJTrue and then check JIsMounted().

	If mount = kJFalse, the file system is unmounted instead.

 ******************************************************************************/

void
JMount
	(
	const JString&	path,
	const JBoolean	mount,
	const JBoolean	block
	)
{
	const JUtf8Byte* argv[] = { mount ? "mount" : "umount", path.GetBytes(), nullptr };
	pid_t pid;
	JExecute(argv, sizeof(argv), block ? (pid_t*) nullptr : &pid,
			 kJIgnoreConnection, nullptr,
			 kJTossOutput, nullptr,
			 kJTossOutput, nullptr);
}

/******************************************************************************
 jGetFullHostName (local)

 ******************************************************************************/

inline void
jGetFullHostName
	(
	JString* host
	)
{
	if (!host->Contains("."))
		{
		const JString localhost = JGetHostName();
		JStringIterator iter(localhost);
		if (iter.Next("."))
			{
			iter.SkipPrev();	// include dot
			iter.BeginMatch();
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			*host += iter.FinishMatch().GetString();
			}
		}
}

/******************************************************************************
 JTranslateLocalToRemote

	Translate local path to remote host and path, if it is mounted.

 ******************************************************************************/

inline JBoolean
jTranslateLocalToRemote1
	(
	const JString&		localPath,
	const JUtf8Byte*	mountDev,
	const JUtf8Byte*	mountDir,
	JBoolean*			found,
	JString*			host,
	JString*			remotePath
	)
{
	const JString dir(mountDir, kJFalse);
	if (!JIsSamePartition(localPath, dir))
		{
		return kJFalse;
		}

	const JString dev(mountDev, kJFalse);
	JStringIterator iter(dev);
	iter.BeginMatch();
	if (iter.Next(":/") && iter.GetPrevCharacterIndex() > 2)
		{
		*host = iter.FinishMatch().GetString();

		#ifdef _J_CYGWIN
		if (host->GetLength() == 1 &&
			'A' <= host->GetFirstCharacter() && host->GetFirstCharacter() <= 'Z')
			{
			*host       = JGetHostName();
			*remotePath = localPath;
			JCleanPath(remotePath);
			*found = kJTrue;
			return kJTrue;
			}
		#endif

		jGetFullHostName(host);

		iter.SkipPrev();	// include leading slash
		iter.BeginMatch();
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		*remotePath = iter.FinishMatch().GetString();
		JAppendDirSeparator(remotePath);

		JStringIterator iter2(localPath, kJIteratorStartAfter, dir.GetCharacterCount());
		iter2.BeginMatch();
		iter2.MoveTo(kJIteratorStartAtEnd, 0);
		*remotePath += iter2.FinishMatch().GetString();
		JCleanPath(remotePath);

		*found = kJTrue;
		}

	return kJTrue;
}

#if defined JMOUNT_BSD || defined JMOUNT_OSX

JBoolean
JTranslateLocalToRemote
	(
	const JString&	localPathStr,
	JString*		host,
	JString*		remotePath
	)
{
	host->Clear();
	remotePath->Clear();

	JString localPath;
	if (!JGetTrueName(localPathStr, &localPath))
		{
		return kJFalse;
		}

	struct statfs* info;
	const JSize count = getmntinfo(&info, MNT_WAIT);

	JBoolean found = kJFalse;
	for (JIndex i=0; i<count; i++)
		{
		if (jTranslateLocalToRemote1(localPath, info[i].f_mntfromname,
									 info[i].f_mntonname,
									 &found, host, remotePath))
			{
			break;		// localPath only on one partition
			}
		}

	return found;
}

#elif defined JMOUNT_SYSV

JBoolean
JTranslateLocalToRemote
	(
	const JString&	localPathStr,
	JString*		host,
	JString*		remotePath
	)
{
	host->Clear();
	remotePath->Clear();

	JString localPath;
	FILE* f = fopen(kMountedInfoName, "r");
	if (f == nullptr || !JGetTrueName(localPathStr, &localPath))
		{
		return kJFalse;
		}

	JBoolean found = kJFalse;
	mnttab info;
	while (getmntent(f, &info) == 0)
		{
		if (jTranslateLocalToRemote1(localPath, info.mnt_special,
									 info.mnt_mountp,
									 &found, host, remotePath))
			{
			break;		// localPath only on one partition
			}
		}

	fclose(f);
	return found;
}

#else

JBoolean
JTranslateLocalToRemote
	(
	const JString&	localPathStr,
	JString*		host,
	JString*		remotePath
	)
{
	host->Clear();
	remotePath->Clear();

	JString localPath;
	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == nullptr || !JGetTrueName(localPathStr, &localPath))
		{
		return kJFalse;
		}

	JBoolean found = kJFalse;
	while (const mntent* info = getmntent(f))
		{
		if (jTranslateLocalToRemote1(localPath, info->mnt_fsname,
									 info->mnt_dir,
									 &found, host, remotePath))
			{
			break;		// localPath only on one partition
			}
		}

	endmntent(f);
	return found;
}

#endif

/******************************************************************************
 JTranslateRemoteToLocal

	Translate remote host and path to local path, if it is mounted.

	Note that this will not work if the remote path is a symbolic link.
	We can't convert that to a true path.

 ******************************************************************************/

inline JBoolean
jTranslateRemoteToLocal1
	(
	const JString&		host,
	const JString&		remotePath,
	const JUtf8Byte*	mountDev,
	const JUtf8Byte*	mountDir,
	JString*			localPath
	)
{
	const JString dev(mountDev, kJFalse);
	JStringIterator iter(dev);
	iter.BeginMatch();
	if (iter.Next(":/") && iter.GetPrevCharacterIndex() > 2)
		{
		JString h = iter.FinishMatch().GetString();
		jGetFullHostName(&h);

		iter.SkipPrev();	// include leading slash
		iter.BeginMatch();
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		JString p = iter.FinishMatch().GetString();
		JAppendDirSeparator(&p);		// force complete name when check BeginsWith()

		if (host == h && remotePath.BeginsWith(p))
			{
			*localPath = remotePath;

			JStringIterator iter2(localPath);
			iter2.BeginMatch();
			iter2.MoveTo(kJIteratorStartAfter, p.GetCharacterCount()-1);
			iter2.FinishMatch();
			iter2.ReplaceLastMatch(JString(mountDir, kJFalse));

			JCleanPath(localPath);
			return kJTrue;
			}
		}

	return kJFalse;
}

#if defined JMOUNT_BSD || defined JMOUNT_OSX

JBoolean
JTranslateRemoteToLocal
	(
	const JString&	hostStr,
	const JString&	remotePath,
	JString*		localPath
	)
{
	localPath->Clear();

	struct statfs* info;
	const JSize count = getmntinfo(&info, MNT_WAIT);

	JBoolean found = kJFalse;
	for (JIndex i=0; i<count; i++)
		{
		if (jTranslateRemoteToLocal1(hostStr, remotePath, info[i].f_mntfromname,
									 info[i].f_mntonname, localPath))
			{
			found = kJTrue;
			break;
			}
		}

	return found;
}

#elif defined JMOUNT_SYSV

JBoolean
JTranslateRemoteToLocal
	(
	const JString&	host,
	const JString&	remotePathStr,
	JString*		localPath
	)
{
	localPath->Clear();

	FILE* f = fopen(kMountedInfoName, "r");
	if (f == nullptr)
		{
		return kJFalse;
		}

	const JString remotePath = remotePathStr;

	JBoolean found = kJFalse;
	mnttab info;
	while (getmntent(f, &info) == 0)
		{
		if (jTranslateRemoteToLocal1(host, remotePath, info.mnt_special,
									 info.mnt_mountp, localPath))
			{
			found = kJTrue;
			break;
			}
		}

	fclose(f);
	return found;
}

#else

JBoolean
JTranslateRemoteToLocal
	(
	const JString&	host,
	const JString&	remotePathStr,
	JString*		localPath
	)
{
	localPath->Clear();

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == nullptr)
		{
		return kJFalse;
		}

	const JString remotePath = remotePathStr;

	JBoolean found = kJFalse;
	while (const mntent* info = getmntent(f))
		{
		if (jTranslateRemoteToLocal1(host, remotePath, info->mnt_fsname,
									 info->mnt_dir, localPath))
			{
			found = kJTrue;
			break;
			}
		}

	endmntent(f);
	return found;
}

#endif

/******************************************************************************
 JFormatPartition

	Erases the specified partition and converts it to the specified type.

		Linux: ext2
		DOS:   msdos

	Others may be supported on various systems.

	*** Use with extreme caution!

 ******************************************************************************/

JError
JFormatPartition
	(
	const JString&	path,
	const JString&	type,
	JProcess**		process
	)
{
	JBoolean writable, isTop;
	JString device;
	if (JIsMounted(path, &writable, &isTop, &device) && isTop)
		{
		JMount(path, kJFalse, kJTrue);
		if (!JIsMounted(path))
			{
			const JUtf8Byte* argv[] = { "xterm", "-T", "Format disk", "-n", "Format disk",
										"-e", "/sbin/mkfs",
											"-t", type.GetBytes(),
											"-c", device.GetBytes(),
										nullptr };
			const JError err = JProcess::Create(process, argv, sizeof(argv));
			if (err.OK())
				{
				JThisProcess::Ignore(*process);
				}
			return err;
			}
		}

	*process = nullptr;
	return JAccessDenied(path);
}

/******************************************************************************
 JIsSamePartition

	Returns kJTrue if the two directories are on the same partition.

 ******************************************************************************/

JBoolean
JIsSamePartition
	(
	const JString& path1,
	const JString& path2
	)
{
	ACE_stat stbuf1, stbuf2;
	return JI2B(ACE_OS::stat(path1.GetBytes(), &stbuf1) == 0 &&
				ACE_OS::stat(path2.GetBytes(), &stbuf2) == 0 &&
				stbuf1.st_dev == stbuf2.st_dev);
}

/******************************************************************************
 JMountPointList class

 ******************************************************************************/

JMountPointList::JMountPointList
	(
	const JPtrArrayT::CleanUpAction action
	)
	:
	JArray<JMountPoint>(),
	itsCleanUpAction(action)
{
}

JMountPointList::~JMountPointList()
{
	if (itsCleanUpAction == JPtrArrayT::kDeleteAll)
		{
		DeleteAll();
		}
}

void
JMountPointList::CleanOut()
{
	if (itsCleanUpAction == JPtrArrayT::kDeleteAll)
		{
		DeleteAll();
		}
	else
		{
		RemoveAll();
		}
}

void
JMountPointList::DeleteAll()
{
	for (const JMountPoint& info : *this)
		{
		jdelete info.path;
		jdelete info.devicePath;
		}

	RemoveAll();
}

JPtrArrayT::CleanUpAction
JMountPointList::GetCleanUpAction()
	const
{
	return itsCleanUpAction;
}

void
JMountPointList::SetCleanUpAction
	(
	const JPtrArrayT::CleanUpAction action
	)
{
	itsCleanUpAction = action;
}

/******************************************************************************
 JMountState

 ******************************************************************************/

JMountState::~JMountState()
{
#if defined JMOUNT_OSX
	jdelete mountCmdOutput;
#endif
}