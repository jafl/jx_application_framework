/******************************************************************************
 JTreeList.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JTreeList
#define _H_JTreeList

#include "JContainer.h"
#include "JPtrArray.h"

class JTree;
class JTreeNode;

class JTreeList : public JContainer
{
public:

	JTreeList(JTree* tree);

	~JTreeList() override;

	JTree*			GetTree();
	const JTree*	GetTree() const;

	bool				IndexValid(const JIndex index) const;
	JTreeNode*			GetNode(const JIndex index);
	const JTreeNode*	GetNode(const JIndex index) const;
	bool				FindNode(const JTreeNode* node, JIndex* index) const;

	bool	IsVisible(const JTreeNode* node) const;
	void	MakeVisible(const JTreeNode* node);
	bool	IsOpen(const JIndex index) const;
	bool	IsOpen(const JTreeNode* node) const;
	bool	Open(const JIndex index);
	bool	Open(const JTreeNode* node);
	void	Close(const JIndex index);
	void	Close(const JTreeNode* node);
	void	Toggle(const JIndex index);
	void	Toggle(const JTreeNode* node);

	void	OpenSiblings(const JIndex index);
	void	OpenSiblings(const JTreeNode* node);
	void	CloseSiblings(const JIndex index);
	void	CloseSiblings(const JTreeNode* node);
	bool	OpenDescendants(const JIndex index, const JSize maxDepth);
	bool	OpenDescendants(const JTreeNode* node, const JSize maxDepth);
	void	CloseDescendants(const JIndex index);
	void	CloseDescendants(const JTreeNode* node);

protected:

	virtual bool	ShouldOpenSibling(const JTreeNode* node);
	virtual bool	ShouldOpenDescendant(const JTreeNode* node);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JTree*					itsTree;
	JPtrArray<JTreeNode>*	itsVisibleNodeList;		// contents owned by itsTree
	JPtrArray<JTreeNode>*	itsOpenNodeList;		// contents owned by itsTree
	bool					itsWasOpenBeforeMoveFlag;

private:

	void	InsertElement(const JIndex index, const JTreeNode* node);
	void	InsertBefore(const JTreeNode* parent, const JIndex index,
						 const JTreeNode* node);
	JIndex	FindLastDescendant(const JIndex index) const;

	void	RemoveItem(const JIndex index);

	void	ShowChildren(const JIndex index, const JTreeNode* parent);
	bool	CalledByOpenDescendants(const JTreeNode* node, JSize* depth,
									const JSize maxDepth);

	// not allowed

	JTreeList(const JTreeList&) = delete;
	JTreeList& operator=(const JTreeList&) = delete;

public:		// should be private

	// base class for JBroadcaster messages

	class NodeMessage : public JBroadcaster::Message
		{
		public:

			NodeMessage(const JUtf8Byte* type,
						const JTreeNode* node, const JIndex index)
				:
				JBroadcaster::Message(type),
				itsNode(node),
				itsIndex(index)
			{ };

			JIndex
		GetIndex() const
		{
			return itsIndex;
		};

			const JTreeNode*
		GetNode() const
		{
			return itsNode;
		};

	private:

			const JTreeNode*	itsNode;
			JIndex				itsIndex;
		};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNodeInserted;
	static const JUtf8Byte* kNodeRemoved;
	static const JUtf8Byte* kNodeChanged;

	static const JUtf8Byte* kNodeOpened;
	static const JUtf8Byte* kNodeClosed;

	class NodeInserted : public NodeMessage
		{
		public:

			NodeInserted(const JTreeNode* node, const JIndex index)
				:
				NodeMessage(kNodeInserted, node, index)
			{ };
		};

	class NodeRemoved : public NodeMessage
		{
		public:

			NodeRemoved(const JTreeNode* node, const JIndex index)
				:
				NodeMessage(kNodeRemoved, node, index)
			{ };
		};

	class NodeChanged : public NodeMessage
		{
		public:

			NodeChanged(const JTreeNode* node, const JIndex index)
				:
				NodeMessage(kNodeChanged, node, index)
			{ };
		};

	class NodeOpened : public NodeMessage
		{
		public:

			NodeOpened(const JTreeNode* node, const JIndex index)
				:
				NodeMessage(kNodeOpened, node, index)
			{ };
		};

	class NodeClosed : public NodeMessage
		{
		public:

			NodeClosed(const JTreeNode* node, const JIndex index)
				:
				NodeMessage(kNodeClosed, node, index)
			{ };
		};
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline JTree*
JTreeList::GetTree()
{
	return itsTree;
}

inline const JTree*
JTreeList::GetTree()
	const
{
	return itsTree;
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline bool
JTreeList::IndexValid
	(
	const JIndex index
	)
	const
{
	return itsVisibleNodeList->IndexValid(index);
}

/******************************************************************************
 GetNode

 ******************************************************************************/

inline JTreeNode*
JTreeList::GetNode
	(
	const JIndex index
	)
{
	return itsVisibleNodeList->GetItem(index);
}

inline const JTreeNode*
JTreeList::GetNode
	(
	const JIndex index
	)
	const
{
	return itsVisibleNodeList->GetItem(index);
}

/******************************************************************************
 FindNode

 ******************************************************************************/

inline bool
JTreeList::FindNode
	(
	const JTreeNode*	node,
	JIndex*				index
	)
	const
{
	return itsVisibleNodeList->Find(node, index);
}

/******************************************************************************
 IsVisible

 ******************************************************************************/

inline bool
JTreeList::IsVisible
	(
	const JTreeNode* node
	)
	const
{
	return itsVisibleNodeList->Includes(node);
}

/******************************************************************************
 IsOpen

 ******************************************************************************/

inline bool
JTreeList::IsOpen
	(
	const JIndex index
	)
	const
{
	return itsOpenNodeList->Includes(GetNode(index));
}

inline bool
JTreeList::IsOpen
	(
	const JTreeNode* node
	)
	const
{
	return itsOpenNodeList->Includes(node);
}

/******************************************************************************
 Open

 ******************************************************************************/

inline bool
JTreeList::Open
	(
	const JIndex index
	)
{
	return Open(GetNode(index));
}

/******************************************************************************
 Close

 ******************************************************************************/

inline void
JTreeList::Close
	(
	const JIndex index
	)
{
	Close(GetNode(index));
}

/******************************************************************************
 OpenSiblings

 ******************************************************************************/

inline void
JTreeList::OpenSiblings
	(
	const JIndex index
	)
{
	OpenSiblings(GetNode(index));
}

/******************************************************************************
 CloseSiblings

 ******************************************************************************/

inline void
JTreeList::CloseSiblings
	(
	const JIndex index
	)
{
	CloseSiblings(GetNode(index));
}

/******************************************************************************
 OpenDescendants

 ******************************************************************************/

inline bool
JTreeList::OpenDescendants
	(
	const JIndex	index,
	const JSize		maxDepth
	)
{
	return OpenDescendants(GetNode(index), maxDepth);
}

/******************************************************************************
 CloseDescendants

 ******************************************************************************/

inline void
JTreeList::CloseDescendants
	(
	const JIndex index
	)
{
	CloseDescendants(GetNode(index));
}

#endif
