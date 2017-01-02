/******************************************************************************
 JStdError.cpp

	Constants for the most common errno values.  Functions returning these
	errors from libc functions should guarantee that the error returned
	reflects only that call.

	Copyright (C) 1997-2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JStdError.h>
#include <jGlobals.h>
#include <jAssert.h>

// Types

const JUtf8Byte* kJGeneralIO = "JGeneralIO";

const JUtf8Byte* kJSegFault           = "JSegFault";
const JUtf8Byte* kJNoProcessMemory    = "JNoProcessMemory";
const JUtf8Byte* kJNoKernelMemory     = "JNoKernelMemory";
const JUtf8Byte* kJMemoryNotReadable  = "JMemoryNotReadable";
const JUtf8Byte* kJMemoryNotWriteable = "JMemoryNotWriteable";

const JUtf8Byte* kJProgramNotAvailable   = "JProgramNotAvailable";
const JUtf8Byte* kJDirEntryAlreadyExists = "JDirEntryAlreadyExists";
const JUtf8Byte* kJDirEntryDoesNotExist  = "JDirEntryDoesNotExist";
const JUtf8Byte* kJNameTooLong           = "JNameTooLong";
const JUtf8Byte* kJFileBusy              = "JFileBusy";
const JUtf8Byte* kJInvalidOpenMode       = "JInvalidOpenMode";

const JUtf8Byte* kJAccessDenied       = "JAccessDenied";
const JUtf8Byte* kJFileSystemReadOnly = "JFileSystemReadOnly";
const JUtf8Byte* kJFileSystemFull     = "JFileSystemFull";
const JUtf8Byte* kJDeviceFull         = "JDeviceFull";

const JUtf8Byte* kJComponentNotDirectory  = "JComponentNotDirectory";
const JUtf8Byte* kJNotSymbolicLink        = "JNotSymbolicLink";
const JUtf8Byte* kJPathContainsLoop       = "JPathContainsLoop";
const JUtf8Byte* kJNoHomeDirectory        = "JNoHomeDirectory";
const JUtf8Byte* kJBadPath                = "JBadPath";
const JUtf8Byte* kJDirectoryNotEmpty      = "JDirectoryNotEmpty";
const JUtf8Byte* kJDirectoryBusy          = "JDirectoryBusy";

const JUtf8Byte* kJCantRenameFileToDirectory     = "JCantRenameFileToDirectory";
const JUtf8Byte* kJCantRenameAcrossFilesystems   = "JCantRenameAcrossFilesystems";
const JUtf8Byte* kJCantRenameToNonemptyDirectory = "JCantRenameToNonemptyDirectory";
const JUtf8Byte* kJDirectoryCantBeOwnChild       = "JDirectoryCantBeOwnChild";
const JUtf8Byte* kJTooManyLinks                  = "JTooManyLinks";

const JUtf8Byte* kJTriedToRemoveDirectory = "JTriedToRemoveDirectory";
const JUtf8Byte* kJIsADirectory           = "JIsADirectory";

const JUtf8Byte* kJDescriptorNotOpen      = "JDescriptorNotOpen";
const JUtf8Byte* kJTooManyDescriptorsOpen = "JTooManyDescriptorsOpen";
const JUtf8Byte* kJNegativeDescriptor     = "JNegativeDescriptor";
const JUtf8Byte* kJInvalidDescriptor      = "JInvalidDescriptor";

const JUtf8Byte* kJWouldHaveBlocked = "JWouldHaveBlocked";
const JUtf8Byte* kJNonBlockedSignal = "JNonBlockedSignal";

const JUtf8Byte* kJNotCompressed = "JNotCompressed";

/******************************************************************************
 Constructors

 *****************************************************************************/

