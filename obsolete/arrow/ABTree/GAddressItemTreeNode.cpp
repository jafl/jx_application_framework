/******************************************************************************
 GAddressItemTreeNode.cc

	Data structure for JXFileTree.

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GAddressItemTreeNode.h"
#include "GAddressEntryTreeNode.h"
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GAddressItemTreeNode::GAddressItemTreeNode
	(
	Type			type,
	JNamedTreeNode* parent,
	const char*		name,
	const JBoolean	isBranch
	)
	:
	GAddressBaseTreeNode(parent, name, isBranch),
	itsType(type),
	itsText(name)
{
	if (type == kName)
		{
		SetName("Nickname");
		}
	else if (type == kFcc)
		{
		SetName("Fcc");
		if (parent != NULL)
			{
			GAddressEntryTreeNode* eparent =
				dynamic_cast<GAddressEntryTreeNode*>(parent);
			assert(eparent != NULL);
			eparent->SetHasFcc(kJTrue);
			}
		}
	else if (type == kComment)
		{
		SetName("Comment");
		if (parent != NULL)
			{
			GAddressEntryTreeNode* eparent =
				dynamic_cast<GAddressEntryTreeNode*>(parent);
			assert(eparent != NULL);
			eparent->SetHasComment(kJTrue);
			}
		}
	else if (type == kEMail)
		{
		SetName("E-mail");
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GAddressItemTreeNode::~GAddressItemTreeNode()
{
	JTreeNode* jparent = GetParent();
	if (jparent != NULL)
		{
		GAddressEntryTreeNode* parent =
			dynamic_cast<GAddressEntryTreeNode*>(jparent);
		if (parent != NULL && itsType == kFcc)
			{
			parent->SetHasFcc(kJFalse);
			}
		else if (parent != NULL && itsType == kComment)
			{
			parent->SetHasComment(kJFalse);
			}
		}
}

/******************************************************************************
 GetFullPathAndName

 ******************************************************************************/

JString
GAddressItemTreeNode::GetFullPathAndName()
	const
{
	return GetName();
}

/******************************************************************************
 SetText (public)

 ******************************************************************************/

void
GAddressItemTreeNode::SetText
	(
	const JString& text
	)
{
	itsText = text;
}

/******************************************************************************
 OKToChange

 ******************************************************************************/

JBoolean
GAddressItemTreeNode::OKToChange()
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
 Save (public)

 ******************************************************************************/

void
GAddressItemTreeNode::Save
	(
	ostream& os
	)
{

}
