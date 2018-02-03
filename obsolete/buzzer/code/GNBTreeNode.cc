/******************************************************************************
 GAddressBaseTreeNode.cc

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "GAddressBaseTreeNode.h"

#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

GAddressBaseTreeNode::GAddressBaseTreeNode
	(
	JTree* 			tree,
	JNamedTreeNode* parent,
	const char* 	name,
	const JBoolean 	isBranch
	)
	:
	JNamedTreeNode(tree, parent, name, isBranch)
{
}


/******************************************************************************
 Destructor

******************************************************************************/

GAddressBaseTreeNode::~GAddressBaseTreeNode()
{
}

/******************************************************************************
 Save (public)

 ******************************************************************************/

void
GAddressBaseTreeNode::Save
	(
	std::ostream& os
	)
{
}