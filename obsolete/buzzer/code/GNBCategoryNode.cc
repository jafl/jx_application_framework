/******************************************************************************
 GNBCategoryNode.cc

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GNBCategoryNode.h"
#include "GNBNoteNode.h"
#include "GNBToDoNode.h"

#include <JTree.h>

#include <iostream.h>
#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

GNBCategoryNode::GNBCategoryNode
	(
	JNamedTreeNode* parent,
	const char* 	name
	)
	:
	GNBBaseNode(parent, name, kJTrue, GNBBaseNode::kCategoryNode)
{
}

GNBCategoryNode::GNBCategoryNode
	(
	JTree* 			tree,
	const char* 	name
	)
	:
	GNBBaseNode(GNBBaseNode::kCategoryNode, tree, name, kJTrue)
{
}

GNBCategoryNode::GNBCategoryNode
	(
	const GNBCategoryNode&	source,
	JTree* 					tree
	)
	:
	GNBBaseNode(GNBBaseNode::kCategoryNode, tree, source.GetName(), kJTrue)
{
	const JSize count	= source.GetChildCount();
	for (JIndex i = 1; i <= count; i++)
		{
		const JTreeNode* jnode		= source.GetChild(i);
		const GNBBaseNode* bnode	= dynamic_cast<const GNBBaseNode*>(jnode);
		assert(bnode != NULL);
		GNBBaseNode::Type type	= bnode->GetType();
		if (type == GNBBaseNode::kCategoryNode)
			{
			const GNBCategoryNode* cnode	= dynamic_cast<const GNBCategoryNode*>(bnode);
			assert(cnode != NULL);
			GNBCategoryNode* node	= new GNBCategoryNode(*cnode, tree);
			assert(node != NULL);
			Append(node);
			}
		else if (type == GNBBaseNode::kNoteNode)
			{
			const GNBNoteNode* nnode	= dynamic_cast<const GNBNoteNode*>(bnode);
			assert(nnode != NULL);
			GNBNoteNode* node	= new GNBNoteNode(*nnode, tree);
			assert(node != NULL);
			Append(node);
			}
		else if (type == GNBBaseNode::kToDoNode)
			{
			const GNBToDoNode* tdnode	= dynamic_cast<const GNBToDoNode*>(bnode);
			assert(tdnode != NULL);
			GNBToDoNode* node	= new GNBToDoNode(*tdnode, tree);
			assert(node != NULL);
			Append(node);
			}
		}
}


/******************************************************************************
 Destructor

******************************************************************************/

GNBCategoryNode::~GNBCategoryNode()
{
}
