/******************************************************************************
 LLDBVarNode.cpp

	BASE CLASS = public CMVarNode

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "LLDBVarNode.h"
#include "lldb/API/SBType.h"
#include "CMVarCommand.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBVarNode::LLDBVarNode			// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	CMVarNode(shouldUpdate)
{
}

LLDBVarNode::LLDBVarNode
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

LLDBVarNode::~LLDBVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
LLDBVarNode::GetFullName
	(
	bool* isPointer
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
	const JUtf8Byte* s = v.GetValue();
	if (s == nullptr)
		{
		s = "";
		}

	JString value(s);
	bool isPointer = false;
	bool isSpecial = false;

	JString name;
	if (v.GetName() != nullptr)
		{
		name = v.GetName();
		}

	if (v.TypeIsPointerType())
		{
		lldb::BasicType type = v.GetType().GetPointeeType().GetBasicType();

		if (type == lldb::eBasicTypeChar       ||
			type == lldb::eBasicTypeSignedChar ||
			type == lldb::eBasicTypeUnsignedChar)
			{
#ifdef _J_LLDB_HAS_SBVALUE_GETSUMMARY
			if (v.GetSummary() != nullptr)
				{
				value += "  ";
				value += v.GetSummary();
				}
#endif
			isSpecial = true;
			}
		else
			{
			isPointer = true;
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
			if (v.GetSummary() != nullptr)
				{
				value += "  ";
				value += v.GetSummary();
				}
#endif
			isSpecial = true;
			}
		}
	else if (v.GetType().GetTypeClass() == lldb::eTypeClassClass &&
			 !JString::IsEmpty(v.GetTypeName()) &&
			 name == v.GetTypeName())
		{
		// mark as "fake"
		name.Prepend("<");
		name.Append(">");
		}

	CMVarNode* node = CMGetLink()->CreateVarNode(nullptr, name, JString::empty, value);
	assert( node != nullptr );

	if (isPointer)
		{
		node->MakePointer(true);
		return node;
		}
	else if (isSpecial)
		{
		return node;
		}

	if (v.MightHaveChildren())
		{
		const JSize count = v.GetNumChildren();
		for (JUnsignedOffset i=0; i<count; i++)
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
