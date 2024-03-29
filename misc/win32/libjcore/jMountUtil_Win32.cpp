/******************************************************************************
 jMountUtil_Win32.cpp

	Utility functions for mounting Windows file system.

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include <jMountUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 JGetUserMountPointList

 ******************************************************************************/

bool
JGetUserMountPointList
	(
	JMountPointList*	list,
	time_t*				modTime
	)
{
	list->CleanOut();
	if (modTime != nullptr)
		{
		*modTime = 0;
		}

	JCharacter* drive = "A:\\";
	for (JCharacter c='A'; c<='Z'; c++)
		{
		drive[0] = c;
		const UINT driveType = GetDriveType(drive);

		JMountType type;
		if (driveType == DRIVE_REMOVABLE)
			{
			type = kJFloppyDisk;
			}
		else if (driveType == DRIVE_FIXED  ||
				 driveType == DRIVE_REMOTE ||
				 driveType == DRIVE_RAMDISK)
			{
			type = kJHardDisk;
			}
		else if (driveType == DRIVE_CDROM)
			{
			type = kJCDROM;
			}
		else
			{
			continue;
			}

		JString* path = new JString(drive);
		assert( path != nullptr );
		JString* devicePath = new JString(drive);
		assert( devicePath != nullptr );
		list->AppendElement(JMountPoint(path, type, c - 'A', devicePath));
		}

	return true;
}

/******************************************************************************
 JGetUserMountPointType

	Returns the type of the specified mount point.

 ******************************************************************************/

JMountType
JGetUserMountPointType
	(
	const JCharacter* path,
	const JCharacter* device,
	const JCharacter* fsType
	)
{
	if (path[0] == '\0' || path[1] == '\0' || path[1] != ':')
		{
		return kJUnknownMountType;
		}

	JCharacter* drive    = "A:\\";
	drive[0]             = toupper(path[0]);
	const UINT driveType = GetDriveType(drive);
	if (driveType == DRIVE_REMOVABLE)
		{
		return kJFloppyDisk;
		}
	else if (driveType == DRIVE_FIXED  ||
			 driveType == DRIVE_REMOTE ||
			 driveType == DRIVE_RAMDISK)
		{
		return kJHardDisk;
		}
	else if (driveType == DRIVE_CDROM)
		{
		return kJCDROM;
		}
	else
		{
		return kJUnknownMountType;
		}
}

/******************************************************************************
 JIsMounted

	device can be nullptr

 ******************************************************************************/

bool
JIsMounted
	(
	const JCharacter*	path,
	bool*			writable,
	JString*			device
	)
{
	if (JString::IsEmpty(path) || !JNameUsed(path) || path[1] != ':')
		{
		return false;
		}

	JCharacter* drive = "A:\\";
	drive[0]          = path[0];
	const UINT type   = GetDriveType(drive);
	return type != DRIVE_UNKNOWN && type != DRIVE_NO_ROOT_DIR;
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
	const JCharacter*	path,
	const bool		mount,
	const bool		block
	)
{
}

/******************************************************************************
 JTranslateLocalToRemote

	Translate remote host and path to local path, if it is mounted.

 ******************************************************************************/

bool
JTranslateLocalToRemote
	(
	const JCharacter*	localPathStr,
	JString*			host,
	JString*			remotePath
	)
{
	return false;
}

/******************************************************************************
 JTranslateRemoteToLocal

	Translate remote host and path to local path, if it is mounted.

	Note that this will not work if the remote path is a symbolic link.
	We can't convert that to a true path.

 ******************************************************************************/

bool
JTranslateRemoteToLocal
	(
	const JCharacter*	hostStr,
	const JCharacter*	remotePathStr,
	JString*			localPath
	)
{
	return false;
}

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
	const JCharacter* path1,
	const JCharacter* path2
	)
{
	JString s1, s2;
	if (!JConvertToAbsolutePath(path1, nullptr, &s1) ||
		!JConvertToAbsolutePath(path2, nullptr, &s2))
		{
		return false;
		}

	return s1.GetCharacter(1) == s2.GetCharacter(1) &&
				s1.GetCharacter(2) == ':' &&
				s2.GetCharacter(2) == ':';
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
