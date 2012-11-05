/******************************************************************************
 JVMVarNode.h

	Copyright © 2008 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JVMVarNode
#define _H_JVMVarNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	JString	GetPath() const;

	// not allowed

	JVMVarNode(const JVMVarNode& source);
	const JVMVarNode& operator=(const JVMVarNode& source);
};

#endif
