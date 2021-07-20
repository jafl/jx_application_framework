/******************************************************************************
 SyGFileTreeList.cpp

	BASE CLASS = JNamedTreeList

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGFileTreeList.h"
#include "SyGFileTree.h"
#include "SyGFileTreeNode.h"
#include <JDirEntry.h>
#include <jVCSUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFileTreeList::SyGFileTreeList
	(
	SyGFileTree* tree
	)
	:
	JNamedTreeList(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFileTreeList::~SyGFileTreeList()
{
}

/******************************************************************************
 GetSyGFileTree

 ******************************************************************************/

SyGFileTree*
SyGFileTreeList::GetSyGFileTree()
{
	auto* tree = dynamic_cast<SyGFileTree*>(GetTree());
	assert( tree != nullptr );
	return tree;
}

const SyGFileTree*
SyGFileTreeList::GetSyGFileTree()
	const
{
	const auto* tree = dynamic_cast<const SyGFileTree*>(GetTree());
	assert( tree != nullptr );
	return tree;
}

/******************************************************************************
 GetSyGNode

 ******************************************************************************/

SyGFileTreeNode*
SyGFileTreeList::GetSyGNode
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<SyGFileTreeNode*>(GetNode(index));
	assert (node != nullptr);
	return node;
}

const SyGFileTreeNode*
SyGFileTreeList::GetSyGNode
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const SyGFileTreeNode*>(GetNode(index));
	assert (node != nullptr);
	return node;
}

/******************************************************************************
 GetDirEntry

 ******************************************************************************/

const JDirEntry*
SyGFileTreeList::GetDirEntry
	(
	const JIndex index
	)
	const
{
	return (GetSyGNode(index))->GetDirEntry();
}

/******************************************************************************
 ShouldOpenSibling (virtual)

	Returns true if sibling should be opened.

 ******************************************************************************/

bool
SyGFileTreeList::ShouldOpenSibling
	(
	const JTreeNode* node
	)
{
	const auto* fsNode = dynamic_cast<const JFSFileTreeNode*>(node);
	assert( fsNode != nullptr );
	const JDirEntry* entry = fsNode->GetDirEntry();
	return !JIsVCSDirectory(entry->GetName());
}

/******************************************************************************
 ShouldOpenDescendant (virtual)

	Returns true if sibling should be opened.

 ******************************************************************************/

bool
SyGFileTreeList::ShouldOpenDescendant
	(
	const JTreeNode* node
	)
{
	return ShouldOpenSibling(node);
}
