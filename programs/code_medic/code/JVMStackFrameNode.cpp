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
	CMStackFrameNode(parent, id, "", "", 0),
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
		const JVMLink::IDResolved* info =
			dynamic_cast<const JVMLink::IDResolved*>(&message);
		assert( info != NULL );

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

JBoolean
JVMStackFrameNode::UpdateNodeName()
{
	JVMLink* link = dynamic_cast<JVMLink*>(CMGetLink());
	JString c, m;
	if (link->GetClassName(itsClassID, &c) &&
		link->GetMethodName(itsClassID, itsMethodID, &m))
		{
		c += ".";
		c += m;
		SetName(c);
		return kJTrue;
		}

	if (GetName().IsEmpty())
		{
		c = JString(itsClassID, JString::kBase10) + ":" +
			JString(itsMethodID, JString::kBase10) + ":" +
			JString(itsCodeOffset, JString::kBase16);
		SetName(c);
		}
	return kJFalse;
}
