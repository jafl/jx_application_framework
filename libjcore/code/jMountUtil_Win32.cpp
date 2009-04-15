/******************************************************************************
 jMountUtil_Win32.cpp

	Utility functions for mounting Windows file system.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jMountUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 JGetUserMountPointList

 ******************************************************************************/

JBoolean
JGetUserMountPointList
	(
	JMountPointList*	list,
	time_t*				modTime
	)
{
	list->CleanOut();
	if (modTime != NULL)
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
		assert( path != NULL );
		JString* devicePath = new JString(drive);
		assert( devicePath != NULL );
		list->AppendElement(JMountPoint(path, type, c - 'A', devicePath));
		}

	return kJTrue;
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

	device can be NULL

 ******************************************************************************/

JBoolean
JIsMounted
	(
	const JCharacter*	path,
	JBoolean*			writable,
	JString*			device
	)
{
	if (JStringEmpty(path) || !JNameUsed(path) || path[1] != ':')
		{
		return kJFalse;
		}

	JCharacter* drive = "A:\\";
	drive[0]          = path[0];
	const UINT type   = GetDriveType(drive);
	return JI2B( type != DRIVE_UNKNOWN && type != DRIVE_NO_ROOT_DIR );
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
}

/******************************************************************************
 JTranslateLocalToRemote

	Translate remote host and path to local path, if it is mounted.

 ******************************************************************************/

JBoolean
JTranslateLocalToRemote
	(
	const JCharacter*	localPathStr,
	JString*			host,
	JString*			remotePath
	)
{
	return kJFalse;
}

/******************************************************************************
 JTranslateRemoteToLocal

	Translate remote host and path to local path, if it is mounted.

	Note that this will not work if the remote path is a symbolic link.
	We can't convert that to a true path.

 ******************************************************************************/

JBoolean
JTranslateRemoteToLocal
	(
	const JCharacter*	hostStr,
	const JCharacter*	remotePathStr,
	JString*			localPath
	)
{
	return kJFalse;
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
	JString s1, s2;
	if (!JConvertToAbsolutePath(path1, NULL, &s1) ||
		!JConvertToAbsolutePath(path2, NULL, &s2))
		{
		return kJFalse;
		}

	return JI2B(s1.GetCharacter(1) == s2.GetCharacter(1) &&
				s1.GetCharacter(2) == ':' &&
				s2.GetCharacter(2) == ':');
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
