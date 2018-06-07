/******************************************************************************
 SyGFileTreeNode.cpp

	BASE CLASS = JFSFileTreeNode

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "SyGFileTreeNode.h"
#include "SyGFileTree.h"
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFileTreeNode::SyGFileTreeNode
	(
	JDirEntry* entry
	)
	:
	JFSFileTreeNode(entry)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFileTreeNode::~SyGFileTreeNode()
{
}

/******************************************************************************
 CreateChild (virtual protected)

 ******************************************************************************/

JFSFileTreeNode*
SyGFileTreeNode::CreateChild
	(
	JDirEntry* entry
	)
{
	JFSFileTreeNode* node = jnew SyGFileTreeNode(entry);
	assert( node != nullptr);
	return node;
}

/******************************************************************************
 SetChildCompareFunction

 ******************************************************************************/

void
SyGFileTreeNode::SetChildCompareFunction
	(
	const GFMColType type
	)
{
	if (type == kGFMUID)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareUserName, JListT::kSortAscending, kJTrue);
		}
	else if (type == kGFMGID)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareGroupName, JListT::kSortAscending, kJTrue);
		}
	else if (type == kGFMSize)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareSize, JListT::kSortDescending, kJTrue);
		}
	else if (type == kGFMDate)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareDate, JListT::kSortDescending, kJTrue);
		}
	else
		{
		JFSFileTreeNode::SetChildCompareFunction(JNamedTreeNode::DynamicCastCompareNames, JListT::kSortAscending, kJTrue);
		}
}

/******************************************************************************
 GetSyGFileTree

 ******************************************************************************/

SyGFileTree*
SyGFileTreeNode::GetSyGFileTree()
{
	SyGFileTree* tree = dynamic_cast<SyGFileTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

const SyGFileTree*
SyGFileTreeNode::GetSyGFileTree()
	const
{
	const SyGFileTree* tree = dynamic_cast<const SyGFileTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

/******************************************************************************
 GetSyGParent

 ******************************************************************************/

SyGFileTreeNode*
SyGFileTreeNode::GetSyGParent()
{
	JTreeNode* p       = GetParent();
	SyGFileTreeNode* n = dynamic_cast<SyGFileTreeNode*>(p);
	assert( n != nullptr );
	return n;
}

const SyGFileTreeNode*
SyGFileTreeNode::GetSyGParent()
	const
{
	const JTreeNode* p       = GetParent();
	const SyGFileTreeNode* n = dynamic_cast<const SyGFileTreeNode*>(p);
	assert( n != nullptr );
	return n;
}

JBoolean
SyGFileTreeNode::GetSyGParent
	(
	SyGFileTreeNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<SyGFileTreeNode*>(p);
		assert( *parent != nullptr );
		return kJTrue;
		}
	else
		{
		*parent = nullptr;
		return kJFalse;
		}
}

JBoolean
SyGFileTreeNode::GetSyGParent
	(
	const SyGFileTreeNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<const SyGFileTreeNode*>(p);
		assert( *parent != nullptr );
		return kJTrue;
		}
	else
		{
		*parent = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 GetSyGChild

 ******************************************************************************/

SyGFileTreeNode*
SyGFileTreeNode::GetSyGChild
	(
	const JIndex index
	)
{
	SyGFileTreeNode* node = dynamic_cast<SyGFileTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const SyGFileTreeNode*
SyGFileTreeNode::GetSyGChild
	(
	const JIndex index
	)
	const
{
	const SyGFileTreeNode* node = dynamic_cast<const SyGFileTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}
