/******************************************************************************
 JTreeNode.h

	Interface for JTreeNode class.

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JTreeNode
#define _H_JTreeNode

#include <JPtrArray.h>

class JTree;

class JTreeNode : public JBroadcaster
{
	friend class JTree;

public:

	JTreeNode(JTree* tree, const JBoolean isOpenable = kJTrue);

	virtual ~JTreeNode();

	JBoolean	IsRoot() const;
	JSize		GetDepth() const;

	JTree*			GetTree();
	const JTree*	GetTree() const;
	JBoolean		HasTree() const;
	JBoolean		GetTree(JTree** tree);
	JBoolean		GetTree(const JTree** tree) const;

	JTreeNode*			GetParent();
	const JTreeNode*	GetParent() const;
	JBoolean			HasParent() const;
	JBoolean			GetParent(JTreeNode** parent);
	JBoolean			GetParent(const JTreeNode** parent) const;
	JIndex				GetIndexInParent() const;
	JBoolean			GetIndexInParent(JIndex* index) const;
	void				DisconnectFromParent();

	JBoolean			IsOpenable() const;
	virtual JBoolean	OKToOpen() const;
	JBoolean			HasChildren() const;
	JSize				GetChildCount() const;
	JTreeNode*			GetChild(const JIndex index);
	const JTreeNode*	GetChild(const JIndex index) const;
	JSize				GetDescendantCount() const;
	void				CollectDescendants(JPtrArray<JTreeNode>* list);
	JBoolean			FindChild(const JTreeNode* child, JIndex* index) const;
	JBoolean			ChildIndexValid(const JIndex index) const;

	void	InsertAtIndex(const JIndex index, JTreeNode* child);
	void	Prepend(JTreeNode* child);
	void	Append(JTreeNode* child);
	void	InsertBefore(const JTreeNode* before, JTreeNode* child);
	void	InsertAfter(const JTreeNode* after, JTreeNode* child);
	void	Remove(JTreeNode* child);
	void	DeleteAllChildren();

	void		InsertSorted(JTreeNode* child);
	JBoolean	GetChildCompareFunction(
					JListT::CompareResult (**compareFn)(JTreeNode * const &,
															  JTreeNode * const &),
					JListT::SortOrder* order);
	void		SetChildCompareFunction(
					JListT::CompareResult (*compareFn)(JTreeNode * const &,
															 JTreeNode * const &),
					const JListT::SortOrder order,
					const JBoolean propagate);
	void		SortChildren(const JBoolean propagate = kJFalse);

	void	MoveToIndex(JTreeNode* child, const JIndex index);
	void	MoveBefore(const JTreeNode* before, JTreeNode* child);
	void	MoveAfter(const JTreeNode* after, JTreeNode* child);

protected:

	void	ShouldBeOpenable(const JBoolean openable);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JTree*					itsTree;			// not owned; can be NULL
	JTreeNode*				itsParent;			// owns us; can be NULL
	JPtrArray<JTreeNode>*	itsChildren;		// can be NULL
	JBoolean				itsIsOpenableFlag;
	JBoolean				itsIsDestructingFlag;

	JListT::CompareResult (*itsCompareFn)(JTreeNode * const &,
												JTreeNode * const &);
	JListT::SortOrder	itsSortOrder;

private:

	void		SetTree(JTree* tree);
	JBoolean	SetParent(JTreeNode* parent);

	void	CreateChildList();
	void	BroadcastInsertChild(JTreeNode* child, const JBoolean isMove);

	// not allowed

	JTreeNode(const JTreeNode& source);
	const JTreeNode& operator=(const JTreeNode& source);
};


/******************************************************************************
 HasTree

 ******************************************************************************/

inline JBoolean
JTreeNode::HasTree()
	const
{
	return JI2B( itsTree != NULL );
}

/******************************************************************************
 GetTree

 ******************************************************************************/

inline JBoolean
JTreeNode::GetTree
	(
	JTree** tree
	)
{
	*tree = itsTree;
	return JI2B( itsTree != NULL );
}

inline JBoolean
JTreeNode::GetTree
	(
	const JTree** tree
	)
	const
{
	*tree = itsTree;
	return JI2B( itsTree != NULL );
}

/******************************************************************************
 GetDepth

 ******************************************************************************/

inline JSize
JTreeNode::GetDepth()
	const
{
	return ((IsRoot() || itsParent == NULL) ? 0 : itsParent->GetDepth() + 1);
}

/******************************************************************************
 HasParent

 ******************************************************************************/

inline JBoolean
JTreeNode::HasParent()
	const
{
	return JI2B( itsParent != NULL );
}

/******************************************************************************
 GetParent

 ******************************************************************************/

inline JBoolean
JTreeNode::GetParent
	(
	JTreeNode** parent
	)
{
	*parent = itsParent;
	return JI2B( itsParent != NULL );
}

inline JBoolean
JTreeNode::GetParent
	(
	const JTreeNode** parent
	)
	const
{
	*parent = itsParent;
	return JI2B( itsParent != NULL );
}

/******************************************************************************
 HasChildren

 ******************************************************************************/

inline JBoolean
JTreeNode::HasChildren()
	const
{
	return JI2B( itsChildren != NULL );
}

/******************************************************************************
 GetChildCount

 ******************************************************************************/

inline JSize
JTreeNode::GetChildCount()
	const
{
	return (itsChildren != NULL ? itsChildren->GetElementCount() : 0);
}

/******************************************************************************
 Prepend

 ******************************************************************************/

inline void
JTreeNode::Prepend
	(
	JTreeNode* child
	)
{
	InsertAtIndex(1, child);
}

/******************************************************************************
 Append

 ******************************************************************************/

inline void
JTreeNode::Append
	(
	JTreeNode* child
	)
{
	InsertAtIndex(GetChildCount()+1, child);
}

/******************************************************************************
 Move child

 ******************************************************************************/

inline void
JTreeNode::MoveToIndex
	(
	JTreeNode*		child,
	const JIndex	index
	)
{
	InsertAtIndex(index, child);
}

inline void
JTreeNode::MoveBefore
	(
	const JTreeNode*	before,
	JTreeNode*			child
	)
{
	InsertBefore(before, child);
}

inline void
JTreeNode::MoveAfter
	(
	const JTreeNode*	after,
	JTreeNode*			child
	)
{
	InsertAfter(after, child);
}

/******************************************************************************
 FindChild

 ******************************************************************************/

inline JBoolean
JTreeNode::FindChild
	(
	const JTreeNode*	child,
	JIndex*				index
	)
	const
{
	*index = 0;
	return JI2B( itsChildren != NULL && itsChildren->Find(child, index) );
}

/******************************************************************************
 ChildIndexValid

 ******************************************************************************/

inline JBoolean
JTreeNode::ChildIndexValid
	(
	const JIndex index
	)
	const
{
	return (itsChildren != NULL ? itsChildren->IndexValid(index) : kJFalse);
}

/******************************************************************************
 IsOpenable

 ******************************************************************************/

inline JBoolean
JTreeNode::IsOpenable()
	const
{
	return itsIsOpenableFlag;
}

/******************************************************************************
 GetChildCompareFunction

 ******************************************************************************/

inline JBoolean
JTreeNode::GetChildCompareFunction
	(
	JListT::CompareResult (**compareFn)(JTreeNode * const &,
											  JTreeNode * const &),
	JListT::SortOrder* order
	)
{
	*compareFn = itsCompareFn;
	*order     = itsSortOrder;
	return JI2B( itsCompareFn != NULL );
}

#endif
