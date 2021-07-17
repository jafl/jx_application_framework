/******************************************************************************
 XDVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "XDVarNode.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

XDVarNode::XDVarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	CMVarNode(shouldUpdate)
{
}

XDVarNode::XDVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
	:
	CMVarNode(parent, name, value)
{
	itsFullName = fullName;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

XDVarNode::~XDVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

 ******************************************************************************/

JString
XDVarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return itsFullName;
}

/******************************************************************************
 NameChanged (virtual protected)

 ******************************************************************************/

void
XDVarNode::NameChanged()
{
	if (HasTree() && GetDepth() == 1)
		{
		itsFullName = GetName();
		}

	CMVarNode::NameChanged();
}
