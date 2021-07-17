/******************************************************************************
 CMThreadNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "CMThreadNode.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMThreadNode::CMThreadNode
	(
	const JUInt64	id,
	const JString&	name,
	const JString&	fileName,
	const JIndex	lineIndex
	)
	:
	JNamedTreeNode(nullptr, name, false),
	itsID(id),
	itsFileName(fileName),
	itsLineIndex(lineIndex)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMThreadNode::~CMThreadNode()
{
}
