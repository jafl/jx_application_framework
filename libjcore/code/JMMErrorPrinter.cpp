/******************************************************************************
 JMMErrorPrinter.cpp

	A simple JMMMonitor which listens for error messages from the memory
	manager and prints them to cout.  It is intended only as a proxy object
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
	const JCharacter* printErrors = getenv("JMM_NO_PRINT_ERRORS");
	if (printErrors != NULL && JStringCompare(printErrors, "yes", kJFalse) == 0)
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
		cout << "*** memory error: Block allocated as object at\n                     "
		     << record.GetNewFile() << ":" << record.GetNewLine()
		     << "\n                  was deleted as array at\n                     "
		     << record.GetDeleteFile()
		     << ":" << record.GetDeleteLine() << endl;
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
		cout << "*** memory error: Block allocated as array at\n                     "
		     << record.GetNewFile() << ":" << record.GetNewLine()
		     << "\n                  was deleted as object at\n                     "
		     << record.GetDeleteFile()
		     << ":" << record.GetDeleteLine() << endl;
		}
}

/******************************************************************************
 HandleUnallocatedDeletion (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleUnallocatedDeletion
	(
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	if (itsPrintErrorsFlag)
		{
		cout << "*** memory error: Block deleted as " << JMMRecord::TypeName(isArray)
		     << " at\n                     " << file << ":" << line
		     << "\n                  was never allocated." << endl;
		}
}

/******************************************************************************
 HandleMultipleDeletion (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleMultipleDeletion
	(
	const JMMRecord&  originalRecord,
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	if (itsPrintErrorsFlag)
		{
		cout << "*** memory error: Block deleted as " << JMMRecord::TypeName(isArray)
		     << " at\n                     " << file << ":" << line
		     << "\n                  was already deleted, most recently as "
		     << originalRecord.DeleteTypeName() << " at\n                     "
		     << originalRecord.GetDeleteFile() << ":" << originalRecord.GetDeleteLine()
		     << endl;
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
		cout << "*** memory error: Item allocated as "
		     << thisRecord.DeleteTypeName() << " at\n                     "
		     << thisRecord.GetNewFile() << ":" << thisRecord.GetNewLine()
		     << "\n                  was first allocated as "
		     << firstRecord.DeleteTypeName() << " at\n                     "
		     << firstRecord.GetNewFile() << ":" << firstRecord.GetNewLine()
		     << endl;
		}
}

/******************************************************************************
 HandleNULLDeleted (virtual protected)

 *****************************************************************************/

void
JMMErrorPrinter::HandleNULLDeleted
	(
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	if (itsPrintErrorsFlag)
		{
		cout << "*** memory error: Attempt to jdelete NULL as "
		     << JMMRecord::TypeName(isArray) << " at\n                     "
		     << file << ":" << line << endl;
		}
}
