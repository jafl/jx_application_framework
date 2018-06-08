/******************************************************************************
 JMMDebugErrorStream.cpp

	JMMMonitor which listens for error messages from the memory manager and
	sends them to jx_memory_debugger.

	This code could be written in a much more efficient way, but since it
	should ideally never be called, it doesn't matter.

	BASE CLASS = public JMMMonitor

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include <JMMDebugErrorStream.h>
#include <JMemoryManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMDebugErrorStream::JMMDebugErrorStream()
	:
	JMMMonitor()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMMDebugErrorStream::~JMMDebugErrorStream()
{
}

/******************************************************************************
 HandleObjectDeletedAsArray (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleObjectDeletedAsArray
	(
	const JMMRecord& record
	)
{
	JMemoryManager::SendError(
		"Block allocated as object at " +
		JString(record.GetNewFile(), kJFalse) + ":" +
		JString((JUInt64) record.GetNewLine()) +
		" was deleted as array at " +
		JString(record.GetDeleteFile(), kJFalse) + ":" +
		JString((JUInt64) record.GetDeleteLine()));
}

/******************************************************************************
 HandleArrayDeletedAsObject (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleArrayDeletedAsObject
	(
	const JMMRecord& record
	)
{
	JMemoryManager::SendError(
		"Block allocated as array at " +
		JString(record.GetNewFile(), kJFalse) + ":" +
		JString((JUInt64) record.GetNewLine()) +
		+ " was deleted as object at " +
		JString(record.GetDeleteFile(), kJFalse) + ":" +
		JString((JUInt64) record.GetDeleteLine()));
}

/******************************************************************************
 HandleUnallocatedDeletion (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleUnallocatedDeletion
	(
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	JMemoryManager::SendError(
		"Block deleted as " + JString(JMMRecord::TypeName(isArray), kJFalse) +
		" at " + JString(file, kJFalse) + ":" +
		JString((JUInt64) line) + " was never allocated.");
}

/******************************************************************************
 HandleMultipleDeletion (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleMultipleDeletion
	(
	const JMMRecord& originalRecord,
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	JMemoryManager::SendError(
		"Block deleted as " + JString(JMMRecord::TypeName(isArray), kJFalse) +
		" at " + JString(file, kJFalse) + ":" + JString((JUInt64) line) +
		" was already deleted, most recently as " +
		JString(originalRecord.DeleteTypeName(), kJFalse) + " at " +
		JString(originalRecord.GetDeleteFile(), kJFalse) +
		":" + JString((JUInt64) originalRecord.GetDeleteLine()));
}

/******************************************************************************
 HandleMultipleAllocation (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleMultipleAllocation
	(
	const JMMRecord& thisRecord,
	const JMMRecord& firstRecord
	)
{
	JMemoryManager::SendError(
		"Item allocated as " +
		JString(thisRecord.DeleteTypeName(), kJFalse) + " at " +
		JString(thisRecord.GetNewFile(), kJFalse) + ":" +
		JString((JUInt64) thisRecord.GetNewLine()) +
		" was first allocated as " +
		JString(firstRecord.DeleteTypeName(), kJFalse) + " at " +
		JString(firstRecord.GetNewFile(), kJFalse) + ":" +
		JString((JUInt64) firstRecord.GetNewLine()));
}

/******************************************************************************
 HandleNULLDeleted (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleNULLDeleted
	(
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	JMemoryManager::SendError(
		"Attempt to jdelete nullptr as " +
		JString(JMMRecord::TypeName(isArray), kJFalse) + " at " +
		JString(file, kJFalse) + ":" + JString((JUInt64) line));
}
