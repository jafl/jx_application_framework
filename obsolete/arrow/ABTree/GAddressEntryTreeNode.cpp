/******************************************************************************
 GAddressEntryTreeNode.cc

	Data structure for JXFileTree.

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GAddressEntryTreeNode.h"
#include "GAddressItemTreeNode.h"
#include "GAddressBookTreeNode.h"
#include "GAddressBookMgr.h"
#include <GMGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

******************************************************************************/

GAddressEntryTreeNode::GAddressEntryTreeNode
	(
	JNamedTreeNode* parent,
	const char*	name,
	const JBoolean	isBranch
	)
	:
	GAddressBaseTreeNode(parent, name, isBranch),
	itsHasFcc(kJFalse),
	itsHasComment(kJFalse),
	itsIsDestructing(kJFalse)
{
}


/******************************************************************************
 Destructor

******************************************************************************/

GAddressEntryTreeNode::~GAddressEntryTreeNode()
{
	itsIsDestructing = kJTrue;
	GGetAddressBookMgr()->DeleteName(GetName());
}

/******************************************************************************
 GetFullPathAndName

 ******************************************************************************/

JString
GAddressEntryTreeNode::GetFullPathAndName()
	const
{
	const JTreeNode* parent = GetParent();
	if (parent == NULL)
		{
		return GetName();
		}

	const GAddressBaseTreeNode* node =
		dynamic_cast<const GAddressBaseTreeNode*>(parent);
	assert(node != NULL);
	return JString(node->GetFullPathAndName() + ":" + GetName());
}

/******************************************************************************
 OKToChange

 ******************************************************************************/

JBoolean
GAddressEntryTreeNode::OKToChange()
	const
{
	const JTreeNode* parent = GetParent();
	assert(parent != NULL);
	const GAddressBaseTreeNode* node =
		dynamic_cast<const GAddressBaseTreeNode*>(parent);
	assert(node != NULL);
	return node->OKToChange();
}

/******************************************************************************
 SetHasFcc (private)

 ******************************************************************************/

void
GAddressEntryTreeNode::SetHasFcc
	(
	const JBoolean fcc
	)
{
	if (itsIsDestructing)
		{
		return;
		}
	itsHasFcc = fcc;
	GGetAddressBookMgr()->AdjustProperties(this);
}

/******************************************************************************
 SetHasComment (private)

 ******************************************************************************/

void
GAddressEntryTreeNode::SetHasComment
	(
	const JBoolean comment
	)
{
	if (itsIsDestructing)
		{
		return;
		}
	itsHasComment = comment;
	GGetAddressBookMgr()->AdjustProperties(this);
}

/******************************************************************************
 Save (public)

 ******************************************************************************/

void
GAddressEntryTreeNode::Save
	(
	std::ostream& os
	)
{
	JTreeNode* jchild = GetChild(1);
	GAddressItemTreeNode* child =
		dynamic_cast<GAddressItemTreeNode*>(jchild);
	assert(child != NULL);
	JSize count = GetChildCount();
	child->GetText().Print(os);
	os << "\t";
	GetName().Print(os);
	os << "\t";
	GetFullAddress(kJTrue).Print(os);
	os << "\t";
	if (HasFcc())
		{
		JTreeNode* jnode = GetChild(count);
		child =
			dynamic_cast<GAddressItemTreeNode*>(jnode);
		assert(child != NULL);
		child->GetText().Print(os);
		}
	os << "\t";
	if (HasComment())
		{
		JIndex findex = count;
		if (HasFcc())
			{
			findex = count - 1;
			}
		JTreeNode* jnode = GetChild(findex);
		child =
			dynamic_cast<GAddressItemTreeNode*>(jnode);
		assert(child != NULL);
		child->GetText().Print(os);
		}
	os << std::endl;
}

/******************************************************************************
 GetFullAddress (public)

 ******************************************************************************/

JString
GAddressEntryTreeNode::GetFullAddress
	(
	const JBoolean withRet
	)
{
	const JSize count = GetChildCount();
	JSize expCount = 2;
	if (HasFcc())
		{
		expCount++;
		}
	if (HasComment())
		{
		expCount++;
		}
	JBoolean multi_address = kJFalse;
	if (expCount < count)
		{
		multi_address = kJTrue;
		}
	JString address;
	if (multi_address)
		{
		address = "(";
		}
	for (JSize i = 1; i <= count; i++)
		{
		JTreeNode* jchild = GetChild(i);
		GAddressItemTreeNode* child =
			dynamic_cast<GAddressItemTreeNode*>(jchild);
		assert(child != NULL);
		GAddressItemTreeNode::Type type = child->GetType();
		if (type == GAddressItemTreeNode::kEMail)
			{
			if (!multi_address)
				{
				address = child->GetText();
				}
			else
				{
				if (address != "(")
					{
					address += ",";
					if (withRet)
						{
						address += "\n   ";
						}
					}
				address += child->GetText();
				}
			}
		}
	if (multi_address)
		{
		address += ")";
		}
	return address;
}
