/******************************************************************************
 JMMErrorPrinter.cpp

	A simple JMMMonitor which listens for error messages from the memory
	manager and prints them to std::cout.  It is intended only as a proxy object
	for JMemoryManager, so its environment variable is documented there and
	its public interface is exposed there.  Anyone wanting to do a similar
	job would need to create their own JMMMonitor anyway.

	BASE CLASS = public JMMMonitor

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include <JMMErrorPrinter.h>
#include <JMMRecord.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMErrorPrinter::JMMErrorPrinter
	(
	)
	:
	JMMMonitor(),
	itsPrintErrorsFlag(kJTrue)
{
	const JUtf8Byte* printErrors = getenv("JMM_NO_PRINT_ERRORS");
	if (printErrors != NULL && JString::Compare(printErrors, "yes", kJFalse) == 0)
		{
		itsPrintErrorsFlag = kJFalse;
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMMErrorPrinter::~JMMErrorPrinter()
{
}

/******************************************************************************
 HandleObjectDeletedAsArray (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleObjectDeletedAsArray
	(
	const JMMRecord& record
	)
{
	if (itsPrintErrorsFlag)
		{
		std::cout << "*** memory error: Block allocated as object at\n                     "
				  << record.GetNewFile() << ":" << record.GetNewLine()
				  << "\n                  was deleted as array at"
				  << "\n                     " << record.GetDeleteFile()
				  << ":" << record.GetDeleteLine() << std::endl;
		}
}

/******************************************************************************
 HandleArrayDeletedAsObject (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleArrayDeletedAsObject
	(
	const JMMRecord& record
	)
{
	if (itsPrintErrorsFlag)
		{
		std::cout << "*** memory error: Block allocated as array at\n                     "
				  << record.GetNewFile() << ":" << record.GetNewLine()
				  << "\n                  was deleted as object at"
				  << "\n                     " << record.GetDeleteFile()
				  << ":" << record.GetDeleteLine() << std::endl;
		}
}

/******************************************************************************
 HandleUnallocatedDeletion (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleUnallocatedDeletion
	(
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	if (itsPrintErrorsFlag)
		{
		std::cout << "*** memory error: Block deleted as "
				  << JMMRecord::TypeName(isArray) << " at"
				  << "\n                     " << file << ":" << line
				  << "\n                  was never allocated." << std::endl;
		}
}

/******************************************************************************
 HandleMultipleDeletion (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleMultipleDeletion
	(
	const JMMRecord& originalRecord,
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	if (itsPrintErrorsFlag)
		{
		std::cout << "*** memory error: Block deleted as "
				  << JMMRecord::TypeName(isArray) << " at"
				  << "\n                     " << file << ":" << line
				  << "\n                  was already deleted, most recently as "
				  << originalRecord.DeleteTypeName() << " at\n                     "
				  << originalRecord.GetDeleteFile() << ":" << originalRecord.GetDeleteLine()
				  << std::endl;
		}
}

/******************************************************************************
 HandleMultipleAllocation (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleMultipleAllocation
	(
	const JMMRecord& thisRecord,
	const JMMRecord& firstRecord
	)
{
	if (itsPrintErrorsFlag)
		{
		std::cout << "*** memory error: Item allocated as "
				  << thisRecord.DeleteTypeName() << " at\n                     "
				  << thisRecord.GetNewFile() << ":" << thisRecord.GetNewLine()
				  << "\n                  was first allocated as "
				  << firstRecord.DeleteTypeName() << " at"
				  << "\n                     "
				  << firstRecord.GetNewFile() << ":" << firstRecord.GetNewLine()
				  << std::endl;
		}
}

/******************************************************************************
 HandleNULLDeleted (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleNULLDeleted
	(
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	if (itsPrintErrorsFlag)
		{
		std::cout << "*** memory error: Attempt to delete NULL as "
				  << JMMRecord::TypeName(isArray) << " at"
				  << "\n                     "
				  << file << ":" << line << std::endl;
		}
}
