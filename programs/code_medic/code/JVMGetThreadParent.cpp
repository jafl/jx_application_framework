/******************************************************************************
 JVMGetThreadParent.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreadParent.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreadParent::JVMGetThreadParent
	(
	JVMThreadNode*	node,
	const JBoolean	checkOnly
	)
	:
	CMCommand("", kJTrue, kJFalse),
	itsNode(node),
	itsCheckOnlyFlag(checkOnly)
{
	ClearWhenGoingAway(itsNode, &itsNode);
	CMCommand::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreadParent::~JVMGetThreadParent()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetThreadParent::Starting()
{
	CMCommand::Starting();

	if (itsNode != nullptr)
		{
		JVMLink* link = dynamic_cast<JVMLink*>CMGetLink();

		const JSize length  = link->GetObjectIDSize();
		unsigned char* data = (unsigned char*) calloc(length, 1);
		assert( data != nullptr );

		JVMSocket::Pack(length, itsNode->GetID(), data);

		const JBoolean isGroup = JI2B(itsNode->GetType() == JVMThreadNode::kGroupType);
		link->Send(this,
			isGroup ? JVMLink::kThreadGroupReferenceCmdSet : JVMLink::kThreadReferenceCmdSet,
			isGroup ? JVMLink::kTGParentCmd : JVMLink::kTThreadGroupCmd,
			data, length);

		free(data);
		}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreadParent::HandleSuccess
	(
	const JString& origData
	)
{
	JVMLink* link = dynamic_cast<JVMLink*>CMGetLink();
	const JVMSocket::MessageReady* msg;
	if (!link->GetLatestMessageFromJVM(&msg))
		{
		return;
		}

	if (itsNode != nullptr && !itsCheckOnlyFlag)
		{
		const unsigned char* data = msg->GetData();

		const JUInt64 id = JVMSocket::Unpack(link->GetObjectIDSize(), data);

		itsNode->FindParent(id);
		}
}

/******************************************************************************
 HandleFailure (virtual protected)

	The thread or group no longer exists.

 *****************************************************************************/

void
JVMGetThreadParent::HandleFailure()
{
	jdelete itsNode;
}
