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
	const bool	checkOnly
	)
	:
	CMCommand("", true, false),
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
		auto* link = dynamic_cast<JVMLink*>(CMGetLink());

		const JSize length  = link->GetObjectIDSize();
		auto* data = (unsigned char*) calloc(length, 1);
		assert( data != nullptr );

		JVMSocket::Pack(length, itsNode->GetID(), data);

		const bool isGroup = itsNode->GetType() == JVMThreadNode::kGroupType;
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
	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
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
