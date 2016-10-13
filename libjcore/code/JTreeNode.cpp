/******************************************************************************
 JTreeNode.cpp

	Base class for elements stored by JTree.

	BASE CLASS = JBroadcaster
	(not virtual since this is root class, and it needs to be lightweight)

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include <JTreeNode.h>
#include <JTree.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	tree can be NULL.  By not providing a constructor that takes a parent,
	we allow tree to be NULL, and we allow JNamedTreeList to keep a sorted
	list of nodes.

 ******************************************************************************/

JTreeNode::JTreeNode
	(
	JTree*			tree,
	const JBoolean	isOpenable
	)
	:
	itsIsOpenableFlag(isOpenable),
	itsIsDestructingFlag(kJFalse)
{
	itsTree      = tree;
	itsParent    = NULL;
	itsChildren  = NULL;
	itsCompareFn = NULL;
	itsSortOrder = JOrderedSetT::kSortAscending;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTreeNode::~JTreeNode()
{
	itsIsDestructingFlag = kJTrue;

	DeleteAllChildren();
	DisconnectFromParent();

	if (itsTree != NULL)
		{
		itsTree->BroadcastDelete(this);
		}
}

/******************************************************************************
 IsRoot

 ******************************************************************************/

JBoolean
JTreeNode::IsRoot()
	const
{
	return JI2B( (itsTree != NULL && itsTree->GetRoot() == this) ||
				 (itsTree == NULL && itsParent == NULL) );
}

/******************************************************************************
 GetTree

 ******************************************************************************/

JTree*
JTreeNode::GetTree()
{
	assert( itsTree != NULL );
	return itsTree;
}

const JTree*
JTreeNode::GetTree()
	const
{
	assert( itsTree != NULL );
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
		if (itsChildren != NULL)
			{
			const JSize count = itsChildren->GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				(itsChildren->NthElement(i))->SetTree(tree);
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
	assert( itsParent != NULL );
	return itsParent;
}

const JTreeNode*
JTreeNode::GetParent()
	const
{
	assert( itsParent != NULL );
	return itsParent;
}

/******************************************************************************
 OKToOpen (virtual)

	Not inline because it is virtual.

 ******************************************************************************/

JBoolean
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
	const JBoolean openable
	)
{
	if (itsIsOpenableFlag != openable)
		{
		if (!openable)
			{
			DeleteAllChildren();
			}
		itsIsOpenableFlag = openable;
		if (itsTree != NULL)
			{
			itsTree->BroadcastChange(this);
			}
		}
}

/******************************************************************************
 GetIndexInParent

	The first version asserts that we have a parent.
	The second version returns kJFalse if we don't have a parent.

 ******************************************************************************/

JIndex
JTreeNode::GetIndexInParent()
	const
{
	JIndex index;
	const JBoolean found = GetIndexInParent(&index);
	assert( found );
	return index;
}

JBoolean
JTreeNode::GetIndexInParent
	(
	JIndex* index
	)
	const
{
	if (itsParent != NULL)
		{
		const JBoolean found = itsParent->FindChild(this, index);
		assert( found );
		return kJTrue;
		}
	else
		{
		*index = 0;
		return kJFalse;
		}
}

/******************************************************************************
 DisconnectFromParent

	Removes us from our parent, if we have one.

 ******************************************************************************/

void
JTreeNode::DisconnectFromParent()
{
	if (itsParent != NULL)
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
	if (itsChildren != NULL)
		{
		const JSize childCount = itsChildren->GetElementCount();
		for (JIndex i=1; i<=childCount; i++)
			{
			count += 1 + (itsChildren->NthElement(i))->GetDescendantCount();
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
	if (itsChildren != NULL)
		{
		list->SetCleanUpAction(JPtrArrayT::kForgetAll);

		const JSize count = itsChildren->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JTreeNode* child = itsChildren->NthElement(i);
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
	assert( itsChildren != NULL );
	return itsChildren->NthElement(index);
}

const JTreeNode*
JTreeNode::GetChild
	(
	const JIndex index
	)
	const
{
	assert( itsChildren != NULL );
	return itsChildren->NthElement(index);
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
	const JBoolean isMove = child->SetParent(this);		// may delete *our* itsChildren list

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
	const JBoolean isMove = child->SetParent(this);		// may delete *our* itsChildren list

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
	const JBoolean isMove = child->SetParent(this);		// may delete *our* itsChildren list

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
	const JBoolean isMove = child->SetParent(this);		// may delete *our* itsChildren list

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
	JOrderedSetT::CompareResult (*compareFn)(JTreeNode * const &,
											 JTreeNode * const &),
	const JOrderedSetT::SortOrder	order,
	const JBoolean					propagate
	)
{
	if (compareFn != itsCompareFn || order != itsSortOrder)
		{
		itsCompareFn = compareFn;
		itsSortOrder = order;

		if (itsChildren != NULL)
			{
			itsChildren->SetCompareFunction(compareFn);
			itsChildren->SetSortOrder(order);
			SortChildren();
			}
		}

	if (propagate)
		{
		const JSize count = GetChildCount();
		for (JIndex i=1; i<=count; i++)
			{
			(GetChild(i))->SetChildCompareFunction(compareFn, order, kJTrue);
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
	const JBoolean propagate
	)
{
	if (itsChildren != NULL && itsCompareFn != NULL)
		{
		if (itsTree != NULL)
			{
			ListenTo(itsChildren);
			}
		itsChildren->Sort();
		if (itsTree != NULL)
			{
			StopListening(itsChildren);
			}
		}

	if (propagate)
		{
		const JSize count = GetChildCount();
		for (JIndex i=1; i<=count; i++)
			{
			(GetChild(i))->SortChildren(propagate);
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

	We have to broadcast messages while itsChildren is sorted.
	We only get these messages if itsTree != NULL.

 ******************************************************************************/

void
JTreeNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChildren && message.Is(JOrderedSetT::kElementMoved))
		{
		assert( itsTree != NULL );

		const JOrderedSetT::ElementMoved* info =
			dynamic_cast<const JOrderedSetT::ElementMoved*>(&message);
		assert( info != NULL );

		const JIndex origIndex = info->GetOrigIndex();
		const JIndex newIndex  = info->GetNewIndex();
		JTreeNode* child       = itsChildren->NthElement(newIndex);

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

	Returns kJTrue if we have a tree and the node will stay in the same tree.

 ******************************************************************************/

JBoolean
JTreeNode::SetParent
	(
	JTreeNode* parent
	)
{
	assert( parent != NULL );

	JBoolean isMove = kJFalse;
	if (itsTree != NULL && itsParent != NULL && parent->itsTree == itsTree)
		{
		isMove = kJTrue;
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
	if (itsChildren == NULL)
		{
		itsChildren = jnew JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
		assert( itsChildren != NULL );
		itsChildren->SetCompareFunction(itsCompareFn);
		itsChildren->SetSortOrder(itsSortOrder);

		ShouldBeOpenable(kJTrue);
		}
}

/******************************************************************************
 BroadcastInsertChild (private)

 ******************************************************************************/

void
JTreeNode::BroadcastInsertChild
	(
	JTreeNode*		child,
	const JBoolean	isMove
	)
{
	if (itsTree != NULL && itsChildren != NULL)
		{
		JIndex index;
		const JBoolean found = itsChildren->Find(child, &index);
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
	if (!itsIsDestructingFlag && itsChildren != NULL &&
		itsChildren->Find(child, &index))
		{
		child->itsParent = NULL;
		child->SetTree(NULL);

		itsChildren->RemoveElement(index);
		if (itsTree != NULL)
			{
			itsTree->BroadcastRemove(child);
			}

		if (itsChildren->IsEmpty())
			{
			jdelete itsChildren;
			itsChildren = NULL;
			}
		}
}

/******************************************************************************
 DeleteAllChildren

 ******************************************************************************/

void
JTreeNode::DeleteAllChildren()
{
	if (itsChildren != NULL)
		{
		const JSize count = itsChildren->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			jdelete itsChildren->NthElement(i);
			}

		jdelete itsChildren;
		itsChildren = NULL;
		}
}
