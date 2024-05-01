/******************************************************************************
 JTreeList.cpp

	Converts a JTree into a linear list for display in a table.

	BASE CLASS = JContainer

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "JTreeList.h"
#include "JTree.h"
#include "JTreeNode.h"
#include "jAssert.h"

const JUtf8Byte* JTreeList::kNodeInserted = "NodeInserted::JTreeList";
const JUtf8Byte* JTreeList::kNodeRemoved  = "NodeRemoved::JTreeList";
const JUtf8Byte* JTreeList::kNodeChanged  = "NodeChanged::JTreeList";
const JUtf8Byte* JTreeList::kNodeOpened   = "NodeOpened::JTreeList";
const JUtf8Byte* JTreeList::kNodeClosed   = "NodeClosed::JTreeList";

/******************************************************************************
 Constructor

 ******************************************************************************/

JTreeList::JTreeList
	(
	JTree* tree
	)
	:
	JContainer()
{
	itsTree = tree;
	ListenTo(tree);

	itsWasOpenBeforeMoveFlag = false;

	itsVisibleNodeList = jnew JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
	itsOpenNodeList    = jnew JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);

	InstallCollection(itsVisibleNodeList);

	const JTreeNode* root = itsTree->GetRoot();
	assert( root->IsOpenable() );
	ShowChildren(0, root);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTreeList::~JTreeList()
{
	jdelete itsVisibleNodeList;
	jdelete itsOpenNodeList;
}

/******************************************************************************
 MakeVisible

 ******************************************************************************/

void
JTreeList::MakeVisible
	(
	const JTreeNode* node
	)
{
	const JTreeNode* parent;
	while (node->GetParent(&parent))
	{
		Open(parent);
		node = parent;
	}
}

/******************************************************************************
 Open

 ******************************************************************************/

