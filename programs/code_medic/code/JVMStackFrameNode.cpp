/******************************************************************************
 JVMStackFrameNode.cpp

	BASE CLASS = public CMStackFrameNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "JVMStackFrameNode.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMStackFrameNode::JVMStackFrameNode
	(
	JTreeNode*		parent,
	const JUInt64	id,
	const JUInt64	classID,
	const JUInt64	methodID,
	const JUInt64	codeOffset
	)
	:
	CMStackFrameNode(parent, id, JString::empty, JString::empty, 0),
	itsClassID(classID),
	itsMethodID(methodID),
	itsCodeOffset(codeOffset)
{
	if (!UpdateNodeName())
		{
		ListenTo(CMGetLink());
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMStackFrameNode::~JVMStackFrameNode()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JVMStackFrameNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JVMLink::kIDResolved))
		{
		const auto* info =
			dynamic_cast<const JVMLink::IDResolved*>(&message);
		assert( info != nullptr );

		if (info->GetID() == itsClassID || info->GetID() == itsMethodID)
			{
			UpdateNodeName();	// can't stop listening, because base class might be listening, too
			}
		}

	else
		{
		CMStackFrameNode::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateNodeName

 *****************************************************************************/

bool
JVMStackFrameNode::UpdateNodeName()
{
	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
	JString c, m;
	if (link->GetClassName(itsClassID, &c) &&
		link->GetMethodName(itsClassID, itsMethodID, &m))
		{
		c += ".";
		c += m;
		SetName(c);
		return true;
		}

	if (GetName().IsEmpty())
		{
		c = JString((JUInt64) itsClassID) + ":" +
			JString((JUInt64) itsMethodID) + ":" +
			JString((JUInt64) itsCodeOffset);
		SetName(c);
		}
	return false;
}
