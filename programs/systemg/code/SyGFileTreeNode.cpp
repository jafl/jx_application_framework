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
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareUserName, JListT::kSortAscending, true);
		}
	else if (type == kGFMGID)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareGroupName, JListT::kSortAscending, true);
		}
	else if (type == kGFMSize)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareSize, JListT::kSortDescending, true);
		}
	else if (type == kGFMDate)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareDate, JListT::kSortDescending, true);
		}
	else
		{
		JFSFileTreeNode::SetChildCompareFunction(JNamedTreeNode::DynamicCastCompareNames, JListT::kSortAscending, true);
		}
}

/******************************************************************************
 GetSyGFileTree

 ******************************************************************************/

SyGFileTree*
SyGFileTreeNode::GetSyGFileTree()
{
	auto* tree = dynamic_cast<SyGFileTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

const SyGFileTree*
SyGFileTreeNode::GetSyGFileTree()
	const
{
	const auto* tree = dynamic_cast<const SyGFileTree*>(GetTree());
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
	auto* n = dynamic_cast<SyGFileTreeNode*>(p);
	assert( n != nullptr );
	return n;
}

const SyGFileTreeNode*
SyGFileTreeNode::GetSyGParent()
	const
{
	const JTreeNode* p       = GetParent();
	const auto* n = dynamic_cast<const SyGFileTreeNode*>(p);
	assert( n != nullptr );
	return n;
}

bool
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
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

bool
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
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
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
	auto* node = dynamic_cast<SyGFileTreeNode*>(GetChild(index));
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
	const auto* node = dynamic_cast<const SyGFileTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}
