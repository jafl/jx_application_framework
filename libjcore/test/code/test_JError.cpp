/******************************************************************************
 test_JStack.cpp

	Program to test JStack class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JStdError.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JUnknownError(0);
	JUnexpectedError(0);

	JGeneralIO();
	JSegFault();
	JNoProcessMemory();
	JNoKernelMemory();
	JMemoryNotReadable();
	JMemoryNotWriteable();
	JProgramNotAvailable e1(JString::empty);
	JDirEntryAlreadyExists e2(JString::empty);
	JDirEntryDoesNotExist e3(JString::empty);
	JNameTooLong();
	JFileBusy e4(JString::empty);
	JInvalidOpenMode e5(JString::empty);
	JAccessDenied e6(JString::empty);
	JAccessDenied e7(JString::empty, JString::empty);
	JFileSystemReadOnly();
	JFileSystemFull();
	JDeviceFull();
	JComponentNotDirectory e8(JString::empty);
	JComponentNotDirectory e9(JString::empty, JString::empty);
	JNotSymbolicLink e10(JString::empty);
	JPathContainsLoop e11(JString::empty);
	JPathContainsLoop e12(JString::empty, JString::empty);
	JNoHomeDirectory();
	JBadPath e13(JString::empty);
	JBadPath e14(JString::empty, JString::empty);
	JDirectoryNotEmpty e15(JString::empty);
	JDirectoryBusy e16(JString::empty);
	JCantRenameFileToDirectory e17(JString::empty, JString::empty);
	JCantRenameAcrossFilesystems();
	JCantRenameToNonemptyDirectory();
	JDirectoryCantBeOwnChild();
	JTooManyLinks e18(JString::empty);
	JTriedToRemoveDirectory();
	JIsADirectory();
	JDescriptorNotOpen();
	JTooManyDescriptorsOpen();
	JNegativeDescriptor();
	JInvalidDescriptor();
	JWouldHaveBlocked();
	JNonBlockedSignal();
	JNotCompressed e19(JString::empty);
}
