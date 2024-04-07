/******************************************************************************
 jMountUtil_UNIX.cpp

	Utility functions for mounting UNIX file system.

	Copyright (C) 2000-2001 by John Lindal.

 ******************************************************************************/

#include "jMountUtil.h"
#include "jDirUtil.h"
#include "JThisProcess.h"
#include "JRegex.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "JStdError.h"
#include "jSysUtil.h"

#if defined __OpenBSD__
// Unfortunately, <sys/mount.h> does not compile cleanly with C++ due to
// the use of the "export" keyword as a struct member name.  Use a #define
// to work around the problem
#define export Export
#endif

#if defined _J_MACOS
	#include "JProcess.h"
	#include "jStreamUtil.h"
	#include "JRegex.h"
	#include <sys/param.h>
	#include <sys/ucred.h>
	#include <sys/mount.h>
	#define JMOUNT_MACOS
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

#include "jAssert.h"

#if defined JMOUNT_MACOS
static const JString kMountCmd("mount");
#elif defined JMOUNT_BSD
static const JString kAvailInfoName(_PATH_FSTAB);
#elif defined JMOUNT_SYSV
static const JString kAvailInfoName(VFSTAB);
static const JUtf8Byte* kMountedInfoName = MNTTAB;
#elif defined _J_CYGWIN
static const JUtf8Byte* kMountedInfoName = MOUNTED;
#else
static const JString kAvailInfoName(_PATH_FSTAB);
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

	Sample macOS output:

		/dev/disk1 on / (hfs, local, journaled)
		/dev/disk3s1 on /Volumes/XFER (msdos, local, nodev, nosuid, noowners)
		ntfs://disk4s1/NTFS on /Volumes/NTFS (lifs, local, nodev, nosuid, read-only, noowners, noatime)

	Unless the current uid is zero (root), opts must include "user" or
	"users".  The other possibility is that the *device* must be owned by
	the user, and opts must include "owner".

	If state != nullptr, its value is compared with the mod time of /etc/fstab
	or equivalent.  If the state is the same, nothing is done.

	Returns true if /etc/fstab was read in, i.e., if state was nullptr or
	outdated.

 ******************************************************************************/

#if defined JMOUNT_MACOS

static const JRegex theLinePattern("^((?:msdos|exfat|ntfs):/)?(/[^\\s]+)\\s+on\\s+(/[^)]+?)\\s+\\((.+)\\)");

bool
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
		return false;
	}

	JString mountData;
	JReadAll(outFD, &mountData);

	p->WaitUntilFinished();
	const bool success = p->SuccessfulFinish();
	jdelete p;
	p = nullptr;

	if (!success)
	{
		if (state != nullptr)
		{
			jdelete state->mountCmdOutput;
			state->mountCmdOutput = nullptr;
		}
		return false;
	}

	if (state != nullptr && state->mountCmdOutput != nullptr &&
		mountData == *state->mountCmdOutput)
	{
		return false;
	}

	list->CleanOut();
	if (state != nullptr && state->mountCmdOutput == nullptr)
	{
		state->mountCmdOutput = jnew JString(mountData);
		assert( state->mountCmdOutput != nullptr );
	}
	else if (state != nullptr)
	{
		*state->mountCmdOutput = mountData;
	}

	JString options;
	ACE_stat stbuf;

	JStringIterator iter(mountData);
	while (iter.Next(theLinePattern))
	{
		const JStringMatch& m = iter.GetLastMatch();

		options = m.GetSubstring(4);
		if (options.Contains("nobrowse"))
		{
			continue;
		}

		auto* path = jnew JString(m.GetSubstring(3));
		assert( path != nullptr );
		auto* devicePath = jnew JString(m.GetSubstring(2));
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
		if (options.Contains("msdos") ||
			options.Contains("exfat") ||
			!m.GetSubstring(1).IsEmpty())
		{
			fsType = kVFATType;
		}

		list->AppendItem(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
	}

	return true;
}

#elif defined JMOUNT_BSD

