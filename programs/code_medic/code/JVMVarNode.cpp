/******************************************************************************
 JVMVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright © 2009 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "JVMVarNode.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <JColormap.h>
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
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	fullName,
	const JCharacter*	value
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

	isPointer can be NULL.  Its content is only modified if GetFullName()
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
