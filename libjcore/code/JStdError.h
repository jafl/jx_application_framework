/******************************************************************************
 JStdError.h

	Defines the most common errors.

	Copyright © 1997-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStdError
#define _H_JStdError

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JError.h>

// Types

extern const JCharacter* kJGeneralIO;

extern const JCharacter* kJSegFault;
extern const JCharacter* kJNoProcessMemory;
extern const JCharacter* kJNoKernelMemory;
extern const JCharacter* kJMemoryNotReadable;
extern const JCharacter* kJMemoryNotWriteable;

extern const JCharacter* kJProgramNotAvailable;
extern const JCharacter* kJDirEntryAlreadyExists;
extern const JCharacter* kJDirEntryDoesNotExist;
extern const JCharacter* kJNameTooLong;
extern const JCharacter* kJFileBusy;
extern const JCharacter* kJInvalidOpenMode;

extern const JCharacter* kJAccessDenied;
extern const JCharacter* kJFileSystemReadOnly;
extern const JCharacter* kJFileSystemFull;
extern const JCharacter* kJDeviceFull;

extern const JCharacter* kJComponentNotDirectory;
extern const JCharacter* kJNotSymbolicLink;
extern const JCharacter* kJPathContainsLoop;
extern const JCharacter* kJNoHomeDirectory;
extern const JCharacter* kJBadPath;
extern const JCharacter* kJDirectoryNotEmpty;
extern const JCharacter* kJDirectoryBusy;

extern const JCharacter* kJCantRenameFileToDirectory;
extern const JCharacter* kJCantRenameAcrossFilesystems;
extern const JCharacter* kJCantRenameToNonemptyDirectory;
extern const JCharacter* kJDirectoryCantBeOwnChild;
extern const JCharacter* kJTooManyLinks;

extern const JCharacter* kJTriedToRemoveDirectory;
extern const JCharacter* kJIsADirectory;

extern const JCharacter* kJDescriptorNotOpen;
extern const JCharacter* kJTooManyDescriptorsOpen;
extern const JCharacter* kJNegativeDescriptor;
extern const JCharacter* kJInvalidDescriptor;

extern const JCharacter* kJWouldHaveBlocked;
extern const JCharacter* kJNonBlockedSignal;

extern const JCharacter* kJNotCompressed;

// Classes

class JGeneralIO : public JError
{
public:

	JGeneralIO()
		:
		JError(kJGeneralIO)
		{ };
};


class JSegFault : public JError
{
public:

	JSegFault()
		:
		JError(kJSegFault)
		{ };
};

class JNoProcessMemory : public JError
{
public:

	JNoProcessMemory()
		:
		JError(kJNoProcessMemory)
		{ };
};

class JNoKernelMemory : public JError
{
public:

	JNoKernelMemory()
		:
		JError(kJNoKernelMemory)
		{ };
};

class JMemoryNotReadable : public JError
{
public:

	JMemoryNotReadable()
		:
		JError(kJMemoryNotReadable)
		{ };
};

class JMemoryNotWriteable : public JError
{
public:

	JMemoryNotWriteable()
		:
		JError(kJMemoryNotWriteable)
		{ };
};


class JProgramNotAvailable : public JError
{
public:

	JProgramNotAvailable(const JCharacter* programName);
};

class JDirEntryAlreadyExists : public JError
{
public:

	JDirEntryAlreadyExists(const JCharacter* fileName);
};

class JDirEntryDoesNotExist : public JError
{
public:

	JDirEntryDoesNotExist(const JCharacter* fileName);
};

class JNameTooLong : public JError
{
public:

	JNameTooLong()
		:
		JError(kJNameTooLong)
		{ };
};

class JFileBusy : public JError
{
public:

	JFileBusy(const JCharacter* fileName);
};

class JInvalidOpenMode : public JError
{
public:

	JInvalidOpenMode(const JCharacter* fileName);
};


class JAccessDenied : public JError
{
public:

	JAccessDenied(const JCharacter* fileName);
	JAccessDenied(const JCharacter* fileName1,
				  const JCharacter* fileName2);
};

class JFileSystemReadOnly : public JError
{
public:

	JFileSystemReadOnly()
		:
		JError(kJFileSystemReadOnly)
		{ };
};

class JFileSystemFull : public JError
{
public:

	JFileSystemFull()
		:
		JError(kJFileSystemFull)
		{ };
};

class JDeviceFull : public JError
{
public:

	JDeviceFull()
		:
		JError(kJDeviceFull)
		{ };
};


class JComponentNotDirectory : public JError
{
public:

	JComponentNotDirectory(const JCharacter* path);
	JComponentNotDirectory(const JCharacter* path1,
						   const JCharacter* path2);
};

class JNotSymbolicLink : public JError
{
public:

	JNotSymbolicLink(const JCharacter* name);
};

class JPathContainsLoop : public JError
{
public:

	JPathContainsLoop(const JCharacter* path);
	JPathContainsLoop(const JCharacter* path1,
					  const JCharacter* path2);
};

class JNoHomeDirectory : public JError
{
public:

	JNoHomeDirectory()
		:
		JError(kJNoHomeDirectory)
		{ };
};

class JBadPath : public JError
{
public:

	JBadPath(const JCharacter* path);
	JBadPath(const JCharacter* path1,
			 const JCharacter* path2);
};

class JDirectoryNotEmpty : public JError
{
public:

	JDirectoryNotEmpty(const JCharacter* name);
};

class JDirectoryBusy : public JError
{
public:

	JDirectoryBusy(const JCharacter* name);
};


class JCantRenameFileToDirectory : public JError
{
public:

	JCantRenameFileToDirectory(const JCharacter* oldName,
							   const JCharacter* newName);
};

class JCantRenameAcrossFilesystems : public JError
{
public:

	JCantRenameAcrossFilesystems()
		:
		JError(kJCantRenameAcrossFilesystems)
		{ };
};

class JCantRenameToNonemptyDirectory : public JError
{
public:

	JCantRenameToNonemptyDirectory()
		:
		JError(kJCantRenameToNonemptyDirectory)
		{ };
};

class JDirectoryCantBeOwnChild : public JError
{
public:

	JDirectoryCantBeOwnChild()
		:
		JError(kJDirectoryCantBeOwnChild)
		{ };
};

class JTooManyLinks : public JError
{
public:

	JTooManyLinks(const JCharacter* name);
};


class JTriedToRemoveDirectory : public JError
{
public:

	JTriedToRemoveDirectory()
		:
		JError(kJTriedToRemoveDirectory)
		{ };
};

class JIsADirectory : public JError
{
public:

	JIsADirectory()
		:
		JError(kJIsADirectory)
		{ };
};


class JDescriptorNotOpen : public JError
{
public:

	JDescriptorNotOpen()
		:
		JError(kJDescriptorNotOpen)
		{ };
};

class JTooManyDescriptorsOpen : public JError
{
public:

	JTooManyDescriptorsOpen()
		:
		JError(kJTooManyDescriptorsOpen)
		{ };
};

class JNegativeDescriptor : public JError
{
public:

	JNegativeDescriptor()
		:
		JError(kJNegativeDescriptor)
		{ };
};

class JInvalidDescriptor : public JError
{
public:

	JInvalidDescriptor()
		:
		JError(kJInvalidDescriptor)
		{ };
};


class JWouldHaveBlocked : public JError
{
public:

	JWouldHaveBlocked()
		:
		JError(kJWouldHaveBlocked)
		{ };
};

class JNonBlockedSignal : public JError
{
public:

	JNonBlockedSignal()
		:
		JError(kJNonBlockedSignal)
		{ };
};


class JNotCompressed : public JError
{
public:

	JNotCompressed(const JCharacter* fileName);
};

#endif
