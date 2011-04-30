/******************************************************************************
 JTreeList.cpp

	Converts a JTree into a linear list for display in a table.

	BASE CLASS = JContainer

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTreeList.h>
#include <JTree.h>
#include <JTreeNode.h>
#include <jAssert.h>

const JCharacter* JTreeList::kNodeInserted = "NodeInserted::JTreeList";
const JCharacter* JTreeList::kNodeRemoved  = "NodeRemoved::JTreeList";
const JCharacter* JTreeList::kNodeChanged  = "NodeChanged::JTreeList";
const JCharacter* JTreeList::kNodeOpened   = "NodeOpened::JTreeList";
const JCharacter* JTreeList::kNodeClosed   = "NodeClosed::JTreeList";

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

	itsWasOpenBeforeMoveFlag = kJFalse;

	itsVisibleNodeList = new JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
	assert( itsVisibleNodeList != NULL );

	itsOpenNodeList = new JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
	assert( itsOpenNodeList != NULL );

	InstallOrderedSet(itsVisibleNodeList);

	const JTreeNode* root = itsTree->GetRoot();
	assert( root->IsOpenable() );
	ShowChildren(0, root);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTreeList::~JTreeList()
{
	delete itsVisibleNodeList;
	delete itsOpenNodeList;
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

JBoolean
JTreeList::Open
	(
	const JTreeNode* node
	)
{
	if (node == itsTree->GetRoot() || IsOpen(node))
		{
		return kJTrue;
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
		return kJTrue;
		}
	else
		{
		return kJFalse;
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
		itsOpenNodeList->RemoveElement(openIndex);

		JIndex index;
		if (FindNode(node, &index))
			{
			JPtrArray<JTreeNode> nodeList(JPtrArrayT::kDeleteAll, 100);
			const_cast<JTreeNode*>(node)->CollectDescendants(&nodeList);

			const JSize count = nodeList.GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				const JTreeNode* n = nodeList.NthElement(i);
				JIndex j;
				if (FindNode(n, &j))
					{
					RemoveElement(j);
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

	Returns kJTrue if sibling should be opened.

 ******************************************************************************/

JBoolean
JTreeList::ShouldOpenSibling
	(
	const JTreeNode* node
	)
{
	return kJTrue;
}

/******************************************************************************
 OpenDescendants

	Opens the node and its descendants, down to maxDepth.
	If maxDepth == 1, only the node itself is opened.

 ******************************************************************************/

JBoolean
JTreeList::OpenDescendants
	(
	const JTreeNode*	node,
	const JSize			maxDepth
	)
{
	JSize depth = 0;
	return OpenDescendants1(node, &depth, maxDepth);
}

// private -- recursive

JBoolean
JTreeList::OpenDescendants1
	(
	const JTreeNode*	node,
	JSize*				depth,
	const JSize			maxDepth
	)
{
	if (!Open(node))
		{
		return kJFalse;
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
				OpenDescendants1(child, depth, maxDepth);
				}
			}
		}
	(*depth)--;

	return kJTrue;
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

	const JSize count = nodeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsOpenNodeList->Remove(nodeList.NthElement(i));
		}
}

/******************************************************************************
 ShouldOpenDescendant (virtual)

	Returns kJTrue if sibling should be opened.

 ******************************************************************************/

JBoolean
JTreeList::ShouldOpenDescendant
	(
	const JTreeNode* node
	)
{
	return kJTrue;
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
			RemoveElement(1);
			}

		const JTreeNode* root = itsTree->GetRoot();
		assert( root->IsOpenable() );
		ShowChildren(0, root);
		}
	else if (sender == itsTree && message.Is(JTree::kNodeInserted))
		{
		const JTree::NodeInserted* info =
			dynamic_cast<const JTree::NodeInserted*>(&message);
		assert( info != NULL );
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
				InsertElement(GetElementCount()+1, node);
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
		const JTree::NodeRemoved* info =
			dynamic_cast<const JTree::NodeRemoved*>(&message);
		assert( info != NULL );
		JTreeNode* node = info->GetNode();
		JIndex index;
		if (FindNode(node, &index))
			{
			Close(index);
			RemoveElement(index);
			}
		}

	else if (sender == itsTree && message.Is(JTree::kNodeDeleted))
		{
		const JTree::NodeDeleted* info =
			dynamic_cast<const JTree::NodeDeleted*>(&message);
		assert( info != NULL );
		JTreeNode* node = info->GetNode();
		itsOpenNodeList->Remove(node);
		JIndex index;
		if (FindNode(node, &index))
			{
			RemoveElement(index);
			}
		}

	else if (sender == itsTree && message.Is(JTree::kNodeChanged))
		{
		const JTree::NodeChanged* info =
			dynamic_cast<const JTree::NodeChanged*>(&message);
		assert( info != NULL );
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
		const JTree::PrepareForNodeMove* info =
			dynamic_cast<const JTree::PrepareForNodeMove*>(&message);
		assert( info != NULL );
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
	const JBoolean found = FindNode(existingNode, &insertIndex);
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
		itsWasOpenBeforeMoveFlag = kJFalse;
		Open(node);
		}
}

/******************************************************************************
 RemoveElement (private)

 ******************************************************************************/

void
JTreeList::RemoveElement
	(
	const JIndex index
	)
{
	const JTreeNode* node = GetNode(index);		// save before removing from list
	itsVisibleNodeList->RemoveElement(index);
	Broadcast(NodeRemoved(node, index));
}
