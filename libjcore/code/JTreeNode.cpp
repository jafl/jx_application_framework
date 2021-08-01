/******************************************************************************
 JTreeNode.cpp

	Base class for elements stored by JTree.

	BASE CLASS = JBroadcaster
	(not virtual since this is root class, and it needs to be lightweight)

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "JTreeNode.h"
#include "JTree.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

	tree can be nullptr.  By not providing a constructor that takes a parent,
	we allow tree to be nullptr, and we allow JNamedTreeList to keep a sorted
	list of nodes.

 ******************************************************************************/

JTreeNode::JTreeNode
	(
	JTree*			tree,
	const bool	isOpenable
	)
	:
	itsIsOpenableFlag(isOpenable),
	itsIsDestructingFlag(false)
{
	itsTree      = tree;
	itsParent    = nullptr;
	itsChildren  = nullptr;
	itsCompareFn = nullptr;
	itsSortOrder = JListT::kSortAscending;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTreeNode::~JTreeNode()
{
	itsIsDestructingFlag = true;

	DeleteAllChildren();
	DisconnectFromParent();

	if (itsTree != nullptr)
		{
		itsTree->BroadcastDelete(this);
		}
}

/******************************************************************************
 IsRoot

 ******************************************************************************/

bool
JTreeNode::IsRoot()
	const
{
	return (itsTree != nullptr && itsTree->GetRoot() == this) ||
				 (itsTree == nullptr && itsParent == nullptr);
}

/******************************************************************************
 GetTree

 ******************************************************************************/

JTree*
JTreeNode::GetTree()
{
	assert( itsTree != nullptr );
	return itsTree;
}

const JTree*
JTreeNode::GetTree()
	const
{
	assert( itsTree != nullptr );
	return itsTree;
}

/******************************************************************************
 SetTree (private)

 ******************************************************************************/

void
JTreeNode::SetTree
	(
	JTree* tree
	)
{
	if (!itsIsDestructingFlag && tree != itsTree)
		{
		itsTree = tree;
		if (itsChildren != nullptr)
			{
			for (auto* n : *itsChildren)
				{
				n->SetTree(tree);
				}
			}
		}
}

/******************************************************************************
 GetParent

 ******************************************************************************/

JTreeNode*
JTreeNode::GetParent()
{
	assert( itsParent != nullptr );
	return itsParent;
}

const JTreeNode*
JTreeNode::GetParent()
	const
{
	assert( itsParent != nullptr );
	return itsParent;
}

/******************************************************************************
 OKToOpen (virtual)

	Not inline because it is virtual.

 ******************************************************************************/

bool
JTreeNode::OKToOpen()
	const
{
	return IsOpenable();
}

/******************************************************************************
 ShouldBeOpenable (protected)

 ******************************************************************************/

void
JTreeNode::ShouldBeOpenable
	(
	const bool openable
	)
{
	if (itsIsOpenableFlag != openable)
		{
		if (!openable)
			{
			DeleteAllChildren();
			}
		itsIsOpenableFlag = openable;
		if (itsTree != nullptr)
			{
			itsTree->BroadcastChange(this);
			}
		}
}

/******************************************************************************
 GetIndexInParent

	The first version asserts that we have a parent.
	The second version returns false if we don't have a parent.

 ******************************************************************************/

JIndex
JTreeNode::GetIndexInParent()
	const
{
	JIndex index;
	const bool found = GetIndexInParent(&index);
	assert( found );
	return index;
}

bool
JTreeNode::GetIndexInParent
	(
	JIndex* index
	)
	const
{
	if (itsParent != nullptr)
		{
		const bool found = itsParent->FindChild(this, index);
		assert( found );
		return true;
		}
	else
		{
		*index = 0;
		return false;
		}
}

/******************************************************************************
 DisconnectFromParent

	Removes us from our parent, if we have one.

 ******************************************************************************/

void
JTreeNode::DisconnectFromParent()
{
	if (itsParent != nullptr)
		{
		itsParent->Remove(this);
		}
}

/******************************************************************************
 GetDescendantCount

	Returns the total number of descendants of this node.

 ******************************************************************************/

JSize
JTreeNode::GetDescendantCount()
	const
{
	JSize count = 0;
	if (itsChildren != nullptr)
		{
		for (const auto* n : *itsChildren)
			{
			count += 1 + n->GetDescendantCount();
			}
		}

	return count;
}

/******************************************************************************
 CollectDescendants

	Fills the list with all of our descendants.  We do not clear the list
	first.

 ******************************************************************************/

void
JTreeNode::CollectDescendants
	(
	JPtrArray<JTreeNode>* list
	)
{
	if (itsChildren != nullptr)
		{
		list->SetCleanUpAction(JPtrArrayT::kForgetAll);

		for (auto* child : *itsChildren)
			{
			list->Append(child);
			child->CollectDescendants(list);
			}
		}
}

/******************************************************************************
 GetChild

	We assert that it has children so clients don't have to.  This is
	consistent because it already asserts if index is invalid, and not
	having any children implies that all indices are invalid.

 ******************************************************************************/

JTreeNode*
JTreeNode::GetChild
	(
	const JIndex index
	)
{
	assert( itsChildren != nullptr );
	return itsChildren->GetElement(index);
}

const JTreeNode*
JTreeNode::GetChild
	(
	const JIndex index
	)
	const
{
	assert( itsChildren != nullptr );
	return itsChildren->GetElement(index);
}

/******************************************************************************
 InsertAtIndex

 ******************************************************************************/

void
JTreeNode::InsertAtIndex
	(
	const JIndex	index,
	JTreeNode*		child
	)
{
	const bool isMove = child->SetParent(this);		// may delete *our* itsChildren list

	CreateChildList();
	itsChildren->InsertAtIndex(index, child);
	BroadcastInsertChild(child, isMove);
}

/******************************************************************************
 InsertBefore

 ******************************************************************************/

void
JTreeNode::InsertBefore
	(
	const JTreeNode*	before,
	JTreeNode*			child
	)
{
	const bool isMove = child->SetParent(this);		// may delete *our* itsChildren list

	CreateChildList();
	itsChildren->InsertBefore(before, child);
	BroadcastInsertChild(child, isMove);
}

/******************************************************************************
 InsertAfter

 ******************************************************************************/

void
JTreeNode::InsertAfter
	(
	const JTreeNode*	after,
	JTreeNode*			child
	)
{
	const bool isMove = child->SetParent(this);		// may delete *our* itsChildren list

	CreateChildList();
	itsChildren->InsertAfter(after, child);
	BroadcastInsertChild(child, isMove);
}

/******************************************************************************
 InsertSorted

 ******************************************************************************/

void
JTreeNode::InsertSorted
	(
	JTreeNode* child
	)
{
	const bool isMove = child->SetParent(this);		// may delete *our* itsChildren list

	CreateChildList();
	if (!itsChildren->Includes(child))
		{
		itsChildren->InsertSorted(child);
		BroadcastInsertChild(child, isMove);
		}
}

/******************************************************************************
 SetChildCompareFunction

	If propagate, sets the same compareFn on all descendants.

 ******************************************************************************/

void
JTreeNode::SetChildCompareFunction
	(
	JListT::CompareResult (*compareFn)(JTreeNode * const &,
											 JTreeNode * const &),
	const JListT::SortOrder	order,
	const bool					propagate
	)
{
	if (compareFn != itsCompareFn || order != itsSortOrder)
		{
		itsCompareFn = compareFn;
		itsSortOrder = order;

		if (itsChildren != nullptr)
			{
			itsChildren->SetCompareFunction(compareFn);
			itsChildren->SetSortOrder(order);
			SortChildren();
			}
		}

	if (propagate && itsChildren != nullptr)
		{
		for (auto* n : *itsChildren)
			{
			n->SetChildCompareFunction(compareFn, order, true);
			}
		}
}

/******************************************************************************
 SortChildren

	Useful for forcing a sort if the children have changed.

 ******************************************************************************/

void
JTreeNode::SortChildren
	(
	const bool propagate
	)
{
	if (itsChildren != nullptr && itsCompareFn != nullptr)
		{
		if (itsTree != nullptr)
			{
			ListenTo(itsChildren);
			}
		itsChildren->Sort();
		if (itsTree != nullptr)
			{
			StopListening(itsChildren);
			}
		}

	if (propagate && itsChildren != nullptr)
		{
		for (auto* n : *itsChildren)
			{
			n->SortChildren(propagate);
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

	We have to broadcast messages while itsChildren is sorted.
	We only get these messages if itsTree != nullptr.

 ******************************************************************************/

void
JTreeNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChildren && message.Is(JListT::kElementMoved))
		{
		assert( itsTree != nullptr );

		const auto* info =
			dynamic_cast<const JListT::ElementMoved*>(&message);
		assert( info != nullptr );

		const JIndex origIndex = info->GetOrigIndex();
		const JIndex newIndex  = info->GetNewIndex();
		JTreeNode* child       = itsChildren->GetElement(newIndex);

		itsChildren->RemoveElement(newIndex);			// avoid another ElementMoved
		itsChildren->InsertAtIndex(origIndex, child);

		itsTree->BroadcastPrepareForMove(child);

		itsChildren->RemoveElement(origIndex);
		itsTree->BroadcastRemove(child);

		itsChildren->InsertAtIndex(newIndex, child);
		itsTree->BroadcastInsert(this, child, newIndex);
		itsTree->BroadcastMoveFinished(child);
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 SetParent (private)

	We always disconnect, even if the parent is the same, because
	JTreeList requires it.

	Returns true if we have a tree and the node will stay in the same tree.

 ******************************************************************************/

bool
JTreeNode::SetParent
	(
	JTreeNode* parent
	)
{
	assert( parent != nullptr );

	bool isMove = false;
	if (itsTree != nullptr && itsParent != nullptr && parent->itsTree == itsTree)
		{
		isMove = true;
		itsTree->BroadcastPrepareForMove(this);
		}

	DisconnectFromParent();
	itsParent = parent;
	SetTree(itsParent->itsTree);

	return isMove;
}

/******************************************************************************
 CreateChildList (private)

 ******************************************************************************/

void
JTreeNode::CreateChildList()
{
	if (itsChildren == nullptr)
		{
		itsChildren = jnew JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
		assert( itsChildren != nullptr );
		itsChildren->SetCompareFunction(itsCompareFn);
		itsChildren->SetSortOrder(itsSortOrder);

		ShouldBeOpenable(true);
		}
}

/******************************************************************************
 BroadcastInsertChild (private)

 ******************************************************************************/

void
JTreeNode::BroadcastInsertChild
	(
	JTreeNode*		child,
	const bool	isMove
	)
{
	if (itsTree != nullptr && itsChildren != nullptr)
		{
		JIndex index;
		const bool found = itsChildren->Find(child, &index);
		assert( found );
		itsTree->BroadcastInsert(this, child, index);

		if (isMove)
			{
			itsTree->BroadcastMoveFinished(child);
			}
		}
}

/******************************************************************************
 Remove

 ******************************************************************************/

void
JTreeNode::Remove
	(
	JTreeNode* child
	)
{
	JIndex index;
	if (!itsIsDestructingFlag && itsChildren != nullptr &&
		itsChildren->Find(child, &index))
		{
		child->itsParent = nullptr;
		child->SetTree(nullptr);

		itsChildren->RemoveElement(index);
		if (itsTree != nullptr)
			{
			itsTree->BroadcastRemove(child);
			}

		if (itsChildren->IsEmpty())
			{
			jdelete itsChildren;
			itsChildren = nullptr;
			}
		}
}

/******************************************************************************
 DeleteAllChildren

 ******************************************************************************/

void
JTreeNode::DeleteAllChildren()
{
	if (itsChildren != nullptr)
		{
		const JSize count = itsChildren->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			jdelete itsChildren->GetElement(i);
			}

		jdelete itsChildren;
		itsChildren = nullptr;
		}
}
