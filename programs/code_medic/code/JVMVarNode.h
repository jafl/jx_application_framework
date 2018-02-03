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

	JVMVarNode(JTreeNode* parent, const JCharacter* name,
			   const JCharacter* fullName, const JCharacter* value);

	virtual	~JVMVarNode();

	virtual JString	GetFullName(JBoolean* isPointer = NULL) const;

private:

	// not allowed

	JVMVarNode(const JVMVarNode& source);
	const JVMVarNode& operator=(const JVMVarNode& source);
};

#endif
