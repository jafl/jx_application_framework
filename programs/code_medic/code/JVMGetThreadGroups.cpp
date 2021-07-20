/******************************************************************************
 JVMGetThreadGroups.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreadGroups.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreadGroups::JVMGetThreadGroups
	(
	JTreeNode*		root,
	JVMThreadNode*	parent
	)
	:
	CMCommand("", true, false),
	itsRoot(root),
	itsParent(parent)
{
	ListenTo(itsRoot);
	if (itsParent != nullptr)
		{
		ListenTo(itsParent);
		}

	CMCommand::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreadGroups::~JVMGetThreadGroups()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetThreadGroups::Starting()
{
	CMCommand::Starting();

	if (itsRoot == nullptr)
		{
		return;
		}

	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
	if (itsParent == nullptr)
		{
		link->Send(this,
			JVMLink::kVirtualMachineCmdSet, JVMLink::kVMTopLevelThreadGroupsCmd, nullptr, 0);
		}
	else
		{
		const JSize length  = link->GetObjectIDSize();
		auto* data = (unsigned char*) calloc(length, 1);
		assert( data != nullptr );

		JVMSocket::Pack(length, itsParent->GetID(), data);

		link->Send(this,
			JVMLink::kThreadGroupReferenceCmdSet, JVMLink::kTGChildrenCmd, data, length);

		free(data);
		}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreadGroups::HandleSuccess
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

	if (itsRoot == nullptr)
		{
		return;
		}

	const unsigned char* data = msg->GetData();
	const JSize idLength      = link->GetObjectIDSize();

	if (itsParent != nullptr)
		{
		const JSize threadCount = JVMSocket::Unpack4(data);
		data                   += 4;

		for (JIndex i=1; i<=threadCount; i++)
			{
			const JUInt64 id = JVMSocket::Unpack(idLength, data);
			data            += idLength;

			JVMThreadNode* node;
			if (!link->FindThread(id, &node))	// might be created by ThreadStartEvent
				{
				node = jnew JVMThreadNode(JVMThreadNode::kThreadType, id);
				assert( node != nullptr );

				itsParent->AppendThread(node);
				}
			}
		}

	const JSize groupCount = JVMSocket::Unpack4(data);
	data                  += 4;

	for (JIndex i=1; i<=groupCount; i++)
		{
		const JUInt64 id = JVMSocket::Unpack(idLength, data);
		data            += idLength;

		JVMThreadNode* node;
		if (!link->FindThread(id, &node))	// might be created by ThreadStartEvent
			{
			node = jnew JVMThreadNode(JVMThreadNode::kGroupType, id);
			assert( node != nullptr );

			if (itsParent != nullptr)
				{
				itsParent->AppendThread(node);
				}
			else
				{
				itsRoot->Append(node);
				}
			}
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

	Warning:	Since this function may be called from within a -chain-
				of destructors, it is not usually safe to do anything
				inside this function other than directly changing
				instance variables (e.g. setting pointers to nullptr).

	This function is not pure virtual because not all classes will want
	to implement it.

 ******************************************************************************/

void
JVMGetThreadGroups::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsRoot || sender == itsParent)
		{
		itsRoot   = nullptr;
		itsParent = nullptr;
		}

	JBroadcaster::ReceiveGoingAway(sender);
}