bool
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	time_t t;
	if (state != nullptr &&
		(!JGetModificationTime(_PATH_FSTAB, &t) || t == state->modTime))
	{
		return false;
	}

	list->CleanOut();
	if (state != nullptr)
	{
		state->modTime = t;
	}

	endfsent();

	const bool isRoot = getuid() == 0;

	if (isRoot)
	{
		ACE_stat stbuf;
		while (const fstab* info = getfsent())
		{
			if (JIsRootDirectory(JString(info->fs_file, false)) ||
				strcmp(info->fs_type, FSTAB_SW) == 0)	// swap partition
			{
				continue;
			}

			const JMountType type = JGetUserMountPointType(
				JString(info->fs_file, false),
				JString(info->fs_spec, false),
				JString(info->fs_vfstype, false));

			if (type == kJUnknownMountType ||
				ACE_OS::stat(info->fs_file, &stbuf) != 0)
			{
				continue;
			}

			JFileSystemType fsType = kOtherFSType;
			if (JString::Compare(info->fs_vfstype, "vfat", JString::kIgnoreCase) == 0)
			{
				fsType = kVFATType;
			}

			JString* path = jnew JString(info->fs_file);
			JString* devicePath = jnew JString(info->fs_spec);
			list->AppendItem(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
		}
	}

	endfsent();
	return true;
}

#elif defined JMOUNT_SYSV

bool
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	time_t t;
	if (state != nullptr &&
		(!JGetModificationTime(kAvailInfoName, &t) || t == state->modTime))
	{
		return false;
	}

	list->CleanOut();
	if (state != nullptr)
	{
		state->modTime = t;
	}

	FILE* f = fopen(kAvailInfoName.GetBytes(), "r");
	if (f == nullptr)
	{
		return true;	// did clear list
	}

	const bool isRoot = getuid() == 0;

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

			const JMountType type = JGetUserMountPointType(
				JString(info.vfs_mountp, false),
				JString(info.vfs_special, false),
				JString(info.vfs_fstype, false));

			if (type == kJUnknownMountType ||
				ACE_OS::stat(info.vfs_mountp, &stbuf) != 0)
			{
				continue;
			}

			JFileSystemType fsType = kOtherFSType;
			if (JString::Compare(info.fs_vfstype, "vfat", JString::kIgnoreCase) == 0)
			{
				fsType = kVFATType;
			}

			JString* path = jnew JString(info.vfs_mountp);
			JString* devicePath = jnew JString(info.vfs_special);
			list->AppendItem(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
		}
	}

	fclose(f);
	return true;
}

#elif defined _J_CYGWIN

bool
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	list->CleanOut();
	return false;
}

#else

inline int
jUserOwnsDevice
	(
	const JString& name
	)
{
	uid_t uid;
	return (JGetOwnerID(name, &uid) && uid == getuid());
}

bool
JGetUserMountPointList
	(
	JMountPointList*	list,
	JMountState*		state
	)
{
	time_t t;
	if (state != nullptr &&
		(!JGetModificationTime(kAvailInfoName, &t) || t == state->modTime))
	{
		return false;
	}

	list->CleanOut();
	if (state != nullptr)
	{
		state->modTime = t;
	}

	FILE* f = setmntent(kAvailInfoName.GetBytes(), "r");
	if (f == nullptr)
	{
		return true;	// did clear list
	}

	const bool isRoot = getuid() == 0;

	ACE_stat stbuf;
	while (const mntent* info = getmntent(f))
	{
		if (!(isRoot ||
			  hasmntopt(info, "user") != nullptr  ||
			  hasmntopt(info, "users") != nullptr ||
			  hasmntopt(info, "pamconsole") != nullptr ||
			  (jUserOwnsDevice(JString(info->mnt_fsname, false)) &&
			   hasmntopt(info, "owner") != nullptr)) ||
			JIsRootDirectory(JString(info->mnt_dir, false)) ||
			strcmp(info->mnt_type, MNTTYPE_SWAP) == 0)
		{
			continue;
		}

		const JMountType type =
			JGetUserMountPointType(JString(info->mnt_dir, false),
								   JString(info->mnt_fsname, false),
								   JString(info->mnt_type, false));
		if (type == kJUnknownMountType ||
			ACE_OS::stat(info->mnt_dir, &stbuf) != 0)
		{
			continue;
		}

		JFileSystemType fsType = kOtherFSType;
		if (JString::Compare(info->mnt_type, "vfat", JString::kIgnoreCase) == 0)
		{
			fsType = kVFATType;
		}

		auto* path = jnew JString(info->mnt_dir);
		auto* devicePath = jnew JString(info->mnt_fsname);
		list->AppendItem(JMountPoint(path, type, stbuf.st_dev, devicePath, fsType));
	}

	endmntent(f);
	return true;
}

