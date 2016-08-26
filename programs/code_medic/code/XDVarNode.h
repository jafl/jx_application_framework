/******************************************************************************
 XDVarNode.h

	Copyright © 2008 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_XDVarNode
#define _H_XDVarNode

#include "CMVarNode.h"

class XDVarNode : public CMVarNode
{
public:

	XDVarNode(const JBoolean shouldUpdate = kJTrue);

	XDVarNode(JTreeNode* parent, const JCharacter* name,
			  const JCharacter* fullName, const JCharacter* value);

	virtual	~XDVarNode();

	virtual JString	GetFullName(JBoolean* isPointer = NULL) const;

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
