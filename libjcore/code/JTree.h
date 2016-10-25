/******************************************************************************
 JTree.h

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTree
#define _H_JTree

#include <JBroadcaster.h>

class JTreeNode;

class JTree : virtual public JBroadcaster
{
	friend class JTreeNode;

public:

	JTree(JTreeNode* root);

	virtual	~JTree();

	JTreeNode*			GetRoot();
	const JTreeNode*	GetRoot() const;
	void				SetRoot(JTreeNode* root);

	void	BroadcastChange(JTreeNode* node);

private:

	JTreeNode*	itsRoot;
	JBoolean	itsIsDestructingFlag;

private:

	// called by JTreeNode

	void	BroadcastInsert(JTreeNode* parent, JTreeNode* node, const JIndex index);
	void	BroadcastRemove(JTreeNode* node);
	void	BroadcastDelete(JTreeNode* node);
	void	BroadcastPrepareForMove(JTreeNode* node);
	void	BroadcastMoveFinished(JTreeNode* node);

	// not allowed

	JTree(const JTree& source);
	const JTree& operator=(const JTree& source);

public:

	// base class for JBroadcaster messages

	class NodeMessage : public JBroadcaster::Message
		{
		public:

			JTreeNode*
			GetNode() const
			{
				return itsNode;
			};

		protected:

			NodeMessage(const JUtf8Byte* type, JTreeNode* node)
				:
				JBroadcaster::Message(type),
				itsNode(node)
			{ };

		private:

			JTreeNode* itsNode;
		};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNewRoot;
	static const JUtf8Byte* kNodeInserted;
	static const JUtf8Byte* kNodeRemoved;
	static const JUtf8Byte* kNodeDeleted;
	static const JUtf8Byte* kNodeChanged;

	static const JUtf8Byte* kPrepareForNodeMove;
	static const JUtf8Byte* kNodeMoveFinished;

	class NewRoot : public JBroadcaster::Message
		{
		public:

			NewRoot()
				:
				JBroadcaster::Message(kNewRoot)
			{ };
		};

	class NodeInserted : public NodeMessage
		{
		public:

			NodeInserted(JTreeNode* parent, JTreeNode* node, const JIndex index)
				:
				NodeMessage(kNodeInserted, node),
				itsParent(parent),
				itsIndex(index)
			{ };

			JTreeNode*
			GetParent() const
			{
				return itsParent;
			};

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

		private:

			JTreeNode*	itsParent;
			JIndex		itsIndex;
		};

	class NodeRemoved : public NodeMessage
		{
		public:

			NodeRemoved(JTreeNode* node)
				:
				NodeMessage(kNodeRemoved, node)
			{ };
		};

	class NodeDeleted : public NodeMessage
		{
		public:

			NodeDeleted(JTreeNode* node)
				:
				NodeMessage(kNodeDeleted, node)
			{ };
		};

	class NodeChanged : public NodeMessage
		{
		public:

			NodeChanged(JTreeNode* node)
				:
				NodeMessage(kNodeChanged, node)
			{ };
		};

	class PrepareForNodeMove : public NodeMessage
		{
		public:

			PrepareForNodeMove(JTreeNode* node)
				:
				NodeMessage(kPrepareForNodeMove, node)
			{ };
		};

	class NodeMoveFinished : public NodeMessage
		{
		public:

			NodeMoveFinished(JTreeNode* node)
				:
				NodeMessage(kNodeMoveFinished, node)
			{ };
		};
};


/******************************************************************************
 GetRoot

 ******************************************************************************/

inline JTreeNode*
JTree::GetRoot()
{
	return itsRoot;
}

inline const JTreeNode*
JTree::GetRoot()
	const
{
	return itsRoot;
}

#endif
