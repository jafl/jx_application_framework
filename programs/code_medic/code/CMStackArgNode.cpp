/******************************************************************************
 CMStackArgNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMStackArgNode.h"
#include "CMStackFrameNode.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMStackArgNode::CMStackArgNode
	(
	CMStackFrameNode*	parent,
	const JString&		name,
	const JString&		value
	)
	:
	JNamedTreeNode(nullptr, name, false),
	itsValue(value)
{
	assert( parent != nullptr );
	parent->AppendArg(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMStackArgNode::~CMStackArgNode()
{
}
