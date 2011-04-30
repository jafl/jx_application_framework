/******************************************************************************
 SyGFileTreeList.cpp

	BASE CLASS = JNamedTreeList

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
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
	SyGFileTree* tree = dynamic_cast<SyGFileTree*>(GetTree());
	assert( tree != NULL );
	return tree;
}

const SyGFileTree*
SyGFileTreeList::GetSyGFileTree()
	const
{
	const SyGFileTree* tree = dynamic_cast<const SyGFileTree*>(GetTree());
	assert( tree != NULL );
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
	SyGFileTreeNode* node = dynamic_cast<SyGFileTreeNode*>(GetNode(index));
	assert (node != NULL);
	return node;
}

const SyGFileTreeNode*
SyGFileTreeList::GetSyGNode
	(
	const JIndex index
	)
	const
{
	const SyGFileTreeNode* node = dynamic_cast<const SyGFileTreeNode*>(GetNode(index));
	assert (node != NULL);
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

	Returns kJTrue if sibling should be opened.

 ******************************************************************************/

JBoolean
SyGFileTreeList::ShouldOpenSibling
	(
	const JTreeNode* node
	)
{
	const JFSFileTreeNode* fsNode = dynamic_cast<const JFSFileTreeNode*>(node);
	assert( fsNode != NULL );
	const JDirEntry* entry = fsNode->GetDirEntry();
	return !JIsVCSDirectory(entry->GetName());
}

/******************************************************************************
 ShouldOpenDescendant (virtual)

	Returns kJTrue if sibling should be opened.

 ******************************************************************************/

JBoolean
SyGFileTreeList::ShouldOpenDescendant
	(
	const JTreeNode* node
	)
{
	return ShouldOpenSibling(node);
}
