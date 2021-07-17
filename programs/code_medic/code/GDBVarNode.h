/******************************************************************************
 GDBVarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBVarNode
#define _H_GDBVarNode

#include "CMVarNode.h"

class GDBVarNode : public CMVarNode
{
public:

	GDBVarNode(const bool shouldUpdate = true);

	GDBVarNode(JTreeNode* parent, const JString& name, const JString& value);

	virtual	~GDBVarNode();

	virtual JString	GetFullName(bool* isPointer = nullptr) const override;

private:

	// not allowed

	GDBVarNode(const GDBVarNode& source);
	const GDBVarNode& operator=(const GDBVarNode& source);
};

#endif
