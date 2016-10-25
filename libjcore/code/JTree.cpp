/******************************************************************************
 JTree.cpp

	BASE CLASS = JBroadcaster

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <JTree.h>
#include <JTreeNode.h>
#include <jAssert.h>

const JUtf8Byte* JTree::kNewRoot            = "NewRoot::JTree";
const JUtf8Byte* JTree::kNodeInserted       = "NodeInserted::JTree";
const JUtf8Byte* JTree::kNodeRemoved        = "NodeRemoved::JTree";
const JUtf8Byte* JTree::kNodeDeleted        = "NodeDeleted::JTree";
const JUtf8Byte* JTree::kNodeChanged        = "NodeChanged::JTree";
const JUtf8Byte* JTree::kPrepareForNodeMove = "PrepareForNodeMove::JTree";
const JUtf8Byte* JTree::kNodeMoveFinished   = "NodeMoveFinished::JTree";

/******************************************************************************
 Constructor

 ******************************************************************************/

JTree::JTree
	(
	JTreeNode* root
	)
	:
	itsIsDestructingFlag(kJFalse)
{
	SetRoot(root);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTree::~JTree()
{
	assert( !itsIsDestructingFlag );

	itsIsDestructingFlag = kJTrue;
	jdelete itsRoot;
}

/******************************************************************************
 SetRoot

 ******************************************************************************/

void
JTree::SetRoot
	(
	JTreeNode* root
	)
{
	assert( root != NULL );

	itsRoot = root;
	itsRoot->ShouldBeOpenable(kJTrue);
	itsRoot->SetTree(this);

	Broadcast(NewRoot());
}

/******************************************************************************
 BroadcastInsert (protected)

 ******************************************************************************/

void
JTree::BroadcastInsert
	(
	JTreeNode*		parent,
	JTreeNode*		node,
	const JIndex	index
	)
{
	if (!itsIsDestructingFlag)
		{
		Broadcast(NodeInserted(parent, node, index));
		}
}

/******************************************************************************
 BroadcastRemove (protected)

 ******************************************************************************/

void
JTree::BroadcastRemove
	(
	JTreeNode* node
	)
{
	if (!itsIsDestructingFlag)
		{
		Broadcast(NodeRemoved(node));
		}
}

/******************************************************************************
 BroadcastDelete (protected)

 ******************************************************************************/

void
JTree::BroadcastDelete
	(
	JTreeNode* node
	)
{
	if (!itsIsDestructingFlag)
		{
		Broadcast(NodeDeleted(node));
		}
}

/******************************************************************************
 BroadcastChange

 ******************************************************************************/

void
JTree::BroadcastChange
	(
	JTreeNode* node
	)
{
	if (!itsIsDestructingFlag)
		{
		Broadcast(NodeChanged(node));
		}
}

/******************************************************************************
 BroadcastPrepareForMove

 ******************************************************************************/

void
JTree::BroadcastPrepareForMove
	(
	JTreeNode* node
	)
{
	if (!itsIsDestructingFlag)
		{
		Broadcast(PrepareForNodeMove(node));
		}
}

/******************************************************************************
 BroadcastMoveFinished

 ******************************************************************************/

void
JTree::BroadcastMoveFinished
	(
	JTreeNode* node
	)
{
	if (!itsIsDestructingFlag)
		{
		Broadcast(NodeMoveFinished(node));
		}
}
