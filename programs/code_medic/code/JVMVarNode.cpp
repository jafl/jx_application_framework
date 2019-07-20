/******************************************************************************
 JVMVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "JVMVarNode.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMVarNode::JVMVarNode				// root node
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
	:
	CMVarNode(shouldUpdate)
{
}

JVMVarNode::JVMVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
	:
	CMVarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMVarNode::~JVMVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
JVMVarNode::GetFullName
	(
	JBoolean* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}
