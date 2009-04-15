/******************************************************************************
 jMountUtil_UNIX.cpp

	Utility functions for mounting UNIX file system.

	Copyright © 2000-2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jMountUtil.h>
#include <jDirUtil.h>
#include <JThisProcess.h>
#include <JRegex.h>
#include <jSysUtil.h>

#if defined __OpenBSD__
// Unfortunately, <sys/mount.h> does not compile cleanly with C++ due to
// the use of the "export" keyword as a struct member name.  Use a #define
// to work around the problem
#define export Export
#endif

#if defined __FreeBSD__ || defined __OpenBSD__ || defined _J_OSX
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

#if defined JMOUNT_BSD
static const JCharacter* kAvailInfoName   = _PATH_FSTAB;
#elif defined JMOUNT_SYSV
static const JCharacter* kAvailInfoName   = VFSTAB;
static const JCharacter* kMountedInfoName = MNTTAB;
#elif defined _J_CYGWIN
static const JCharacter* kMountedInfoName = MOUNTED;
#else
static const JCharacter* kAvailInfoName   = _PATH_FSTAB;
static const JCharacter* kMountedInfoName = _PATH_MOUNTED;
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

	Unless the current uid is zero (root), opts must include "user" or
	"users".  The other possibility is that the *device* must be owned by
	the user, and opts must include "owner".

	If modTime != NULL, its value is compared with the mod time of
	/etc/fstab.  If the times are the same, and the list is not empty,
	nothing is done.

	Returns kJTrue if /etc/fstab was read in, i.e., if modTime was NULL or
	outdated.

 ******************************************************************************/

#if defined JMOUNT_BSD

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	time_t*				modTime
	)
{
	time_t t;
	if (modTime != NULL &&
		(!(JGetModificationTime(_PATH_FSTAB, &t)).OK() ||
		 t == *modTime))
		{
		return kJFalse;
		}

	list->CleanOut();
	if (modTime != NULL)
		{
		*modTime = t;
		}

	endfsent();

	const JBoolean isRoot = JI2B( getuid() == 0 );

	ACE_stat stbuf;
	while (const fstab* info = getfsent())
		{
		if (!isRoot ||
			JIsRootDirectory(info->fs_file) ||
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

		JString* path = new JString(info->fs_file);
		assert( path != NULL );
		JString* devicePath = new JString(info->fs_spec);
		assert( devicePath != NULL );
		list->AppendElement(JMountPoint(path, type, stbuf.st_dev, devicePath));
		}

	endfsent();
	return kJTrue;
}

#elif defined JMOUNT_SYSV

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	time_t*				modTime
	)
{
	time_t t;
	if (modTime != NULL &&
		(!(JGetModificationTime(kAvailInfoName, &t)).OK() ||
		 t == *modTime))
		{
		return kJFalse;
		}

	list->CleanOut();
	if (modTime != NULL)
		{
		*modTime = t;
		}

	FILE* f = fopen(kAvailInfoName, "r");
	if (f == NULL)
		{
		return kJTrue;	// did clear list
		}

	const JBoolean isRoot = JI2B( getuid() == 0 );

	ACE_stat stbuf;
	vfstab info;
	while (getvfsent(f, &info) == 0)
		{
		if (!isRoot ||
			JIsRootDirectory(info.vfs_mountp) ||
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

		JString* path = new JString(info.vfs_mountp);
		assert( path != NULL );
		JString* devicePath = new JString(info.vfs_special);
		assert( devicePath != NULL );
		list->AppendElement(JMountPoint(path, type, stbuf.st_dev, devicePath));
		}

	fclose(f);
	return kJTrue;
}

#elif defined _J_CYGWIN

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	time_t*				modTime
	)
{
	list->CleanOut();
	return kJFalse;
}

#else