bool
JTreeList::Open
	(
	const JTreeNode* node
	)
{
	if (node == itsTree->GetRoot() || IsOpen(node))
	{
		return true;
	}
	else if (node->OKToOpen())
	{
		itsOpenNodeList->Append(const_cast<JTreeNode*>(node));

		JIndex index;
		if (FindNode(node, &index))
		{
			ShowChildren(index, node);
			Broadcast(NodeOpened(node, index));
		}
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ShowChildren (private)

	Called by constructor with index 0.

 ******************************************************************************/

void
JTreeList::ShowChildren
	(
	const JIndex		index,
	const JTreeNode*	parent
	)
{
JIndex i;

	const JIndex childCount = parent->GetChildCount();

	// loop through children and insert them

	for (i=1; i<=childCount; i++)
	{
		const JTreeNode* node = parent->GetChild(i);
		InsertElement(index + i, node);
	}

	// loop backwards through children and recursively insert their children
	// (This avoids recalculating the index passed to ShowChildren()
	//  every time through the loop.)

	for (i=childCount; i>=1; i--)
	{
		const JTreeNode* node = parent->GetChild(i);
		if (IsOpen(node))
		{
			ShowChildren(index + i, node);
		}
	}
}

/******************************************************************************
 Close

 ******************************************************************************/

void
JTreeList::Close
	(
	const JTreeNode* node
	)
{
	JIndex openIndex;
	if (itsOpenNodeList->Find(node, &openIndex))
	{
		itsOpenNodeList->RemoveItem(openIndex);

		JIndex index;
		if (FindNode(node, &index))
		{
			JPtrArray<JTreeNode> nodeList(JPtrArrayT::kDeleteAll, 100);
			const_cast<JTreeNode*>(node)->CollectDescendants(&nodeList);

			JIndex j;
			for (const auto* n : nodeList)
			{
				if (FindNode(n, &j))
				{
					RemoveItem(j);
				}
			}

			Broadcast(NodeClosed(node, index));
		}
	}
}

/******************************************************************************
 Toggle

 ******************************************************************************/

void
JTreeList::Toggle
	(
	const JIndex index
	)
{
	if (IsOpen(index))
	{
		Close(index);
	}
	else
	{
		Open(index);
	}
}

void
JTreeList::Toggle
	(
	const JTreeNode* node
	)
{
	if (IsOpen(node))
	{
		Close(node);
	}
	else
	{
		Open(node);
	}
}

/******************************************************************************
 OpenSiblings

	Opens the node and its siblings.

 ******************************************************************************/

void
JTreeList::OpenSiblings
	(
	const JTreeNode* node
	)
{
	const JTreeNode* parent = node->GetParent();
	const JSize childCount  = parent->GetChildCount();
	for (JIndex i=1; i<=childCount; i++)
	{
		const JTreeNode* child = parent->GetChild(i);
		if (ShouldOpenSibling(child))
		{
			Open(child);
		}
	}
}

/******************************************************************************
 CloseSiblings

	Closes the node and its siblings.

 ******************************************************************************/

void
JTreeList::CloseSiblings
	(
	const JTreeNode* node
	)
{
	const JTreeNode* parent = node->GetParent();
	const JSize childCount  = parent->GetChildCount();
	for (JIndex i=1; i<=childCount; i++)
	{
		Close(parent->GetChild(i));
	}
}

/******************************************************************************
 ShouldOpenSibling (virtual)

	Returns true if sibling should be opened.

 ******************************************************************************/

bool
JTreeList::ShouldOpenSibling
	(
	const JTreeNode* node
	)
{
	return true;
}

/******************************************************************************
 OpenDescendants

	Opens the node and its descendants, down to maxDepth.
	If maxDepth == 1, only the node itself is opened.

 ******************************************************************************/

bool
JTreeList::OpenDescendants
	(
	const JTreeNode*	node,
	const JSize			maxDepth
	)
{
	JSize depth = 0;
	return CalledByOpenDescendants(node, &depth, maxDepth);
}

// private -- recursive

bool
JTreeList::CalledByOpenDescendants
	(
	const JTreeNode*	node,
	JSize*				depth,
	const JSize			maxDepth
	)
{
	if (!Open(node))
	{
		return false;
	}

	(*depth)++;
	if (*depth < maxDepth)
	{
		const JSize childCount = node->GetChildCount();
		for (JIndex i=1; i<=childCount; i++)
		{
			const JTreeNode* child = node->GetChild(i);
			if (ShouldOpenDescendant(child))
			{
				CalledByOpenDescendants(child, depth, maxDepth);
			}
		}
	}
	(*depth)--;

	return true;
}

/******************************************************************************
 CloseDescendants

	Closes the node and its descendants.

 ******************************************************************************/

void
JTreeList::CloseDescendants
	(
	const JTreeNode* node
	)
{
	Close(node);

	JPtrArray<JTreeNode> nodeList(JPtrArrayT::kDeleteAll, 100);
	const_cast<JTreeNode*>(node)->CollectDescendants(&nodeList);

	for (const auto* n : nodeList)
	{
		itsOpenNodeList->Remove(n);
	}
}

/******************************************************************************
 ShouldOpenDescendant (virtual)

	Returns true if sibling should be opened.

 ******************************************************************************/

bool
JTreeList::ShouldOpenDescendant
	(
	const JTreeNode* node
	)
{
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JTreeList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTree && message.Is(JTree::kNewRoot))
	{
		while (!itsVisibleNodeList->IsEmpty())
		{
			RemoveItem(1);
		}

		const JTreeNode* root = itsTree->GetRoot();
		assert( root->IsOpenable() );
		ShowChildren(0, root);
	}
	else if (sender == itsTree && message.Is(JTree::kNodeInserted))
	{
		auto* info = dynamic_cast<const JTree::NodeInserted*>(&message);
		assert( info != nullptr );
		const JIndex index = info->GetIndex();
		JTreeNode* parent  = info->GetParent();
		JTreeNode* node    = info->GetNode();

		// Since every node may have children, we have to insert the new
		// node just above the node it displaced.

		JIndex parentIndex;
		if (parent == itsTree->GetRoot())
		{
			if (index == parent->GetChildCount())
			{
				InsertElement(GetItemCount()+1, node);
			}
			else
			{
				InsertBefore(parent, index, node);
			}
		}
		else if (FindNode(parent, &parentIndex) && IsOpen(parentIndex))
		{
			if (index == parent->GetChildCount())
			{
				InsertElement(FindLastDescendant(parentIndex)+1, node);
			}
			else
			{
				InsertBefore(parent, index, node);
			}
		}
	}

	else if (sender == itsTree && message.Is(JTree::kNodeRemoved))
	{
		auto* info = dynamic_cast<const JTree::NodeRemoved*>(&message);
		assert( info != nullptr );
		JTreeNode* node = info->GetNode();
		JIndex index;
		if (FindNode(node, &index))
		{
			Close(index);
			RemoveItem(index);
		}
	}

	else if (sender == itsTree && message.Is(JTree::kNodeDeleted))
	{
		auto* info = dynamic_cast<const JTree::NodeDeleted*>(&message);
		assert( info != nullptr );
		JTreeNode* node = info->GetNode();
		itsOpenNodeList->Remove(node);
		JIndex index;
		if (FindNode(node, &index))
		{
			RemoveItem(index);
		}
	}

	else if (sender == itsTree && message.Is(JTree::kNodeChanged))
	{
		auto* info = dynamic_cast<const JTree::NodeChanged*>(&message);
		assert( info != nullptr );
		JTreeNode* node = info->GetNode();

		JIndex index;
		if (FindNode(node, &index))
		{
			Broadcast(NodeChanged(node, index));
		}

		if (!node->IsOpenable())
		{
			itsOpenNodeList->Remove(node);
		}
	}

	else if (sender == itsTree && message.Is(JTree::kPrepareForNodeMove))
	{
		auto* info = dynamic_cast<const JTree::PrepareForNodeMove*>(&message);
		assert( info != nullptr );
		itsWasOpenBeforeMoveFlag = IsOpen(info->GetNode());
	}

	else
	{
		JContainer::Receive(sender, message);
	}
}

/******************************************************************************
 InsertBefore (private)

 ******************************************************************************/

void
JTreeList::InsertBefore
	(
	const JTreeNode*	parent,
	const JIndex		index,
	const JTreeNode*	node
	)
{
	const JTreeNode* existingNode = parent->GetChild(index + 1);

	JIndex insertIndex;
	const bool found = FindNode(existingNode, &insertIndex);
	assert( found );
	InsertElement(insertIndex, node);
}

/******************************************************************************
 FindLastDescendant (private)

	Starting from the node at index, returns the index of the last visible
	descendant of this node.  This can return the same index if there are
	no visible descendants.

 ******************************************************************************/

JIndex
JTreeList::FindLastDescendant
	(
	const JIndex index
	)
	const
{
	const JSize startDepth = (GetNode(index))->GetDepth();

	JIndex i = index+1;
	while (IndexValid(i) && (GetNode(i))->GetDepth() > startDepth)
	{
		i++;
	}

	return i-1;
}

/******************************************************************************
 InsertElement (private)

 ******************************************************************************/

void
JTreeList::InsertElement
	(
	const JIndex		index,
	const JTreeNode*	node
	)
{
	itsVisibleNodeList->InsertAtIndex(index, const_cast<JTreeNode*>(node));
	Broadcast(NodeInserted(node, index));

	if (itsWasOpenBeforeMoveFlag)
	{
		itsWasOpenBeforeMoveFlag = false;
		Open(node);
	}
}

/******************************************************************************
 RemoveItem (private)

 ******************************************************************************/

void
JTreeList::RemoveItem
	(
	const JIndex index
	)
{
	const JTreeNode* node = GetNode(index);		// save before removing from list
	itsVisibleNodeList->RemoveItem(index);
	Broadcast(NodeRemoved(node, index));
}
