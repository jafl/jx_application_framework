/******************************************************************************
 JNamedTreeList.cpp

	Adds useful functions for JNamedTreeNodes.

	BASE CLASS = JTreeList

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "JNamedTreeList.h"
#include "JNamedTreeNode.h"
#include "JTree.h"
#include "JString.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JNamedTreeList::JNamedTreeList
	(
	JTree* tree
	)
	:
	JTreeList(tree)
{
	itsSortedNodeList = jnew JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
	itsSortedNodeList->SetSortOrder(JListT::kSortAscending);
	itsSortedNodeList->SetCompareFunction(JNamedTreeNode::DynamicCastCompareNames);

	BuildSortedNodeList();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNamedTreeList::~JNamedTreeList()
{
	jdelete itsSortedNodeList;
}

/******************************************************************************
 GetNamedNode

	We assert if the dynamic_cast fails because all nodes are required to be
	JNamedTreeNodes.

 ******************************************************************************/

JNamedTreeNode*
JNamedTreeList::GetNamedNode
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<JNamedTreeNode*>(GetNode(index));
	assert( node != nullptr );
	return node;
}

const JNamedTreeNode*
JNamedTreeList::GetNamedNode
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const JNamedTreeNode*>(GetNode(index));
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 GetNodeName

 ******************************************************************************/

const JString&
JNamedTreeList::GetNodeName
	(
	const JIndex index
	)
	const
{
	return GetNamedNode(index)->GetName();
}

/******************************************************************************
 SetNodeName

 ******************************************************************************/

void
JNamedTreeList::SetNodeName
	(
	const JIndex	index,
	const JString&	name
	)
{
	GetNamedNode(index)->SetName(name);
}

/******************************************************************************
 Find

	Returns the index of the node with the given name.
	Returns false if the item does not exist or is *not visible*.

 ******************************************************************************/

bool
JNamedTreeList::Find
	(
	const JString&	name,
	JIndex*			index
	)
	const
{
	JNamedTreeNode target(nullptr, name);
	if (itsSortedNodeList->SearchSorted(&target, JListT::kFirstMatch, index))
	{
		const bool found = FindNode(itsSortedNodeList->GetItem(*index), index);
		assert( found );
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ClosestMatch

	Returns the index of the closest match for the given name prefix.
	Returns false if the list is empty.

 ******************************************************************************/

bool
JNamedTreeList::ClosestMatch
	(
	const JString&	prefixStr,
	JIndex*			index
	)
	const
{
	itsSortedNodeList->SetCompareFunction(JNamedTreeNode::DynamicCastCompareNamesForIncrSearch);

	JNamedTreeNode target(nullptr, prefixStr);
	bool found;
	*index = itsSortedNodeList->SearchSortedOTI(&target, JListT::kFirstMatch, &found);
	if (*index > itsSortedNodeList->GetItemCount())		// insert beyond end of list
	{
		*index = itsSortedNodeList->GetItemCount();
	}

	itsSortedNodeList->SetCompareFunction(JNamedTreeNode::DynamicCastCompareNames);

	if (*index > 0)
	{
		const bool found = FindNode(itsSortedNodeList->GetItem(*index), index);
		assert( found );
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JNamedTreeList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(kNodeInserted))
	{
		auto& info = dynamic_cast<const NodeInserted&>(message);
		itsSortedNodeList->InsertSorted(const_cast<JTreeNode*>(info.GetNode()));
	}

	else if (sender == this && message.Is(kNodeRemoved))
	{
		auto& info = dynamic_cast<const NodeRemoved&>(message);
		itsSortedNodeList->Remove(info.GetNode());
	}

	else if (sender == this && message.Is(kNodeChanged))
	{
		auto& info = dynamic_cast<const NodeChanged&>(message);
		itsSortedNodeList->Remove(info.GetNode());
		itsSortedNodeList->InsertSorted(const_cast<JTreeNode*>(info.GetNode()));

		const JTreeNode* node = info.GetNode();
		if (node != GetTree()->GetRoot())
		{
			itsSortedNodeList->Remove(node);
			itsSortedNodeList->InsertSorted(const_cast<JTreeNode*>(node));
		}
	}

	else
	{
		JTreeList::Receive(sender, message);
	}
}

/******************************************************************************
 BuildSortedNodeList (private)

	Sorts the nodes that are already in the tree when we are created.

 ******************************************************************************/

void
JNamedTreeList::BuildSortedNodeList()
{
	itsSortedNodeList->RemoveAll();

	// we have to do the root's children by hand because root isn't visible or open

	JTreeNode* root   = GetTree()->GetRoot();
	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		CalledByBuildSortedNodeList(root->GetChild(i));
	}
}

void
JNamedTreeList::CalledByBuildSortedNodeList
	(
	JTreeNode* node
	)
{
	if (IsVisible(node))
	{
		itsSortedNodeList->InsertSorted(node);

		if (IsOpen(node))
		{
			const JSize count = node->GetChildCount();
			for (JIndex i=1; i<=count; i++)
			{
				CalledByBuildSortedNodeList(node->GetChild(i));
			}
		}
	}
}
