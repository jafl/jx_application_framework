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

	JVMVarNode(const JBoolean shouldUpdate = kJTrue);

	JVMVarNode(JTreeNode* parent, const JString& name,
			   const JString& fullName, const JString& value);

	virtual	~JVMVarNode();

	virtual JString	GetFullName(JBoolean* isPointer = nullptr) const override;

private:

	// not allowed

	JVMVarNode(const JVMVarNode& source);
	const JVMVarNode& operator=(const JVMVarNode& source);
};

#endif
