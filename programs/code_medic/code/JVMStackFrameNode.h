/******************************************************************************
 JVMStackFrameNode.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JVMStackFrameNode
#define _H_JVMStackFrameNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMStackFrameNode.h"

class JVMStackFrameNode : public CMStackFrameNode
{
public:

	JVMStackFrameNode(JTreeNode* parent,
					  const JUInt64 id, const JUInt64 classID,
					  const JUInt64 methodID, const JUInt64 codeOffset);

	virtual	~JVMStackFrameNode();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const JUInt64	itsClassID;
	const JUInt64	itsMethodID;
	const JUInt64	itsCodeOffset;

private:

	JBoolean	UpdateNodeName();

	// not allowed

	JVMStackFrameNode(const JVMStackFrameNode& source);
	const JVMStackFrameNode& operator=(const JVMStackFrameNode& source);
};

#endif
