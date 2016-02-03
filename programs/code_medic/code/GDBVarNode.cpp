/******************************************************************************
 GDBVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright © 2008 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
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
	return GetFullName(this, isPointer);
}

/******************************************************************************
 GetFullName (static)

 ******************************************************************************/

JString
GDBVarNode::GetFullName
	(
	const CMVarNode*	node,
	JBoolean*			isPointer
	)
{
	JString str;
	if (node->IsRoot())
		{
		return str;
		}

	const GDBVarNode* parent = dynamic_cast<const GDBVarNode*>(node->GetVarParent());
	const JString& name      = node->GetName();
	if (parent->IsRoot())
		{
		str = "(" + name + ")";
		}
	else if (name.IsEmpty())
		{
		JIndex i;
		const JBoolean found = parent->FindChild(node, &i);
		assert( found );
		str = parent->GetFullName(isPointer);
		if (!str.BeginsWith("(") || !str.EndsWith(")"))
			{
			str.PrependCharacter('(');
			str.AppendCharacter(')');
			}
		str += "[" + JString(i-1, JString::kBase10) + "]";
		}
	else if (name.BeginsWith("<"))
		{
		if (isPointer != NULL)
			{
			*isPointer = parent->IsPointer();
			}
		str = parent->GetFullName(isPointer);
		}
	else if (name.BeginsWith("["))
		{
		str = parent->GetFullName(isPointer) + name;
		}
	else if (name.BeginsWith("*"))
		{
		str = GetPath(parent) + "(" + name + ")";
		}
	else
		{
		str = name;
		if (str.BeginsWith("static "))
			{
			str.RemoveSubstring(1,7);
			}
		str.Prepend(GetPath(parent));
		}

	return str;
}

/******************************************************************************
 GetPath (static private)

 ******************************************************************************/

JString
GDBVarNode::GetPath
	(
	const CMVarNode* node
	)
{
	JString str;
	if (node->IsRoot())
		{
		return str;
		}

	JBoolean isPointer = node->IsPointer();
	str  = node->GetFullName(&isPointer);
	str += isPointer ? "->" : ".";
	return str;
}
