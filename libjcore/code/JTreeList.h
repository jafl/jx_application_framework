/******************************************************************************
 JTreeList.h

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTreeList
#define _H_JTreeList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JPtrArray.h>

class JTree;
class JTreeNode;

class JTreeList : public JContainer
{
public:

	JTreeList(JTree* tree);

	virtual	~JTreeList();

	JTree*			GetTree();
	const JTree*	GetTree() const;

	JBoolean			IndexValid(const JIndex index) const;
	JTreeNode*			GetNode(const JIndex index);
	const JTreeNode*	GetNode(const JIndex index) const;
	JBoolean			FindNode(const JTreeNode* node, JIndex* index) const;

	JBoolean	IsVisible(const JTreeNode* node) const;
	void		MakeVisible(const JTreeNode* node);
	JBoolean	IsOpen(const JIndex index) const;
	JBoolean	IsOpen(const JTreeNode* node) const;
	JBoolean	Open(const JIndex index);
	JBoolean	Open(const JTreeNode* node);
	void		Close(const JIndex index);
	void		Close(const JTreeNode* node);
	void		Toggle(const JIndex index);
	void		Toggle(const JTreeNode* node);

	void		OpenSiblings(const JIndex index);
	void		OpenSiblings(const JTreeNode* node);
	void		CloseSiblings(const JIndex index);
	void		CloseSiblings(const JTreeNode* node);
	JBoolean	OpenDescendants(const JIndex index, const JSize maxDepth);
	JBoolean	OpenDescendants(const JTreeNode* node, const JSize maxDepth);
	void		CloseDescendants(const JIndex index);
	void		CloseDescendants(const JTreeNode* node);

protected:

	virtual JBoolean	ShouldOpenSibling(const JTreeNode* node);
	virtual JBoolean	ShouldOpenDescendant(const JTreeNode* node);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JTree*					itsTree;
	JPtrArray<JTreeNode>*	itsVisibleNodeList;		// contents owned by itsTree
	JPtrArray<JTreeNode>*	itsOpenNodeList;		// contents owned by itsTree
	JBoolean				itsWasOpenBeforeMoveFlag;

private:

	void	InsertElement(const JIndex index, const JTreeNode* node);
	void	InsertBefore(const JTreeNode* parent, const JIndex index,
						 const JTreeNode* node);
	JIndex	FindLastDescendant(const JIndex index) const;

	void	RemoveElement(const JIndex index);

	void		ShowChildren(const JIndex index, const JTreeNode* parent);
	JBoolean	OpenDescendants1(const JTreeNode* node, JSize* depth,
								 const JSize maxDepth);

	// not allowed

	JTreeList(const JTreeList& source);
	const JTreeList& operator=(const JTreeList& source);

public:		// should be private

	// base class for JBroadcaster messages

	class NodeMessage : public JBroadcaster::Message
		{
		public:

			NodeMessage(const JCharacter* type,
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

	static const JCharacter* kNodeInserted;
	static const JCharacter* kNodeRemoved;
	static const JCharacter* kNodeChanged;

	static const JCharacter* kNodeOpened;
	static const JCharacter* kNodeClosed;

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

inline JBoolean
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
	return itsVisibleNodeList->NthElement(index);
}

inline const JTreeNode*
JTreeList::GetNode
	(
	const JIndex index
	)
	const
{
	return itsVisibleNodeList->NthElement(index);
}

/******************************************************************************
 FindNode

 ******************************************************************************/

inline JBoolean
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

inline JBoolean
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

inline JBoolean
JTreeList::IsOpen
	(
	const JIndex index
	)
	const
{
	return itsOpenNodeList->Includes(GetNode(index));
}

inline JBoolean
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

inline JBoolean
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

inline JBoolean
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