inline int
jUserOwnsDevice
	(
	const JCharacter* name
	)
{
	uid_t uid;
	return ((JGetOwnerID(name, &uid)).OK() && uid == getuid());
}

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	time_t*				modTime
	)
{
	time_t t;
	if (modTime != NULL &&
		(!(JGetModificationTime(kAvailInfoName, &t)).OK() ||
		 t == *modTime))
		{
		return kJFalse;
		}

	list->CleanOut();
	if (modTime != NULL)
		{
		*modTime = t;
		}

	FILE* f = setmntent(kAvailInfoName, "r");
	if (f == NULL)
		{
		return kJTrue;	// did clear list
		}

	const JBoolean isRoot = JI2B( getuid() == 0 );

	ACE_stat stbuf;
	while (const mntent* info = getmntent(f))
		{
		if (!(isRoot ||
			  hasmntopt(info, "user") != NULL  ||
			  hasmntopt(info, "users") != NULL ||
			  hasmntopt(info, "pamconsole") != NULL ||
			  (jUserOwnsDevice(info->mnt_fsname) &&
			   hasmntopt(info, "owner") != NULL)) ||
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

		JString* path = new JString(info->mnt_dir);
		assert( path != NULL );
		JString* devicePath = new JString(info->mnt_fsname);
		assert( devicePath != NULL );
		list->AppendElement(JMountPoint(path, type, stbuf.st_dev, devicePath));
		}

	endmntent(f);
	return kJTrue;
}

#endif

/******************************************************************************
 JGetUserMountPointType

	Returns the type of the specified mount point.

 ******************************************************************************/

#if defined JMOUNT_BSD

JMountType
JGetUserMountPointType
	(
	const JCharacter* path,
	const JCharacter* device,
	const JCharacter* fsType
	)
{
	if (strstr(fsType, "cd9660") != NULL)
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
	const JCharacter* path,
	const JCharacter* device,
	const JCharacter* fsType
	)
{
	if (strstr(fsType, "iso9660") != NULL)
		{
		return kJCDROM;
		}
	else if (strstr(fsType, "ufs") != NULL)
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
	const JCharacter* path,
	const JCharacter* device,
	const JCharacter* fsType
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
	JCharacter*	buffer
	)
{
	JIndex i = 0;
	JCharacter c;
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
	const JCharacter* path,
	const JCharacter* device,
	const JCharacter* fsType
	)
{
	if (strstr(fsType, "iso9660") != NULL)
		{
		return kJCDROM;
		}
	else if (JCompareMaxN("/mnt/floppy", path, 11, kJFalse))
		{
		return kJFloppyDisk;	// hot-swappable drives are often /dev/sd*
		}
	else if (strncmp("/dev/hd", device, 7) == 0)		// IDE
		{
		JMountType type = kJHardDisk;

		// check for ZIP disk

		JString modelFile = device;
		JIndexRange r;
		if (devIndexPattern.Match(modelFile, &r))
			{
			modelFile.RemoveSubstring(r);
			}
		modelFile.ReplaceSubstring(2,4, "proc/ide");
		modelFile = JCombinePathAndName(modelFile, "model");

		const int fd = open(modelFile, O_RDONLY);
		if (fd != -1)
			{
			JCharacter buf[501];
			const ssize_t count = read(fd, buf, 500);
			if (count > 0)
				{
				const JString model(buf, count);
				if (model.Contains("IOMEGA ZIP 100", kJFalse))
					{
					type = kJZipDisk;
					}
				}
			close(fd);
			}

		return type;
		}
	else if (strncmp("/dev/sd", device, 7) == 0)		// SCSI
		{
		JMountType type = kJHardDisk;

		// check for ZIP disk

		const int fd = open("/proc/scsi/scsi", O_RDONLY);
		if (fd != -1)
			{
			JCharacter buf[101];
			buf[100] = '\0';

			// compute index into /proc/scsi/scsi

			const JIndex index = device[7] - 'a' + 1;

			// Attached devices: ?

			jReadLine(fd, buf);

			// skip preceding records

			for (JIndex i=1; i<index; i++)
				{
				jReadLine(fd, buf);
				}

			// Host: scsi_ Channel: __ Id: __ Lun: __

			jReadLine(fd, buf);

			//   Vendor: ? Model: ZIP 100 Rev: ?

			jReadLine(fd, buf);
			const JString model(buf);
			if (model.Contains("ZIP 100", kJFalse))
				{
				type = kJZipDisk;
				}

			close(fd);
			}

		return type;
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

	device can be NULL

 ******************************************************************************/

#if defined JMOUNT_BSD

JBoolean
JIsMounted
	(
	const JCharacter*	path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JString*			fsType
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
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
			if (writable != NULL)
				{
				*writable = JNegate(info[i].f_flags & MNT_RDONLY != 0);
				}
			if (isTop != NULL)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != NULL)
				{
				*device = info[i].f_mntfromname;
				}
			if (fsType != NULL)
				{
				*fsType = info[i].f_fstypename;
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
	const JCharacter*	path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JString*			fsType
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
		{
		return kJFalse;
		}

	FILE* f = fopen(kMountedInfoName, "r");
	if (f == NULL)
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
			if (writable != NULL)
				{
				*writable = JI2B(hasmntopt(&info, "ro") == NULL);
				}
			if (isTop != NULL)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != NULL)
				{
				*device = info.mnt_special;
				}
			if (fsType != NULL)
				{
				*fsType = info.mnt_fstype;
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
	const JCharacter*	path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JString*			fsType
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
		{
		return kJFalse;
		}

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == NULL)
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
			if (writable != NULL)
				{
				*writable = kJTrue;
				}
			if (isTop != NULL)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != NULL)
				{
				*device = info->mnt_fsname;
				}
			if (fsType != NULL)
				{
				*fsType = info->mnt_type;
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
	const JCharacter*	path,
	JBoolean*			writable,
	JBoolean*			isTop,
	JString*			device,
	JString*			fsType
	)
{
	struct stat stbuf1, stbuf2;
	if (stat(path, &stbuf1) != 0)
		{
		return kJFalse;
		}

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == NULL)
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
			if (writable != NULL)
				{
				*writable = JI2B(hasmntopt(info, MNTOPT_RO) == NULL);
				}
			if (isTop != NULL)
				{
				*isTop = JI2B(stbuf1.st_ino == stbuf2.st_ino);
				}
			if (device != NULL)
				{
				*device = info->mnt_fsname;
				}
			if (fsType != NULL)
				{
				*fsType = info->mnt_type;
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
	const JCharacter*		path,
	const JMountPointList&	list,
	JIndex*					index
	)
{
	*index = 0;

	struct stat stbuf;
	if (stat(path, &stbuf) != 0)
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
	const JCharacter*	path,
	const JBoolean		mount,
	const JBoolean		block
	)
{
	const JCharacter* argv[] = { mount ? "mount" : "umount", path, NULL };
	pid_t pid;
	JExecute(argv, sizeof(argv), block ? (pid_t*) NULL : &pid,
			 kJIgnoreConnection, NULL,
			 kJTossOutput, NULL,
			 kJTossOutput, NULL);
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
		JIndex dotIndex;
		if (localhost.LocateSubstring(".", &dotIndex))
			{
			*host += localhost.GetSubstring(dotIndex, localhost.GetLength());
			}
		}
}

/******************************************************************************
 JTranslateLocalToRemote

	Translate remote host and path to local path, if it is mounted.

 ******************************************************************************/

inline JBoolean
jTranslateLocalToRemote1
	(
	const JString&		localPath,
	const JCharacter*	mountDev,
	const JCharacter*	mountDir,
	JBoolean*			found,
	JString*			host,
	JString*			remotePath
	)
{
	if (JIsSamePartition(localPath, mountDir))
		{
		const JString dev = mountDev;
		JIndex hostEndIndex;
		if (dev.LocateSubstring(":/", &hostEndIndex) && hostEndIndex > 1)
			{
			*host = dev.GetSubstring(1, hostEndIndex-1);
			jGetFullHostName(host);

			*remotePath = dev.GetSubstring(hostEndIndex+1, dev.GetLength());
			JAppendDirSeparator(remotePath);

			// use JIndexRange to allow empty

			JIndexRange r(strlen(mountDir)+1, localPath.GetLength());
			*remotePath += localPath.GetSubstring(r);
			JCleanPath(remotePath);

			*found = kJTrue;
			}
		return kJTrue;
		}

	return kJFalse;
}

#if defined JMOUNT_BSD

JBoolean
JTranslateLocalToRemote
	(
	const JCharacter*	localPathStr,
	JString*			host,
	JString*			remotePath
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
	const JCharacter*	localPathStr,
	JString*			host,
	JString*			remotePath
	)
{
	host->Clear();
	remotePath->Clear();

	JString localPath;
	FILE* f = fopen(kMountedInfoName, "r");
	if (f == NULL || !JGetTrueName(localPathStr, &localPath))
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
	const JCharacter*	localPathStr,
	JString*			host,
	JString*			remotePath
	)
{
	host->Clear();
	remotePath->Clear();

	JString localPath;
	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == NULL || !JGetTrueName(localPathStr, &localPath))
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
	const JCharacter*	host,
	const JString&		remotePath,
	const JCharacter*	mountDev,
	const JCharacter*	mountDir,
	JString*			localPath
	)
{
	const JString dev = mountDev;
	JIndex hostEndIndex;
	if (dev.LocateSubstring(":/", &hostEndIndex) && hostEndIndex > 1)
		{
		JString h = dev.GetSubstring(1, hostEndIndex-1);
		jGetFullHostName(&h);

		JString p = dev.GetSubstring(hostEndIndex+1, dev.GetLength());
		JAppendDirSeparator(&p);		// force complete name when check BeginsWith()

		if (host == h && remotePath.BeginsWith(p))
			{
			*localPath = remotePath;
			localPath->ReplaceSubstring(1, p.GetLength()-1, mountDir);
			JCleanPath(localPath);
			return kJTrue;
			}
		}

	return kJFalse;
}

