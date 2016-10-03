/******************************************************************************
 JMMDebugErrorStream.cpp

	JMMMonitor which listens for error messages from the memory manager and
	sends them to jx_memory_debugger.

	This code could be written in a much more efficient way, but since it
	should ideally never be called, it doesn't matter.

	BASE CLASS = public JMMMonitor

	Copyright (C) 2010 by John Lindal.  All rights reserved.

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
		JString(record.GetNewFile()) + ":" +
		JString(record.GetNewLine(), JString::kBase10) +
		" was deleted as array at " +
		JString(record.GetDeleteFile()) + ":" +
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
		JString(record.GetNewFile()) + ":" +
		JString(record.GetNewLine(), JString::kBase10) +
		+ " was deleted as object at " +
		JString(record.GetDeleteFile()) + ":" +
		JString(record.GetDeleteLine(), JString::kBase10));
}

/******************************************************************************
 HandleUnallocatedDeletion (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleUnallocatedDeletion
	(
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	JMemoryManager::SendError(
		"Block deleted as " + JString(JMMRecord::TypeName(isArray)) +
		" at " + JString(file) + ":" +
		JString(line, JString::kBase10) + " was never allocated.");
}

/******************************************************************************
 HandleMultipleDeletion (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleMultipleDeletion
	(
	const JMMRecord&  originalRecord,
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	JMemoryManager::SendError(
		"Block deleted as " + JString(JMMRecord::TypeName(isArray)) +
		" at " + JString(file) + ":" + JString(line, JString::kBase10) +
		" was already deleted, most recently as " +
		JString(originalRecord.DeleteTypeName()) + " at " +
		JString(originalRecord.GetDeleteFile()) +
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
		JString(thisRecord.DeleteTypeName()) + " at " +
		JString(thisRecord.GetNewFile()) + ":" +
		JString(thisRecord.GetNewLine(), JString::kBase10) +
		" was first allocated as " +
		JString(firstRecord.DeleteTypeName()) + " at " +
		JString(firstRecord.GetNewFile()) + ":" +
		JString(firstRecord.GetNewLine(), JString::kBase10));
}

/******************************************************************************
 HandleNULLDeleted (virtual protected)

 *****************************************************************************/

void
JMMDebugErrorStream::HandleNULLDeleted
	(
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	JMemoryManager::SendError(
		"Attempt to delete NULL as " +
		JString(JMMRecord::TypeName(isArray)) + " at " +
		JString(file) + ":" + JString(line, JString::kBase10));
}
