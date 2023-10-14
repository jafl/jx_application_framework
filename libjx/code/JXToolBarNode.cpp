/******************************************************************************
 JXToolBarNode.cpp

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include "JXToolBarNode.h"
#include "JXTextMenu.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

******************************************************************************/

JXToolBarNode::JXToolBarNode
	(
	JXTextMenu*		menu,
	const JIndex	index,
	const bool		hasSeparator,
	const bool		checked,
	JTree*			tree,
	JNamedTreeNode*	parent,
	const JString&	name
	)
	:
	JNamedTreeNode(tree, name, false),
	itsMenu(menu),
	itsIndex(index),
	itsHasSeparator(hasSeparator),
	itsIsChecked(checked)
{
	assert(parent != nullptr);
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
		tree->BroadcastChange(this);
	}
}
