/******************************************************************************
 JTree.cpp

	BASE CLASS = JBroadcaster

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTree.h>
#include <JTreeNode.h>
#include <jAssert.h>

const JCharacter* JTree::kNodeInserted       = "NodeInserted::JTree";
const JCharacter* JTree::kNodeRemoved        = "NodeRemoved::JTree";
const JCharacter* JTree::kNodeDeleted        = "NodeDeleted::JTree";
const JCharacter* JTree::kNodeChanged        = "NodeChanged::JTree";
const JCharacter* JTree::kPrepareForNodeMove = "PrepareForNodeMove::JTree";
const JCharacter* JTree::kNodeMoveFinished   = "NodeMoveFinished::JTree";

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
	assert( root != NULL );

	itsRoot = root;
	itsRoot->ShouldBeOpenable(kJTrue);
	itsRoot->SetTree(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTree::~JTree()
{
	assert( !itsIsDestructingFlag );

	itsIsDestructingFlag = kJTrue;
	delete itsRoot;
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
