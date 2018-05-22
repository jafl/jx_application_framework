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
#include <JXColorManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBVarNode::GDBVarNode				// root node
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
	:
	CMVarNode(shouldUpdate)
{
}

GDBVarNode::GDBVarNode
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

GDBVarNode::~GDBVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be NULL.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
GDBVarNode::GetFullName
	(
	JBoolean* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}
