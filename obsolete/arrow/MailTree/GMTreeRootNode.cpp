/******************************************************************************
 GMTreeRootNode.cc

	Base class for parents of JFSFileTreeNodes.

	BASE CLASS = JNamedTreeNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "GMTreeRootNode.h"
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMTreeRootNode::GMTreeRootNode()
	:
	JFSFileTreeNodeBase(kJTrue)
{
	JBoolean ok	= JDirInfo::Create("/", &itsInfo);
	assert(ok);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMTreeRootNode::~GMTreeRootNode()
{
	delete itsInfo;
}

/******************************************************************************
 GetDirInfo (public)

 ******************************************************************************/

JBoolean
GMTreeRootNode::GetDirInfo
	(
	JDirInfo** info
	)
{
	*info	= itsInfo;
	return kJTrue;
}

/******************************************************************************
 GetDirInfo (public)

 ******************************************************************************/

JBoolean
GMTreeRootNode::GetDirInfo
	(
	const JDirInfo** info
	)
	const
{
	*info	= itsInfo;
	return kJTrue;
}