JProgramNotAvailable::JProgramNotAvailable
	(
	const JString& programName
	)
	:
	JError(kJProgramNotAvailable)
{
	const JUtf8Byte* map[] = { "name", programName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JDirEntryAlreadyExists::JDirEntryAlreadyExists
	(
	const JString& fileName
	)
	:
	JError(kJDirEntryAlreadyExists)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JDirEntryDoesNotExist::JDirEntryDoesNotExist
	(
	const JString& fileName
	)
	:
	JError(kJDirEntryDoesNotExist)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JFileBusy::JFileBusy
	(
	const JString& fileName
	)
	:
	JError(kJFileBusy)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JInvalidOpenMode::JInvalidOpenMode
	(
	const JString& fileName
	)
	:
	JError(kJInvalidOpenMode)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}


JAccessDenied::JAccessDenied
	(
	const JString& fileName
	)
	:
	JError(kJAccessDenied)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JAccessDenied::JAccessDenied
	(
	const JString& fileName1,
	const JString& fileName2
	)
	:
	JError(kJAccessDenied)
{
	const JUtf8Byte* map[] =
		{
		"name1", fileName1.GetBytes(),
		"name2", fileName2.GetBytes()
		};
	const JString msg = JGetString("JAccessDenied2", map, sizeof(map));
	SetMessage(msg);
}


JComponentNotDirectory::JComponentNotDirectory
	(
	const JString& path
	)
	:
	JError(kJComponentNotDirectory)
{
	const JUtf8Byte* map[] = { "name", path.GetBytes() };
	SetMessage(map, sizeof(map));
}

JComponentNotDirectory::JComponentNotDirectory
	(
	const JString& path1,
	const JString& path2
	)
	:
	JError(kJComponentNotDirectory)
{
	const JUtf8Byte* map[] =
		{
		"name1", path1.GetBytes(),
		"name2", path2.GetBytes()
		};
	const JString msg = JGetString("JComponentNotDirectory2", map, sizeof(map));
	SetMessage(msg);
}

JNotSymbolicLink::JNotSymbolicLink
	(
	const JString& name
	)
	:
	JError(kJNotSymbolicLink)
{
	const JUtf8Byte* map[] = { "name", name.GetBytes() };
	SetMessage(map, sizeof(map));
}

JPathContainsLoop::JPathContainsLoop
	(
	const JString& path
	)
	:
	JError(kJPathContainsLoop)
{
	const JUtf8Byte* map[] = { "name", path.GetBytes() };
	SetMessage(map, sizeof(map));
}

JPathContainsLoop::JPathContainsLoop
	(
	const JString& path1,
	const JString& path2
	)
	:
	JError(kJPathContainsLoop)
{
	const JUtf8Byte* map[] =
		{
		"name1", path1.GetBytes(),
		"name2", path2.GetBytes()
		};
	const JString msg = JGetString("JPathContainsLoop2", map, sizeof(map));
	SetMessage(msg);
}

JBadPath::JBadPath
	(
	const JString& path
	)
	:
	JError(kJBadPath)
{
	const JUtf8Byte* map[] = { "name", path.GetBytes() };
	SetMessage(map, sizeof(map));
}

JBadPath::JBadPath
	(
	const JString& path1,
	const JString& path2
	)
	:
	JError(kJBadPath)
{
	const JUtf8Byte* map[] =
		{
		"name1", path1.GetBytes(),
		"name2", path2.GetBytes()
		};
	const JString msg = JGetString("JBadPath2", map, sizeof(map));
	SetMessage(msg);
}

JDirectoryNotEmpty::JDirectoryNotEmpty
	(
	const JString& name
	)
	:
	JError(kJDirectoryNotEmpty)
{
	const JUtf8Byte* map[] = { "name", name.GetBytes() };
	SetMessage(map, sizeof(map));
}

JDirectoryBusy::JDirectoryBusy
	(
	const JString& name
	)
	:
	JError(kJDirectoryBusy)
{
	const JUtf8Byte* map[] = { "name", name.GetBytes() };
	SetMessage(map, sizeof(map));
}


JCantRenameFileToDirectory::JCantRenameFileToDirectory
	(
	const JString& oldName,
	const JString& newName
	)
	:
	JError(kJCantRenameFileToDirectory)
{
	const JUtf8Byte* map[] =
		{
		"old_name", oldName.GetBytes(),
		"new_name", newName.GetBytes()
		};
	SetMessage(map, sizeof(map));
}

JTooManyLinks::JTooManyLinks
	(
	const JString& name
	)
	:
	JError(kJTooManyLinks)
{
	const JUtf8Byte* map[] = { "name", name.GetBytes() };
	SetMessage(map, sizeof(map));
}


JNotCompressed::JNotCompressed
	(
	const JString& fileName
	)
	:
	JError(kJNotCompressed)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}
