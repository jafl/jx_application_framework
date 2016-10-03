/******************************************************************************
 GNBRootNode.cc

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GNBRootNode.h"

#include <JTree.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

GNBRootNode::GNBRootNode
	(
	JTree* 			tree,
	const char* 	name
	)
	:
	GNBBaseNode(GNBBaseNode::kRootNode, tree, name, kJTrue)
{
}


/******************************************************************************
 Destructor

******************************************************************************/

GNBRootNode::~GNBRootNode()
{
}
