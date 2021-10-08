/******************************************************************************
 JTreeNode.h

	Interface for JTreeNode class.

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JTreeNode
#define _H_JTreeNode

#include "jx-af/jcore/JPtrArray.h"

class JTree;

class JTreeNode : public JBroadcaster
{
	friend class JTree;

public:

	JTreeNode(JTree* tree, const bool isOpenable = true);

	~JTreeNode() override;

	bool	IsRoot() const;
	JSize	GetDepth() const;

	JTree*			GetTree();
	const JTree*	GetTree() const;
	bool			HasTree() const;
	bool			GetTree(JTree** tree);
	bool			GetTree(const JTree** tree) const;

	JTreeNode*			GetParent();
	const JTreeNode*	GetParent() const;
	bool				HasParent() const;
	bool				GetParent(JTreeNode** parent);
	bool				GetParent(const JTreeNode** parent) const;
	JIndex				GetIndexInParent() const;
	bool				GetIndexInParent(JIndex* index) const;
	void				DisconnectFromParent();

	bool				IsOpenable() const;
	virtual bool		OKToOpen() const;
	bool				HasChildren() const;
	JSize				GetChildCount() const;
	JTreeNode*			GetChild(const JIndex index);
	const JTreeNode*	GetChild(const JIndex index) const;
	JSize				GetDescendantCount() const;
	void				CollectDescendants(JPtrArray<JTreeNode>* list);
	bool				FindChild(const JTreeNode* child, JIndex* index) const;
	bool				ChildIndexValid(const JIndex index) const;

	void	InsertAtIndex(const JIndex index, JTreeNode* child);
	void	Prepend(JTreeNode* child);
	void	Append(JTreeNode* child);
	void	InsertBefore(const JTreeNode* before, JTreeNode* child);
	void	InsertAfter(const JTreeNode* after, JTreeNode* child);
	void	Remove(JTreeNode* child);
	void	DeleteAllChildren();

	void	InsertSorted(JTreeNode* child);
	bool	GetChildCompareFunction(
				JListT::CompareResult (**compareFn)(JTreeNode * const &,
													JTreeNode * const &),
				JListT::SortOrder* order);
	void	SetChildCompareFunction(
				JListT::CompareResult (*compareFn)(JTreeNode * const &,
												   JTreeNode * const &),
				const JListT::SortOrder order,
				const bool propagate);
	void	SortChildren(const bool propagate = false);

	void	MoveToIndex(JTreeNode* child, const JIndex index);
	void	MoveBefore(const JTreeNode* before, JTreeNode* child);
	void	MoveAfter(const JTreeNode* after, JTreeNode* child);

protected:

	void	ShouldBeOpenable(const bool openable);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JTree*					itsTree;			// not owned; can be nullptr
	JTreeNode*				itsParent;			// owns us; can be nullptr
	JPtrArray<JTreeNode>*	itsChildren;		// can be nullptr
	bool					itsIsOpenableFlag;
	bool					itsIsDestructingFlag;

	JListT::CompareResult (*itsCompareFn)(JTreeNode * const &,
										  JTreeNode * const &);
	JListT::SortOrder	itsSortOrder;

private:

	void	SetTree(JTree* tree);
	bool	SetParent(JTreeNode* parent);

	void	CreateChildList();
	void	BroadcastInsertChild(JTreeNode* child, const bool isMove);

	// not allowed

	JTreeNode(const JTreeNode&) = delete;
	JTreeNode& operator=(const JTreeNode&) = delete;
};


/******************************************************************************
 HasTree

 ******************************************************************************/

inline bool
JTreeNode::HasTree()
	const
{
	return itsTree != nullptr;
}

/******************************************************************************
 GetTree

 ******************************************************************************/

inline bool
JTreeNode::GetTree
	(
	JTree** tree
	)
{
	*tree = itsTree;
	return itsTree != nullptr;
}

inline bool
JTreeNode::GetTree
	(
	const JTree** tree
	)
	const
{
	*tree = itsTree;
	return itsTree != nullptr;
}

/******************************************************************************
 GetDepth

 ******************************************************************************/

inline JSize
JTreeNode::GetDepth()
	const
{
	return ((IsRoot() || itsParent == nullptr) ? 0 : itsParent->GetDepth() + 1);
}

/******************************************************************************
 HasParent

 ******************************************************************************/

inline bool
JTreeNode::HasParent()
	const
{
	return itsParent != nullptr;
}

/******************************************************************************
 GetParent

 ******************************************************************************/

inline bool
JTreeNode::GetParent
	(
	JTreeNode** parent
	)
{
	*parent = itsParent;
	return itsParent != nullptr;
}

inline bool
JTreeNode::GetParent
	(
	const JTreeNode** parent
	)
	const
{
	*parent = itsParent;
	return itsParent != nullptr;
}

/******************************************************************************
 HasChildren

 ******************************************************************************/

inline bool
JTreeNode::HasChildren()
	const
{
	return itsChildren != nullptr;
}

/******************************************************************************
 GetChildCount

 ******************************************************************************/

inline JSize
JTreeNode::GetChildCount()
	const
{
	return (itsChildren != nullptr ? itsChildren->GetElementCount() : 0);
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

inline bool
JTreeNode::FindChild
	(
	const JTreeNode*	child,
	JIndex*				index
	)
	const
{
	*index = 0;
	return itsChildren != nullptr && itsChildren->Find(child, index);
}

/******************************************************************************
 ChildIndexValid

 ******************************************************************************/

inline bool
JTreeNode::ChildIndexValid
	(
	const JIndex index
	)
	const
{
	return (itsChildren != nullptr ? itsChildren->IndexValid(index) : false);
}

/******************************************************************************
 IsOpenable

 ******************************************************************************/

inline bool
JTreeNode::IsOpenable()
	const
{
	return itsIsOpenableFlag;
}

/******************************************************************************
 GetChildCompareFunction

 ******************************************************************************/

inline bool
JTreeNode::GetChildCompareFunction
	(
	JListT::CompareResult (**compareFn)(JTreeNode * const &,
											  JTreeNode * const &),
	JListT::SortOrder* order
	)
{
	*compareFn = itsCompareFn;
	*order     = itsSortOrder;
	return itsCompareFn != nullptr;
}

#endif
