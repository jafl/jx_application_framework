/******************************************************************************
 GAddressBaseTreeNode.cc

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "GAddressBaseTreeNode.h"

#include <JTree.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

GAddressBaseTreeNode::GAddressBaseTreeNode
	(
	JNamedTreeNode* parent,
	const char*	name,
	const JBoolean	isBranch
	)
	:
	JNamedTreeNode((JTree*)NULL, name, isBranch)
{
	if (parent != NULL)
		{
		parent->Append(this);
		}
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

/******************************************************************************
 OKToChange (public)

 ******************************************************************************/

JBoolean
GAddressBaseTreeNode::OKToChange()
	const
{
	return kJTrue;
}
