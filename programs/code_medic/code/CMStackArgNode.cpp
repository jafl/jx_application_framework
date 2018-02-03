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
	const JCharacter*	name,
	const JCharacter*	value
	)
	:
	JNamedTreeNode(NULL, name, kJFalse),
	itsValue(value)
{
	assert( parent != NULL );
	parent->AppendArg(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMStackArgNode::~CMStackArgNode()
{
}
