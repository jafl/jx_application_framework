/******************************************************************************
 GDBVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "GDBVarNode.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBVarNode::GDBVarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	CMVarNode(shouldUpdate)
{
}

GDBVarNode::GDBVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	value
	)
	:
	CMVarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBVarNode::~GDBVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
GDBVarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}
