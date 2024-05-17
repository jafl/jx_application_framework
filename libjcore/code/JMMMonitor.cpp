/******************************************************************************
 JMMMonitor.cpp

	A (conceptually) abstract base class for objects which monitor JMemoryManager
	activity.  Derived classes override the appropriate message handlers to
	perform their actions.

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include "JMMMonitor.h"

#include "JMemoryManager.h"

#include "jAssert.h"

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMMonitor::JMMMonitor
	(
	)
{
	ListenTo( JMemoryManager::Instance() );
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMMMonitor::~JMMMonitor()
{
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
JMMMonitor::Receive
	(
	JBroadcaster*  sender,
	const Message& message
	)
{
	if (sender != JMemoryManager::Instance() )
	{
		return;
	}

	if ( message.Is(JMemoryManager::kObjectDeletedAsArray) )
	{
		auto& msg = dynamic_cast<const JMemoryManager::ObjectDeletedAsArray&>(message);
		HandleObjectDeletedAsArray( msg.GetRecord() );
	}
	else if ( message.Is(JMemoryManager::kArrayDeletedAsObject) )
	{
		auto& msg = dynamic_cast<const JMemoryManager::ArrayDeletedAsObject&>(message);
		HandleArrayDeletedAsObject( msg.GetRecord() );
	}
	else if ( message.Is(JMemoryManager::kUnallocatedDeletion) )
	{
		auto& msg = dynamic_cast<const JMemoryManager::UnallocatedDeletion&>(message);
		HandleUnallocatedDeletion( msg.GetFile(), msg.GetLine(), msg.IsArray() );
	}
	else if ( message.Is(JMemoryManager::kMultipleDeletion) )
	{
		auto& msg = dynamic_cast<const JMemoryManager::MultipleDeletion&>(message);
		HandleMultipleDeletion( msg.GetRecord(), msg.GetFile(),
								msg.GetLine(), msg.IsArray() );
	}
	else if ( message.Is(JMemoryManager::kMultipleAllocation) )
	{
		auto& msg = dynamic_cast<const JMemoryManager::MultipleAllocation&>(message);
		HandleMultipleAllocation( msg.GetThisRecord(), msg.GetFirstRecord() );
	}
}

/******************************************************************************
 HandleObjectDeletedAsArray (virtual protected)

 *****************************************************************************/

void
JMMMonitor::HandleObjectDeletedAsArray
	(
	const JMMRecord& record
	)
{
}

/******************************************************************************
 HandleArrayDeletedAsObject (virtual protected)

 *****************************************************************************/

void
JMMMonitor::HandleArrayDeletedAsObject
	(
	const JMMRecord& record
	)
{
}

/******************************************************************************
 HandleUnallocatedDeletion (virtual protected)

	Called when a block is deleted which was never allocated (more precisely,
	which has no allocation record; this includes multiple deletions if
	delete records are not being kept).

 *****************************************************************************/

void
JMMMonitor::HandleUnallocatedDeletion
	(
	const JUtf8Byte* file,
	const JUInt32    line,
	const bool   isArray
	)
{
}

/******************************************************************************
 HandleMultipleDeletion (virtual protected)

	Called when an already-deleted memory block is deleted again.  originalRecord
	is the JMMRecord for the block including its first deletion, file and line
	are the location of the multiple deletion that triggered the message.  This
	message cannot be generated unless delete records are being kept; otherwise,
	the message is UnallocatedDeletion rather than MultipleDeletion.

 *****************************************************************************/

void
JMMMonitor::HandleMultipleDeletion
	(
	const JMMRecord& originalRecord,
	const JUtf8Byte* file,
	const JUInt32    line,
	const bool   isArray
	)
{
}

/******************************************************************************
 HandleMultipleAllocation (virtual protected)

	Called when memory is allocated more than once at the same location
	(indicates bugs in malloc or JMM, not client code).  thisRecord is the
	record of the allocation that generated the message, firstRecord is the
	record of the first allocation at that address.

 *****************************************************************************/

void
JMMMonitor::HandleMultipleAllocation
	(
	const JMMRecord& thisRecord,
	const JMMRecord& firstRecord
	)
{
}
