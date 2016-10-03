/******************************************************************************
 JXToolBarNode.cpp

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "JXToolBarNode.h"
#include <JXTextMenu.h>
#include <JTree.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

******************************************************************************/

JXToolBarNode::JXToolBarNode
	(
	JXTextMenu*			menu,
	const JIndex		index,
	const JBoolean		hasSeparator,
	const JBoolean		checked,
	JTree*				tree,
	JNamedTreeNode*		parent,
	const JCharacter*	name
	)
	:
	JNamedTreeNode(tree, name, kJFalse),
	itsMenu(menu),
	itsIndex(index),
	itsHasSeparator(hasSeparator),
	itsIsChecked(checked)
{
	assert(parent != NULL);
	parent->Append(this);
}


/******************************************************************************
 Destructor

******************************************************************************/

JXToolBarNode::~JXToolBarNode()
{
}

/******************************************************************************
 ToggleChecked (public)

 ******************************************************************************/

void
JXToolBarNode::ToggleChecked()
{
	itsIsChecked = !itsIsChecked;

	JTree* tree;
	if (GetTree(&tree))
		{
		tree->JTree::BroadcastChange(this);
		}
}
