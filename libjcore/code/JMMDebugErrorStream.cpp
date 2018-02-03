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
		JString(record.GetNewFile(), 0, kJFalse) + ":" +
		JString(record.GetNewLine(), JString::kBase10) +
		" was deleted as array at " +
		JString(record.GetDeleteFile(), 0, kJFalse) + ":" +
		JString(record.GetDeleteLine(), JString::kBase10));
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
		JString(record.GetNewFile(), 0, kJFalse) + ":" +
		JString(record.GetNewLine(), JString::kBase10) +
		+ " was deleted as object at " +
		JString(record.GetDeleteFile(), 0, kJFalse) + ":" +
		JString(record.GetDeleteLine(), JString::kBase10));
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
		"Block deleted as " + JString(JMMRecord::TypeName(isArray), 0, kJFalse) +
		" at " + JString(file, 0, kJFalse) + ":" +
		JString(line, JString::kBase10) + " was never allocated.");
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
		"Block deleted as " + JString(JMMRecord::TypeName(isArray), 0, kJFalse) +
		" at " + JString(file, 0, kJFalse) + ":" + JString(line, JString::kBase10) +
		" was already deleted, most recently as " +
		JString(originalRecord.DeleteTypeName(), 0, kJFalse) + " at " +
		JString(originalRecord.GetDeleteFile(), 0, kJFalse) +
		":" + JString(originalRecord.GetDeleteLine(), JString::kBase10));
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
		JString(thisRecord.DeleteTypeName(), 0, kJFalse) + " at " +
		JString(thisRecord.GetNewFile(), 0, kJFalse) + ":" +
		JString(thisRecord.GetNewLine(), JString::kBase10) +
		" was first allocated as " +
		JString(firstRecord.DeleteTypeName(), 0, kJFalse) + " at " +
		JString(firstRecord.GetNewFile(), 0, kJFalse) + ":" +
		JString(firstRecord.GetNewLine(), JString::kBase10));
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
		"Attempt to jdelete NULL as " +
		JString(JMMRecord::TypeName(isArray), 0, kJFalse) + " at " +
		JString(file, 0, kJFalse) + ":" + JString(line, JString::kBase10));
}
