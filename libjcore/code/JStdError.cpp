/******************************************************************************
 JStdError.cpp

	Constants for the most common errno values.  Functions returning these
	errors from libc functions should guarantee that the error returned
	reflects only that call.

	Copyright © 1997-2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JStdError.h>
#include <jGlobals.h>
#include <jAssert.h>

// Types

const JCharacter* kJGeneralIO = "JGeneralIO";

const JCharacter* kJSegFault           = "JSegFault";
const JCharacter* kJNoProcessMemory    = "JNoProcessMemory";
const JCharacter* kJNoKernelMemory     = "JNoKernelMemory";
const JCharacter* kJMemoryNotReadable  = "JMemoryNotReadable";
const JCharacter* kJMemoryNotWriteable = "JMemoryNotWriteable";

const JCharacter* kJProgramNotAvailable   = "JProgramNotAvailable";
const JCharacter* kJDirEntryAlreadyExists = "JDirEntryAlreadyExists";
const JCharacter* kJDirEntryDoesNotExist  = "JDirEntryDoesNotExist";
const JCharacter* kJNameTooLong           = "JNameTooLong";
const JCharacter* kJFileBusy              = "JFileBusy";
const JCharacter* kJInvalidOpenMode       = "JInvalidOpenMode";

const JCharacter* kJAccessDenied       = "JAccessDenied";
const JCharacter* kJFileSystemReadOnly = "JFileSystemReadOnly";
const JCharacter* kJFileSystemFull     = "JFileSystemFull";
const JCharacter* kJDeviceFull         = "JDeviceFull";

const JCharacter* kJComponentNotDirectory  = "JComponentNotDirectory";
const JCharacter* kJNotSymbolicLink        = "JNotSymbolicLink";
const JCharacter* kJPathContainsLoop       = "JPathContainsLoop";
const JCharacter* kJNoHomeDirectory        = "JNoHomeDirectory";
const JCharacter* kJBadPath                = "JBadPath";
const JCharacter* kJDirectoryNotEmpty      = "JDirectoryNotEmpty";
const JCharacter* kJDirectoryBusy          = "JDirectoryBusy";

const JCharacter* kJCantRenameFileToDirectory     = "JCantRenameFileToDirectory";
const JCharacter* kJCantRenameAcrossFilesystems   = "JCantRenameAcrossFilesystems";
const JCharacter* kJCantRenameToNonemptyDirectory = "JCantRenameToNonemptyDirectory";
const JCharacter* kJDirectoryCantBeOwnChild       = "JDirectoryCantBeOwnChild";
const JCharacter* kJTooManyLinks                  = "JTooManyLinks";

const JCharacter* kJTriedToRemoveDirectory = "JTriedToRemoveDirectory";
const JCharacter* kJIsADirectory           = "JIsADirectory";

const JCharacter* kJDescriptorNotOpen      = "JDescriptorNotOpen";
const JCharacter* kJTooManyDescriptorsOpen = "JTooManyDescriptorsOpen";
const JCharacter* kJNegativeDescriptor     = "JNegativeDescriptor";
const JCharacter* kJInvalidDescriptor      = "JInvalidDescriptor";

const JCharacter* kJWouldHaveBlocked = "JWouldHaveBlocked";
const JCharacter* kJNonBlockedSignal = "JNonBlockedSignal";

const JCharacter* kJNotCompressed = "JNotCompressed";

/******************************************************************************
 Constructors

 *****************************************************************************/

static const JCharacter* kJStdErrorMsgMap[] =
	{
	"name", NULL
	};


