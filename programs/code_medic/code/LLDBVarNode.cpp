/******************************************************************************
 LLDBVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright © 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "LLDBVarNode.h"
#include "lldb/API/SBType.h"
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
	return GetFullNameForCFamilyLanguage(isPointer);
}

/******************************************************************************
 BuildTree (static)

 ******************************************************************************/

CMVarNode*
LLDBVarNode::BuildTree
	(
	lldb::SBFrame& f,
	lldb::SBValue& v
	)
{
	const JCharacter* s = v.GetValue();
	if (s == NULL)
		{
		s = "";
		}

	JString value      = s;
	JBoolean isPointer = kJFalse;
	JBoolean isSpecial = kJFalse;

	if (v.TypeIsPointerType())
		{
		lldb::BasicType type = v.GetType().GetPointeeType().GetBasicType();

		if (type == lldb::eBasicTypeChar       ||
			type == lldb::eBasicTypeSignedChar ||
			type == lldb::eBasicTypeUnsignedChar)
			{
#ifdef _J_LLDB_HAS_SBVALUE_GETSUMMARY
			if (v.GetSummary() != NULL)
				{
				value += "  ";
				value += v.GetSummary();
				}
#endif
			isSpecial = kJTrue;
			}
		else
			{
			isPointer = kJTrue;
			}
		}
	else if (v.GetType().GetTypeClass() == lldb::eTypeClassArray)
		{
		lldb::BasicType type = v.GetChildAtIndex(0).GetType().GetBasicType();
		if (type == lldb::eBasicTypeChar       ||
			type == lldb::eBasicTypeSignedChar ||
			type == lldb::eBasicTypeUnsignedChar)
			{
#ifdef _J_LLDB_HAS_SBVALUE_GETSUMMARY
			if (v.GetSummary() != NULL)
				{
				value += "  ";
				value += v.GetSummary();
				}
#endif
			isSpecial = kJTrue;
			}
		}

	CMVarNode* node = (CMGetLink())->CreateVarNode(NULL, v.GetName(), NULL, value);
	assert( node != NULL );

	if (isPointer)
		{
		node->MakePointer(kJTrue);
		return node;
		}
	else if (isSpecial)
		{
		return node;
		}

	if (v.MightHaveChildren())
		{
		const JSize count = v.GetNumChildren();
		for (JIndex i=0; i<count; i++)
			{
			lldb::SBValue child = v.GetChildAtIndex(i, lldb::eDynamicDontRunTarget, true);
			if (child.IsValid())
				{
				CMVarNode* n = BuildTree(f, child);
				node->Append(n);	// avoid automatic update
				}
			}
		}

	return node;
}