#if defined JMOUNT_BSD

JBoolean
JTranslateRemoteToLocal
	(
	const JCharacter*	hostStr,
	const JCharacter*	remotePathStr,
	JString*			localPath
	)
{
	localPath->Clear();

	struct statfs* info;
	const JSize count = getmntinfo(&info, MNT_WAIT);

	const JString remotePath = remotePathStr;

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
	const JCharacter*	host,
	const JCharacter*	remotePathStr,
	JString*			localPath
	)
{
	localPath->Clear();

	FILE* f = fopen(kMountedInfoName, "r");
	if (f == NULL)
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
	const JCharacter*	host,
	const JCharacter*	remotePathStr,
	JString*			localPath
	)
{
	localPath->Clear();

	FILE* f = setmntent(kMountedInfoName, "r");
	if (f == NULL)
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
	const JCharacter*	path,
	const JCharacter*	type,
	JProcess**			process
	)
{
	JBoolean writable, isTop;
	JString device;
	if (JIsMounted(path, &writable, &isTop, &device) && isTop)
		{
		JMount(path, kJFalse, kJTrue);
		if (!JIsMounted(path))
			{
			const JCharacter* argv[] = { "xterm", "-T", "Format disk", "-n", "Format disk",
										 "-e", "/sbin/mkfs", "-t", type, "-c", device, NULL };
			const JError err = JProcess::Create(process, argv, sizeof(argv));
			if (err.OK())
				{
				JThisProcess::Ignore(*process);
				}
			return err;
			}
		}

	*process = NULL;
	return JAccessDenied(path);
}

/******************************************************************************
 JIsSamePartition

	Returns kJTrue if the two directories are on the same partition.

 ******************************************************************************/

JBoolean
JIsSamePartition
	(
	const JCharacter* path1,
	const JCharacter* path2
	)
{
	ACE_stat stbuf1, stbuf2;
	return JI2B(ACE_OS::stat(path1, &stbuf1) == 0 &&
				ACE_OS::stat(path2, &stbuf2) == 0 &&
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
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JMountPoint info = GetElement(i);
		delete info.path;
		delete info.devicePath;
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

#define JTemplateType JMountPoint
#include <JArray.tmpls>
#undef JTemplateType