JProgramNotAvailable::JProgramNotAvailable
	(
	const JCharacter* programName
	)
	:
	JError(kJProgramNotAvailable, "")
{
	kJStdErrorMsgMap[1] = programName;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JDirEntryAlreadyExists::JDirEntryAlreadyExists
	(
	const JCharacter* fileName
	)
	:
	JError(kJDirEntryAlreadyExists, "")
{
	kJStdErrorMsgMap[1] = fileName;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JDirEntryDoesNotExist::JDirEntryDoesNotExist
	(
	const JCharacter* fileName
	)
	:
	JError(kJDirEntryDoesNotExist, "")
{
	kJStdErrorMsgMap[1] = fileName;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JFileBusy::JFileBusy
	(
	const JCharacter* fileName
	)
	:
	JError(kJFileBusy, "")
{
	kJStdErrorMsgMap[1] = fileName;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JInvalidOpenMode::JInvalidOpenMode
	(
	const JCharacter* fileName
	)
	:
	JError(kJInvalidOpenMode, "")
{
	kJStdErrorMsgMap[1] = fileName;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}


JAccessDenied::JAccessDenied
	(
	const JCharacter* fileName
	)
	:
	JError(kJAccessDenied, "")
{
	kJStdErrorMsgMap[1] = fileName;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JAccessDenied::JAccessDenied
	(
	const JCharacter* fileName1,
	const JCharacter* fileName2
	)
	:
	JError(kJAccessDenied, "")
{
	const JCharacter* map[] =
		{
		"name1", fileName1,
		"name2", fileName2
		};
	const JString msg =
		JGetString("JAccessDenied2", map, sizeof(map));
	SetMessage(msg, kJTrue);
}


JComponentNotDirectory::JComponentNotDirectory
	(
	const JCharacter* path
	)
	:
	JError(kJComponentNotDirectory, "")
{
	kJStdErrorMsgMap[1] = path;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JComponentNotDirectory::JComponentNotDirectory
	(
	const JCharacter* path1,
	const JCharacter* path2
	)
	:
	JError(kJComponentNotDirectory, "")
{
	const JCharacter* map[] =
		{
		"name1", path1,
		"name2", path2
		};
	const JString msg =
		JGetString("JComponentNotDirectory2", map, sizeof(map));
	SetMessage(msg, kJTrue);
}

JNotSymbolicLink::JNotSymbolicLink
	(
	const JCharacter* name
	)
	:
	JError(kJNotSymbolicLink, "")
{
	kJStdErrorMsgMap[1] = name;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JPathContainsLoop::JPathContainsLoop
	(
	const JCharacter* path
	)
	:
	JError(kJPathContainsLoop, "")
{
	kJStdErrorMsgMap[1] = path;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JPathContainsLoop::JPathContainsLoop
	(
	const JCharacter* path1,
	const JCharacter* path2
	)
	:
	JError(kJPathContainsLoop, "")
{
	const JCharacter* map[] =
		{
		"name1", path1,
		"name2", path2
		};
	const JString msg =
		JGetString("JPathContainsLoop2", map, sizeof(map));
	SetMessage(msg, kJTrue);
}

JBadPath::JBadPath
	(
	const JCharacter* path
	)
	:
	JError(kJBadPath, "")
{
	kJStdErrorMsgMap[1] = path;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JBadPath::JBadPath
	(
	const JCharacter* path1,
	const JCharacter* path2
	)
	:
	JError(kJBadPath, "")
{
	const JCharacter* map[] =
		{
		"name1", path1,
		"name2", path2
		};
	const JString msg =
		JGetString("JBadPath2", map, sizeof(map));
	SetMessage(msg, kJTrue);
}

JDirectoryNotEmpty::JDirectoryNotEmpty
	(
	const JCharacter* name
	)
	:
	JError(kJDirectoryNotEmpty, "")
{
	kJStdErrorMsgMap[1] = name;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}

JDirectoryBusy::JDirectoryBusy
	(
	const JCharacter* name
	)
	:
	JError(kJDirectoryBusy, "")
{
	kJStdErrorMsgMap[1] = name;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}


JCantRenameFileToDirectory::JCantRenameFileToDirectory
	(
	const JCharacter* oldName,
	const JCharacter* newName
	)
	:
	JError(kJCantRenameFileToDirectory, "")
{
	const JCharacter* map[] =
		{
		"old_name", oldName,
		"new_name", newName
		};
	SetMessage(map, sizeof(map));
}

JTooManyLinks::JTooManyLinks
	(
	const JCharacter* name
	)
	:
	JError(kJTooManyLinks, "")
{
	kJStdErrorMsgMap[1] = name;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}


JNotCompressed::JNotCompressed
	(
	const JCharacter* fileName
	)
	:
	JError(kJNotCompressed, "")
{
	kJStdErrorMsgMap[1] = fileName;
	SetMessage(kJStdErrorMsgMap, sizeof(kJStdErrorMsgMap));
}
