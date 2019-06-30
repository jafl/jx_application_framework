/******************************************************************************
 XDVarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDVarNode
#define _H_XDVarNode

#include "CMVarNode.h"

class XDVarNode : public CMVarNode
{
public:

	XDVarNode(const JBoolean shouldUpdate = kJTrue);

	XDVarNode(JTreeNode* parent, const JString& name,
			  const JString& fullName, const JString& value);

	virtual	~XDVarNode();

	virtual JString	GetFullName(JBoolean* isPointer = nullptr) const;

protected:

	virtual void	NameChanged();

private:

	JString	itsFullName;

private:

	// not allowed

	XDVarNode(const XDVarNode& source);
	const XDVarNode& operator=(const XDVarNode& source);
};

#endif
