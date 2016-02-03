/******************************************************************************
 LLDBVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright © 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "LLDBVarNode.h"
#include "GDBVarNode.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <JColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBVarNode::LLDBVarNode			// root node
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
	:
	CMVarNode(shouldUpdate)
{
}

LLDBVarNode::LLDBVarNode
	(
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	value
	)
	:
	CMVarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LLDBVarNode::~LLDBVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be NULL.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
LLDBVarNode::GetFullName
	(
	JBoolean* isPointer
	)
	const
{
	return GDBVarNode::GetFullName(this, isPointer);
}
