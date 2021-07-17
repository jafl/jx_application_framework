/******************************************************************************
 LLDBVarNode.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBVarNode
#define _H_LLDBVarNode

#include "CMVarNode.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"

class LLDBVarNode : public CMVarNode
{
public:

	LLDBVarNode(const bool shouldUpdate = true);

	LLDBVarNode(JTreeNode* parent, const JString& name, const JString& value);

	virtual	~LLDBVarNode();

	virtual JString	GetFullName(bool* isPointer = nullptr) const override;

	static CMVarNode*	BuildTree(lldb::SBFrame& frame, lldb::SBValue& value);

private:

	// not allowed

	LLDBVarNode(const LLDBVarNode& source);
	const LLDBVarNode& operator=(const LLDBVarNode& source);
};

#endif
