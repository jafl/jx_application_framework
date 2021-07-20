/******************************************************************************
 SVNRepoTreeList.cpp

	BASE CLASS = JNamedTreeList

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "SVNRepoTreeList.h"
#include "SVNRepoTree.h"
#include "SVNRepoTreeNode.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNRepoTreeList::SVNRepoTreeList
	(
	SVNRepoTree* tree
	)
	:
	JNamedTreeList(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNRepoTreeList::~SVNRepoTreeList()
{
}

/******************************************************************************
 GetRepoTree

 ******************************************************************************/

SVNRepoTree*
SVNRepoTreeList::GetRepoTree()
{
	auto* tree = dynamic_cast<SVNRepoTree*>(GetTree());
	assert( tree != nullptr );
	return tree;
}

const SVNRepoTree*
SVNRepoTreeList::GetRepoTree()
	const
{
	const auto* tree = dynamic_cast<const SVNRepoTree*>(GetTree());
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 GetRepoNode

 ******************************************************************************/

SVNRepoTreeNode*
SVNRepoTreeList::GetRepoNode
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<SVNRepoTreeNode*>(GetNode(index));
	assert (node != nullptr);
	return node;
}

const SVNRepoTreeNode*
SVNRepoTreeList::GetRepoNode
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const SVNRepoTreeNode*>(GetNode(index));
	assert (node != nullptr);
	return node;
}
