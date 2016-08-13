/******************************************************************************
 XDVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright © 2008 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "XDVarNode.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <JColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

XDVarNode::XDVarNode				// root node
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
	:
	CMVarNode(shouldUpdate)
{
}

XDVarNode::XDVarNode
	(
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	fullName,
	const JCharacter*	value
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
	JBoolean* isPointer
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
