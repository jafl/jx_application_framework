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
	const JUInt64		id,
	const JCharacter*	name,
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
	:
	JNamedTreeNode(nullptr, name, kJFalse),
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
