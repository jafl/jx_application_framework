/******************************************************************************
 LLDBVarNode.h

	Copyright © 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_LLDBVarNode
#define _H_LLDBVarNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMVarNode.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"

class LLDBVarNode : public CMVarNode
{
public:

	LLDBVarNode(const JBoolean shouldUpdate = kJTrue);

	LLDBVarNode(JTreeNode* parent, const JCharacter* name, const JCharacter* value);

	virtual	~LLDBVarNode();

	virtual JString	GetFullName(JBoolean* isPointer = NULL) const;

	static CMVarNode*	BuildTree(lldb::SBFrame& frame, lldb::SBValue& value);

private:

	// not allowed

	LLDBVarNode(const LLDBVarNode& source);
	const LLDBVarNode& operator=(const LLDBVarNode& source);
};

#endif
