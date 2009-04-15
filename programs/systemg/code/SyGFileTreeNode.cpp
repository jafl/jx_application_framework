/******************************************************************************
 SyGFileTreeNode.cpp

	BASE CLASS = JFSFileTreeNode

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include <SyGStdInc.h>
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
	JFSFileTreeNode* node = new SyGFileTreeNode(entry);
	assert( node != NULL);
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
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareUserName, JOrderedSetT::kSortAscending, kJTrue);
		}
	else if (type == kGFMGID)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareGroupName, JOrderedSetT::kSortAscending, kJTrue);
		}
	else if (type == kGFMSize)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareSize, JOrderedSetT::kSortDescending, kJTrue);
		}
	else if (type == kGFMDate)
		{
		JFSFileTreeNode::SetChildCompareFunction(JFSFileTreeNode::CompareDate, JOrderedSetT::kSortDescending, kJTrue);
		}
	else
		{
		JFSFileTreeNode::SetChildCompareFunction(JNamedTreeNode::DynamicCastCompareNames, JOrderedSetT::kSortAscending, kJTrue);
		}
}

/******************************************************************************
 GetSyGFileTree

 ******************************************************************************/

SyGFileTree*
SyGFileTreeNode::GetSyGFileTree()
{
	SyGFileTree* tree = dynamic_cast(SyGFileTree*, GetTree());
	assert (tree != NULL);
	return tree;
}

const SyGFileTree*
SyGFileTreeNode::GetSyGFileTree()
	const
{
	const SyGFileTree* tree = dynamic_cast(const SyGFileTree*, GetTree());
	assert (tree != NULL);
	return tree;
}

/******************************************************************************
 GetSyGParent

 ******************************************************************************/

SyGFileTreeNode*
SyGFileTreeNode::GetSyGParent()
{
	JTreeNode* p       = GetParent();
	SyGFileTreeNode* n = dynamic_cast(SyGFileTreeNode*, p);
	assert( n != NULL );
	return n;
}

const SyGFileTreeNode*
SyGFileTreeNode::GetSyGParent()
	const
{
	const JTreeNode* p       = GetParent();
	const SyGFileTreeNode* n = dynamic_cast(const SyGFileTreeNode*, p);
	assert( n != NULL );
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
		*parent = dynamic_cast(SyGFileTreeNode*, p);
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
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
		*parent = dynamic_cast(const SyGFileTreeNode*, p);
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
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
	SyGFileTreeNode* node = dynamic_cast(SyGFileTreeNode*, GetChild(index));
	assert (node != NULL);
	return node;
}

const SyGFileTreeNode*
SyGFileTreeNode::GetSyGChild
	(
	const JIndex index
	)
	const
{
	const SyGFileTreeNode* node = dynamic_cast(const SyGFileTreeNode*, GetChild(index));
	assert (node != NULL);
	return node;
}
