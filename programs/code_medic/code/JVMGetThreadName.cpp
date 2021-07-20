/******************************************************************************
 JVMGetThreadName.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreadName.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreadName::JVMGetThreadName
	(
	JVMThreadNode* node
	)
	:
	CMCommand("", true, false),
	itsNode(node)
{
	ClearWhenGoingAway(itsNode, &itsNode);
	CMCommand::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreadName::~JVMGetThreadName()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetThreadName::Starting()
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
			isGroup ? JVMLink::kTGNameCmd : JVMLink::kTNameCmd,
			data, length);

		free(data);
		}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreadName::HandleSuccess
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

	if (itsNode != nullptr)
		{
		const unsigned char* data = msg->GetData();

		JSize count;
		const JString name = JVMSocket::UnpackString(data, &count);

		itsNode->SetName(name);
		}
}

/******************************************************************************
 HandleFailure (virtual protected)

	The thread or group no longer exists.

 *****************************************************************************/

void
JVMGetThreadName::HandleFailure()
{
	jdelete itsNode;
}
