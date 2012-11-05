/******************************************************************************
 GDBVarNode.h

	Copyright © 2008 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GDBVarNode
#define _H_GDBVarNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMVarNode.h"

class GDBVarNode : public CMVarNode
{
public:

	GDBVarNode(const JBoolean shouldUpdate = kJTrue);

	GDBVarNode(JTreeNode* parent, const JCharacter* name, const JCharacter* value);

	virtual	~GDBVarNode();

	virtual JString	GetFullName(JBoolean* isPointer = NULL) const;

private:

	JString	GetPath() const;

	// not allowed

	GDBVarNode(const GDBVarNode& source);
	const GDBVarNode& operator=(const GDBVarNode& source);
};

#endif