#endif

/******************************************************************************
 JGetUserMountPointType

	Returns the type of the specified mount point.

 ******************************************************************************/

#if defined JMOUNT_MACOS

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
	if (fsType.Contains("cd9660"))
	{
		return kJCDROM;
	}
	else if (device.StartsWith("/dev/ad") ||	// IDE
			 device.StartsWith("/dev/da"))		// SCSI
	{
		return kJHardDisk;
	}
	else if (device.StartsWith("/dev/fd"))
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
	if (fsType.Contains("iso9660"))
	{
		return kJCDROM;
	}
	else if (fsType.Contains("ufs"))
	{
		return kJHardDisk;
	}
	else if (device.StartsWith("/dev/fd"))
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

JMountType
JGetUserMountPointType
	(
	const JString& path,
	const JString& device,
	const JString& fsType
	)
{
	if (fsType.Contains("iso9660"))
	{
		return kJCDROM;
	}
	else if (path.Contains("floppy"))
	{
		return kJFloppyDisk;	// hot-swappable drives are often /dev/sd*
	}
	else if (device.StartsWith("/dev/hd") ||		// IDE
			 device.StartsWith("/dev/sd"))			// SCSI
	{
		return kJHardDisk;
	}
	else if (device.StartsWith("/dev/fd"))
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

#if defined JMOUNT_BSD || defined JMOUNT_MACOS

bool
JIsMounted
	(
	const JString&		path,
	bool*				writable,
	bool*				isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path.GetBytes(), &stbuf1) != 0)
	{
		return false;
	}

	bool isMounted = false;

	struct statfs* info;
	const JSize count = getmntinfo(&info, MNT_WAIT);

	JString p;
	for (JUnsignedOffset i=0; i<count; i++)
	{
		if (strcmp(info[i].f_mntonname, "/") != 0 &&
			stat(info[i].f_mntonname, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
		{
			isMounted = true;
			if (writable != nullptr)
			{
				*writable = (info[i].f_flags & MNT_RDONLY) == 0;
			}
			if (isTop != nullptr)
			{
				*isTop = stbuf1.st_ino == stbuf2.st_ino;
			}
			if (device != nullptr)
			{
				*device = info[i].f_mntfromname;
			}
			if (fsType != nullptr)
			{
				*fsType = kOtherFSType;
				if (JString::Compare(info[i].f_fstypename, "vfat", JString::kIgnoreCase) == 0  ||	// UNIX
					JString::Compare(info[i].f_fstypename, "msdos", JString::kIgnoreCase) == 0 ||	// macOS
					JString::Compare(info[i].f_fstypename, "exfat", JString::kIgnoreCase) == 0 ||	// macOS
					JString::CompareMaxNBytes(info[i].f_mntfromname, "msdos:", 6) == 0 ||			// macOS
					JString::CompareMaxNBytes(info[i].f_mntfromname, "exfat:", 6) == 0)				// macOS
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

bool
JIsMounted
	(
	const JString&		path,
	bool*				writable,
	bool*				isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
	{
		return false;
	}

	FILE* f = fopen(kMountedInfoName, "r");
	if (f == nullptr)
	{
		return false;
	}

	bool isMounted = false;
	JString p;
	mnttab info;
	while (getmntent(f, &info) == 0)
	{
		if (strcmp(info.mnt_mountp, "/") != 0 &&
			stat(info.mnt_mountp, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
		{
			isMounted = true;
			if (writable != nullptr)
			{
				*writable = hasmntopt(&info, "ro") == nullptr;
			}
			if (isTop != nullptr)
			{
				*isTop = stbuf1.st_ino == stbuf2.st_ino;
			}
			if (device != nullptr)
			{
				*device = info.mnt_special;
			}
			if (fsType != nullptr)
			{
				*fsType = kOtherFSType;
				if (JString::Compare(info.mnt_fstype, "vfat", JString::kIgnoreCase) == 0)
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

bool
JIsMounted
	(
	const JString&		path,
	bool*				writable,
	bool*				isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
	{
		return false;
	}

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == nullptr)
	{
		return false;
	}

	bool isMounted = false;
	JString p;
	while (const mntent* info = getmntent(f))
	{
		if (strcmp(info->mnt_dir, "/") != 0 &&
			stat(info->mnt_dir, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
		{
			isMounted = true;
			if (writable != nullptr)
			{
				*writable = true;
			}
			if (isTop != nullptr)
			{
				*isTop = stbuf1.st_ino == stbuf2.st_ino;
			}
			if (device != nullptr)
			{
				*device = info->mnt_fsname;
			}
			if (fsType != nullptr)
			{
				*fsType = kOtherFSType;
				if (JString::Compare(info->mnt_type, "vfat", JString::kIgnoreCase) == 0)
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

bool
JIsMounted
	(
	const JString&		path,
	bool*				writable,
	bool*				isTop,
	JString*			device,
	JFileSystemType*	fsType,
	JString*			fsTypeString
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path.GetBytes(), &stbuf1) != 0)
	{
		return false;
	}

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == nullptr)
	{
		return false;
	}

	bool isMounted = false;
	JString p;
	while (const mntent* info = getmntent(f))
	{
		if (strcmp(info->mnt_dir, "/") != 0 &&
			stat(info->mnt_dir, &stbuf2) == 0 &&
			stbuf1.st_dev == stbuf2.st_dev)
		{
			isMounted = true;
			if (writable != nullptr)
			{
				*writable = hasmntopt(info, MNTOPT_RO) == nullptr;
			}
			if (isTop != nullptr)
			{
				*isTop = stbuf1.st_ino == stbuf2.st_ino;
			}
			if (device != nullptr)
			{
				*device = info->mnt_fsname;
			}
			if (fsType != nullptr)
			{
				*fsType = kOtherFSType;
				if (JString::Compare(info->mnt_type, "vfat", JString::kIgnoreCase) == 0)
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

	Returns true if the given path is on a mounted partition.

 ******************************************************************************/

bool
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
		return false;
	}

	const JSize count = list.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JMountPoint pt = list.GetItem(i);
		if (pt.device == stbuf.st_dev)
		{
			*index = i;
			return true;
		}
	}

	return false;
}

/******************************************************************************
 JMount

	To determine whether or not this function succeeded, call it with
	block=true and then check JIsMounted().

	If mount = false, the file system is unmounted instead.

 ******************************************************************************/

void
JMount
	(
	const JString&	path,
	const bool	mount,
	const bool	block
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
			iter.MoveTo(JStringIterator::kStartAtEnd, 0);
			*host += iter.FinishMatch().GetString();
		}
	}
}

/******************************************************************************
 JTranslateLocalToRemote

	Translate local path to remote host and path, if it is mounted.

 ******************************************************************************/

inline bool
jCalledByTranslateLocalToRemote
	(
	const JString&		localPath,
	const JUtf8Byte*	mountDev,
	const JUtf8Byte*	mountDir,
	bool*				found,
	JString*			host,
	JString*			remotePath
	)
{
	const JString dir(mountDir, JString::kNoCopy);
	if (!JIsSamePartition(localPath, dir))
	{
		return false;
	}

	const JString dev(mountDev, JString::kNoCopy);
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
			*found = true;
			return true;
		}
		#endif

		jGetFullHostName(host);

		iter.SkipPrev();	// include leading slash
		iter.BeginMatch();
		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		*remotePath = iter.FinishMatch().GetString();
		JAppendDirSeparator(remotePath);

		JStringIterator iter2(localPath, JStringIterator::kStartAfterChar, dir.GetCharacterCount());
		iter2.BeginMatch();
		iter2.MoveTo(JStringIterator::kStartAtEnd, 0);
		*remotePath += iter2.FinishMatch().GetString();
		JCleanPath(remotePath);

		*found = true;
	}

	return true;
}

#if defined JMOUNT_BSD || defined JMOUNT_MACOS

bool
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
		return false;
	}

	struct statfs* info;
	const JSize count = getmntinfo(&info, MNT_WAIT);

	bool found = false;
	for (JUnsignedOffset i=0; i<count; i++)
	{
		if (jCalledByTranslateLocalToRemote(localPath, info[i].f_mntfromname,
											info[i].f_mntonname,
											&found, host, remotePath))
		{
			break;		// localPath only on one partition
		}
	}

	return found;
}

#elif defined JMOUNT_SYSV

bool
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
		return false;
	}

	bool found = false;
	mnttab info;
	while (getmntent(f, &info) == 0)
	{
		if (jCalledByTranslateLocalToRemote(localPath, info.mnt_special,
											info.mnt_mountp, &found, host, remotePath))
		{
			break;		// localPath only on one partition
		}
	}

	fclose(f);
	return found;
}

#else

bool
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
		return false;
	}

	bool found = false;
	while (const mntent* info = getmntent(f))
	{
		if (jCalledByTranslateLocalToRemote(localPath, info->mnt_fsname,
											info->mnt_dir, &found, host, remotePath))
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

inline bool
jCalledByTranslateRemoteToLocal
	(
	const JString&		host,
	const JString&		remotePath,
	const JUtf8Byte*	mountDev,
	const JUtf8Byte*	mountDir,
	JString*			localPath
	)
{
	const JString dev(mountDev, JString::kNoCopy);
	JStringIterator iter(dev);
	iter.BeginMatch();
	if (iter.Next(":/") && iter.GetPrevCharacterIndex() > 2)
	{
		JString h = iter.FinishMatch().GetString();
		jGetFullHostName(&h);

		iter.SkipPrev();	// include leading slash
		iter.BeginMatch();
		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		JString p = iter.FinishMatch().GetString();
		JAppendDirSeparator(&p);		// force complete name when check StartsWith()

		if (host == h && remotePath.StartsWith(p))
		{
			*localPath = remotePath;

			JStringIterator iter2(localPath);
			iter2.BeginMatch();
			iter2.MoveTo(JStringIterator::kStartBeforeChar, p.GetCharacterCount());
			iter2.FinishMatch();
			iter2.ReplaceLastMatch(JString(mountDir, JString::kNoCopy));

			JCleanPath(localPath);
			return true;
		}
	}

	return false;
}

#if defined JMOUNT_BSD || defined JMOUNT_MACOS

bool
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

	bool found = false;
	for (JUnsignedOffset i=0; i<count; i++)
	{
		if (jCalledByTranslateRemoteToLocal(hostStr, remotePath, info[i].f_mntfromname,
											info[i].f_mntonname, localPath))
		{
			found = true;
			break;
		}
	}

	return found;
}

#elif defined JMOUNT_SYSV

bool
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
		return false;
	}

	const JString remotePath = remotePathStr;

	bool found = false;
	mnttab info;
	while (getmntent(f, &info) == 0)
	{
		if (jCalledByTranslateRemoteToLocal(host, remotePath, info.mnt_special,
											info.mnt_mountp, localPath))
		{
			found = true;
			break;
		}
	}

	fclose(f);
	return found;
}

#else

bool
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
		return false;
	}

	const JString remotePath = remotePathStr;

	bool found = false;
	while (const mntent* info = getmntent(f))
	{
		if (jCalledByTranslateRemoteToLocal(host, remotePath, info->mnt_fsname,
											info->mnt_dir, localPath))
		{
			found = true;
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
	bool writable, isTop;
	JString device;
	if (JIsMounted(path, &writable, &isTop, &device) && isTop)
	{
		JMount(path, false, true);
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

	Returns true if the two directories are on the same partition.

 ******************************************************************************/

bool
JIsSamePartition
	(
	const JString& path1,
	const JString& path2
	)
{
	ACE_stat stbuf1, stbuf2;
	return (ACE_OS::stat(path1.GetBytes(), &stbuf1) == 0 &&
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
	for (const auto& info : *this)
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
#if defined JMOUNT_MACOS
	jdelete mountCmdOutput;
#endif
}
