/******************************************************************************
 JStdError.h

	Defines the most common errors.

	Copyright (C) 1997-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JStdError
#define _H_JStdError

#include <JError.h>

// Types

extern const JUtf8Byte* kJGeneralIO;

extern const JUtf8Byte* kJSegFault;
extern const JUtf8Byte* kJNoProcessMemory;
extern const JUtf8Byte* kJNoKernelMemory;
extern const JUtf8Byte* kJMemoryNotReadable;
extern const JUtf8Byte* kJMemoryNotWriteable;

extern const JUtf8Byte* kJProgramNotAvailable;
extern const JUtf8Byte* kJDirEntryAlreadyExists;
extern const JUtf8Byte* kJDirEntryDoesNotExist;
extern const JUtf8Byte* kJNameTooLong;
extern const JUtf8Byte* kJFileBusy;
extern const JUtf8Byte* kJInvalidOpenMode;

extern const JUtf8Byte* kJAccessDenied;
extern const JUtf8Byte* kJFileSystemReadOnly;
extern const JUtf8Byte* kJFileSystemFull;
extern const JUtf8Byte* kJDeviceFull;

extern const JUtf8Byte* kJComponentNotDirectory;
extern const JUtf8Byte* kJNotSymbolicLink;
extern const JUtf8Byte* kJPathContainsLoop;
extern const JUtf8Byte* kJNoHomeDirectory;
extern const JUtf8Byte* kJBadPath;
extern const JUtf8Byte* kJDirectoryNotEmpty;
extern const JUtf8Byte* kJDirectoryBusy;

extern const JUtf8Byte* kJCantRenameFileToDirectory;
extern const JUtf8Byte* kJCantRenameAcrossFilesystems;
extern const JUtf8Byte* kJCantRenameToNonemptyDirectory;
extern const JUtf8Byte* kJDirectoryCantBeOwnChild;
extern const JUtf8Byte* kJTooManyLinks;

extern const JUtf8Byte* kJTriedToRemoveDirectory;
extern const JUtf8Byte* kJIsADirectory;

extern const JUtf8Byte* kJDescriptorNotOpen;
extern const JUtf8Byte* kJTooManyDescriptorsOpen;
extern const JUtf8Byte* kJNegativeDescriptor;
extern const JUtf8Byte* kJInvalidDescriptor;

extern const JUtf8Byte* kJWouldHaveBlocked;
extern const JUtf8Byte* kJNonBlockedSignal;

extern const JUtf8Byte* kJNotCompressed;

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

	JProgramNotAvailable(const JString& programName);
};

class JDirEntryAlreadyExists : public JError
{
public:

	JDirEntryAlreadyExists(const JString& fileName);
};

class JDirEntryDoesNotExist : public JError
{
public:

	JDirEntryDoesNotExist(const JString& fileName);
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

	JFileBusy(const JString& fileName);
};

class JInvalidOpenMode : public JError
{
public:

	JInvalidOpenMode(const JString& fileName);
};


class JAccessDenied : public JError
{
public:

	JAccessDenied(const JString& fileName);
	JAccessDenied(const JString& fileName1,
				  const JString& fileName2);
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

	JComponentNotDirectory(const JString& path);
	JComponentNotDirectory(const JString& path1,
						   const JString& path2);
};

class JNotSymbolicLink : public JError
{
public:

	JNotSymbolicLink(const JString& name);
};

class JPathContainsLoop : public JError
{
public:

	JPathContainsLoop(const JString& path);
	JPathContainsLoop(const JString& path1,
					  const JString& path2);
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

	JBadPath(const JString& path);
	JBadPath(const JString& path1,
			 const JString& path2);
};

class JDirectoryNotEmpty : public JError
{
public:

	JDirectoryNotEmpty(const JString& name);
};

class JDirectoryBusy : public JError
{
public:

	JDirectoryBusy(const JString& name);
};


class JCantRenameFileToDirectory : public JError
{
public:

	JCantRenameFileToDirectory(const JString& oldName,
							   const JString& newName);
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

	JTooManyLinks(const JString& name);
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

	JNotCompressed(const JString& fileName);
};

#endif
