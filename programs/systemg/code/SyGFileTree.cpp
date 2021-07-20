/******************************************************************************
 SyGFileTree.cpp

	BASE CLASS = JFSFileTree

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGFileTree.h"
#include "SyGFileTreeNode.h"
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFileTree::SyGFileTree
	(
	SyGFileTreeNode* root
	)
	:
	JFSFileTree(root)
{
	root->Init();	// build child list
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFileTree::~SyGFileTree()
{
}

/******************************************************************************
 GetDirectory

 ******************************************************************************/

const JString&
SyGFileTree::GetDirectory()
	const
{
	return GetRootDirInfo()->GetDirectory();
}

/******************************************************************************
 Update

	Returns true if anything changed.

 ******************************************************************************/

bool
SyGFileTree::Update
	(
	const bool			force,
	JFSFileTreeNodeBase**	updateNode
	)
{
	if (force || updateNode == nullptr || *updateNode == nullptr)
		{
		return GetSyGRoot()->Update(force, updateNode);
		}
	else
		{
		return (**updateNode).Update(force, updateNode);
		}
}

/******************************************************************************
 HiddenVisible

 ******************************************************************************/

bool
SyGFileTree::HiddenVisible()
	const
{
	return GetRootDirInfo()->HiddenVisible();
}

/******************************************************************************
 ShowHidden

 ******************************************************************************/

void
SyGFileTree::ShowHidden
	(
	const bool show
	)
{
	GetRootDirInfo()->ShowHidden(show);
}

/******************************************************************************
 SetWildcardFilter

 ******************************************************************************/

void
SyGFileTree::SetWildcardFilter
	(
	const JString& filter
	)
{
	GetRootDirInfo()->SetWildcardFilter(filter);
}

/******************************************************************************
 ClearWildcardFilter

 ******************************************************************************/

void
SyGFileTree::ClearWildcardFilter()
{
	GetRootDirInfo()->ClearWildcardFilter();
}

/******************************************************************************
 SetNodeCompareFunction

 ******************************************************************************/

void
SyGFileTree::SetNodeCompareFunction
	(
	const GFMColType type
	)
{
	GetSyGRoot()->SetChildCompareFunction(type);
}

/******************************************************************************
 GetSyGRoot

 ******************************************************************************/

SyGFileTreeNode*
SyGFileTree::GetSyGRoot()
{
	auto* root = dynamic_cast<SyGFileTreeNode*>(GetRoot());
	assert( root != nullptr );
	return root;
}

const SyGFileTreeNode*
SyGFileTree::GetSyGRoot()
	const
{
	const auto* root = dynamic_cast<const SyGFileTreeNode*>(GetRoot());
	assert( root != nullptr );
	return root;
}

/******************************************************************************
 GetRootDirInfo

 ******************************************************************************/

JDirInfo*
SyGFileTree::GetRootDirInfo()
{
	JDirInfo* info;
	const bool ok = GetSyGRoot()->GetDirInfo(&info);
	assert( ok );
	return info;
}

const JDirInfo*
SyGFileTree::GetRootDirInfo()
	const
{
	const JDirInfo* info;
	const bool ok = GetSyGRoot()->GetDirInfo(&info);
	assert( ok );
	return info;
}
