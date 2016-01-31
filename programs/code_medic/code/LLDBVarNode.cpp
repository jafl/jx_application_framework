/******************************************************************************
 LLDBVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright © 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "LLDBVarNode.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <JColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBVarNode::LLDBVarNode				// root node
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
	JString str;
	if (IsRoot())
		{
		return str;
		}

	const LLDBVarNode* parent = dynamic_cast<const LLDBVarNode*>(GetVarParent());
	const JString& name      = GetName();
	if (parent->IsRoot())
		{
		str = "(" + name + ")";
		}
	else if (name.IsEmpty())
		{
		JIndex i;
		const JBoolean found = parent->FindChild(this, &i);
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
		str = parent->GetPath() + "(" + name + ")";
		}
	else
		{
		str = name;
		if (str.BeginsWith("static "))
			{
			str.RemoveSubstring(1,7);
			}
		str.Prepend(parent->GetPath());
		}

	return str;
}

/******************************************************************************
 GetPath (private)

 ******************************************************************************/

JString
LLDBVarNode::GetPath()
	const
{
	JString str;
	if (IsRoot())
		{
		return str;
		}

	JBoolean isPointer = IsPointer();
	str  = GetFullName(&isPointer);
	str += isPointer ? "->" : ".";
	return str;
}
