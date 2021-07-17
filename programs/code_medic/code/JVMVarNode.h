/******************************************************************************
 JVMVarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMVarNode
#define _H_JVMVarNode

#include "CMVarNode.h"

class JVMVarNode : public CMVarNode
{
public:

	JVMVarNode(const bool shouldUpdate = true);

	JVMVarNode(JTreeNode* parent, const JString& name,
			   const JString& fullName, const JString& value);

	virtual	~JVMVarNode();

	virtual JString	GetFullName(bool* isPointer = nullptr) const override;

private:

	// not allowed

	JVMVarNode(const JVMVarNode& source);
	const JVMVarNode& operator=(const JVMVarNode& source);
};

#endif
